/*
 * kernel/core/gdbstub.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <bus/uart.h>
#include <xboot/gdbstub.h>

#define MAX_PACKET_LENGTH	(4096)

static const char hex_asc[] = "0123456789abcdef";

static inline char hex_asc_lo(unsigned char x)
{
	return hex_asc[((x) & 0x0f)];
}

static inline char hex_asc_hi(unsigned char x)
{
	return hex_asc[((x) & 0xf0) >> 4];
}

static int hex_to_bin(char c)
{
	if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	if((c >= '0') && (c <= '9'))
		return c - '0';
	if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	return 0;
}

static void mem_to_hex(char * buf, unsigned char * mem, int len)
{
	char * p = buf;
	int i;

	for(i = 0; i < len; i++)
	{
		*p++ = hex_asc_hi(mem[i]);
		*p++ = hex_asc_lo(mem[i]);
	}
	*p = 0;
}

static void hex_to_mem(unsigned char * mem, char * buf, int len)
{
	int i;

	for(i = 0; i < len; i++)
	{
		mem[i] = (hex_to_bin(buf[0]) << 4) | hex_to_bin(buf[1]);
		buf += 2;
	}
}

static inline int gdb_cpu_nregs(struct gdb_state_t * s)
{
	return s->cpu->nregs;
}

static inline int gdb_cpu_read_register(struct gdb_state_t * s, char * buf, int reg)
{
	return s->cpu->read_register(s->cpu, buf, reg);
}

static inline int gdb_cpu_write_register(struct gdb_state_t * s, char * buf, int reg)
{
	return s->cpu->write_register(s->cpu, buf, reg);
}

static inline int gdb_cpu_set_pc(struct gdb_state_t * s, virtual_addr_t addr)
{
	return s->cpu->set_pc(s->cpu, addr);
}

static inline int gdb_cpu_mem_access(struct gdb_state_t * s, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return s->cpu->mem_access(s->cpu, addr, size, rw);
}

static inline int gdb_interface_read_byte_wait(struct gdb_state_t * s)
{
	char c;

	do {
		if(s->iface->read(s->iface, &c, 1) == 1)
			break;
	} while(1);

	return c;
}

static inline void gdb_interface_write_byte(struct gdb_state_t * s, char c)
{
	s->iface->write(s->iface, &c, 1);
}

static inline void gdb_interface_flush(struct gdb_state_t * s)
{
	s->iface->flush(s->iface);
}

static char * get_packet(struct gdb_state_t * s, char * buf)
{
	unsigned char checksum;
	unsigned char xmitcsum;
	int count;
	char c;

	do {
		while((c = (gdb_interface_read_byte_wait(s))) != '$');

		checksum = 0;
		xmitcsum = -1;
		count = 0;

		while(count < (MAX_PACKET_LENGTH - 1))
		{
			c = gdb_interface_read_byte_wait(s);
			if(c == '#')
				break;
			checksum = checksum + c;
			buf[count] = c;
			count = count + 1;
		}

		if(c == '#')
		{
			xmitcsum = hex_to_bin(gdb_interface_read_byte_wait(s)) << 4;
			xmitcsum += hex_to_bin(gdb_interface_read_byte_wait(s));

			if(checksum != xmitcsum)
				gdb_interface_write_byte(s, '-');
			else
				gdb_interface_write_byte(s, '+');
			gdb_interface_flush(s);
		}
		buf[count] = 0;
	} while(checksum != xmitcsum);

	return buf;
}

static void put_packet(struct gdb_state_t * s, char * buf)
{
	unsigned char checksum;
	int count;
	char c;

	while(1)
	{
		gdb_interface_write_byte(s, '$');
		checksum = 0;
		count = 0;

		while((c = buf[count]) != 0)
		{
			gdb_interface_write_byte(s, c);
			checksum += c;
			count++;
		}

		gdb_interface_write_byte(s, '#');
		gdb_interface_write_byte(s, hex_asc_hi(checksum));
		gdb_interface_write_byte(s, hex_asc_lo(checksum));
		gdb_interface_flush(s);

		c = gdb_interface_read_byte_wait(s);
		if(c == 3)
			c = gdb_interface_read_byte_wait(s);

		if(c == '+')
			return;

		if(c == '$')
		{
			gdb_interface_write_byte(s, '-');
			gdb_interface_flush(s);
			return;
		}
	}
}

static int is_query_packet(const char * p, const char * query, char sep)
{
    int len = strlen(query);
    return strncmp(p, query, len) == 0 && (p[len] == '\0' || p[len] == sep);
}

static void monitor_output(struct gdb_state_t * s, const char * msg, int len)
{
	char buf[MAX_PACKET_LENGTH];

	buf[0] = 'O';
	if(len > (MAX_PACKET_LENGTH / 2) - 1)
		len = (MAX_PACKET_LENGTH / 2) - 1;
	mem_to_hex(buf + 1, (unsigned char *)msg, len);
	put_packet(s, buf);
}

static int gdb_monitor_output(struct gdb_state_t * s, const char * buf, int len)
{
	const char * p = buf;
	int max;

	max = (MAX_PACKET_LENGTH - 2) / 2;
	for(;;)
	{
		if(len <= max)
		{
			monitor_output(s, p, len);
			break;
		}
		monitor_output(s, p, max);
		p += max;
		len -= max;
	}
	return len;
}

void gdb_handle_event(struct gdb_state_t * s)
{
	char packet[MAX_PACKET_LENGTH];
	char buf[MAX_PACKET_LENGTH];
	char mem[MAX_PACKET_LENGTH];
	virtual_addr_t addr;
	virtual_size_t size;
	char c, * p, * q;
	int i, n, len;

	while(1)
	{
		p = get_packet(s, packet);
		c = *p++;

		switch(c)
		{
		case '?':
			sprintf(buf, "T%02xthread:%02x;", 5, 0);
			put_packet(s, buf);
			break;

		case 'q':
	    case 'Q':
			if(is_query_packet(p, "Supported", ':'))
			{
				snprintf(buf, sizeof(buf), "PacketSize=%x", MAX_PACKET_LENGTH);
				put_packet(s, buf);
				break;
			}
			goto emptypacket;

		case 'H':
			c = *p++;
			i = strtoull(p, (char **)&p, 16);
			if(i == -1 || i == 0)
			{
				put_packet(s, "OK");
				break;
			}
			switch(c)
			{
			case 'c':
				put_packet(s, "OK");
				break;
			case 'g':
				put_packet(s, "OK");
				break;
			default:
				put_packet(s, "E22");
				break;
			}
			break;

		case 'g':
			for(len = 0, i = 0; i < gdb_cpu_nregs(s); i++)
			{
				n = gdb_cpu_read_register(s, mem + len, i);
				len += n;
			}
			mem_to_hex(buf, (unsigned char *)mem, len);
			put_packet(s, buf);
			break;

		case 'G':
			len = strlen(p) / 2;
			hex_to_mem((unsigned char *)mem, p, len);
			for(q = mem, i = 0; (i < gdb_cpu_nregs(s)) && (len > 0); i++)
			{
				n = gdb_cpu_write_register(s, q, i);
				len -= n;
				q += n;
			}
			put_packet(s, "OK");
			break;

		case 'p':
			i = strtoull(p, (char **)&p, 16);
			n = gdb_cpu_read_register(s, mem, i);
			if(n > 0)
			{
				mem_to_hex(buf, (unsigned char *)mem, n);
				put_packet(s, buf);
			}
			else
			{
				put_packet(s, "E14");
			}
			break;

		case 'P':
			i = strtoull(p, (char **)&p, 16);
			if(*p == '=')
				p++;
			n = strlen(p) / 2;
			hex_to_mem((unsigned char *)mem, p, n);
			gdb_cpu_write_register(s, mem, i);
			put_packet(s, "OK");
			break;

		case 'm':
			addr = strtoull(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			size = strtoull(p, NULL, 16);

			if(size > MAX_PACKET_LENGTH / 2)
			{
				put_packet(s, "E22");
				break;
			}
			if(gdb_cpu_mem_access(s, addr, size, 0) < 0)
			{
				put_packet(s, "E14");
			}
			else
			{
				mem_to_hex(buf, (unsigned char *)addr, size);
				put_packet(s, buf);
			}
	        break;

		case 'M':
			addr = strtoull(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			size = strtoull(p, (char **)&p, 16);
			if(*p == ':')
				p++;

			if(size > strlen(p) / 2)
			{
				put_packet(s, "E22");
				break;
			}
			if(gdb_cpu_mem_access(s, addr, size, 1) < 0)
			{
				put_packet(s, "E14");
			}
			else
			{
				hex_to_mem((unsigned char *)addr, p, size);
				put_packet(s, "OK");
			}
			break;
/*
		case 'X':
			break;

		case 'D':
			break;

		case 'k':
			break;

		case 'z':
			break;

		case 'Z':
			break;

		case 'c':
			break;

		case 's':
			break;
*/
		default:
emptypacket:
			buf[0] = 0;
			put_packet(s, buf);
			break;
		}
	}
}

static struct gdb_cpu_t * __arch_gdb_cpu(void)
{
	return NULL;
}
extern __typeof(__arch_gdb_cpu) arch_gdb_cpu __attribute__((weak, alias("__arch_gdb_cpu")));

static int gdb_interface_uart_read(struct gdb_iterface_t * iface, char * buf, int count)
{
	struct uart_t * uart = (struct uart_t *)iface->priv;
	return uart->read(uart, (u8_t *)buf, (size_t)count);
}

static int gdb_interface_uart_write(struct gdb_iterface_t * iface, const char * buf, int count)
{
	struct uart_t * uart = (struct uart_t *)iface->priv;
	return uart->write(uart, (u8_t *)buf, (size_t)count);
}

static void gdb_interface_uart_flush(struct gdb_iterface_t * iface)
{
}

struct gdb_state_t * gdbserver_init(const char * device)
{
	struct gdb_state_t * s;
	struct gdb_cpu_t * cpu = arch_gdb_cpu();
	struct gdb_iterface_t * iface;
	struct uart_t * uart;

	if(!cpu)
		return NULL;

	uart = search_bus_uart(device);
	if(!uart)
		return NULL;

	s = malloc(sizeof(struct gdb_state_t));
	if(!s)
		return NULL;

	iface = malloc(sizeof(struct gdb_iterface_t));
	if(!iface)
	{
		free(s);
		return NULL;
	}
	iface->read = gdb_interface_uart_read;
	iface->write = gdb_interface_uart_write;
	iface->flush = gdb_interface_uart_flush;
	iface->priv = uart;

	s->cpu = cpu;
	s->iface = iface;
	return s;
}

void gdbserver_exit(struct gdb_state_t * s)
{
	if(!s)
		return;
	free(s->iface);
	free(s->cpu);
	free(s);
}

void gdbserver_start(struct gdb_state_t * s)
{
	if(s)
		gdb_handle_event(s);
}
