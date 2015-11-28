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

static int hex_to_bin(char ch)
{
	if((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	if((ch >= '0') && (ch <= '9'))
		return ch - '0';
	if((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
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

static int gdb_interface_read_byte_wait(struct gdb_state_t * s)
{
	char ch;

	do {
		if(s->iface->read(s->iface, &ch, 1) == 1)
			break;
	} while(1);

	return ch;
}

static void gdb_interface_write_byte(struct gdb_state_t * s, char ch)
{
	s->iface->write(s->iface, &ch, 1);
}

static void gdb_interface_flush(struct gdb_state_t * s)
{
	s->iface->flush(s->iface);
}

static inline int gdb_cpu_num_register(struct gdb_state_t * s)
{
	return s->cpu->nregs;
}

static int gdb_cpu_read_register(struct gdb_state_t * s, char * buf, int reg)
{
	return s->cpu->read_register(s->cpu, buf, reg);
}

static int gdb_cpu_write_register(struct gdb_state_t * s, char * buf, int reg)
{
	return s->cpu->write_register(s->cpu, buf, reg);
}

static char * get_packet(struct gdb_state_t * s, char * buf)
{
	unsigned char checksum;
	unsigned char xmitcsum;
	int count;
	char ch;

	do {
		while((ch = (gdb_interface_read_byte_wait(s))) != '$');

		checksum = 0;
		xmitcsum = -1;
		count = 0;

		while(count < (MAX_PACKET_LENGTH - 1))
		{
			ch = gdb_interface_read_byte_wait(s);
			if(ch == '#')
				break;
			checksum = checksum + ch;
			buf[count] = ch;
			count = count + 1;
		}

		if(ch == '#')
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
	char ch;

	while(1)
	{
		gdb_interface_write_byte(s, '$');
		checksum = 0;
		count = 0;

		while((ch = buf[count]) != 0)
		{
			gdb_interface_write_byte(s, ch);
			checksum += ch;
			count++;
		}

		gdb_interface_write_byte(s, '#');
		gdb_interface_write_byte(s, hex_asc_hi(checksum));
		gdb_interface_write_byte(s, hex_asc_lo(checksum));
		gdb_interface_flush(s);

		ch = gdb_interface_read_byte_wait(s);
		if(ch == 3)
			ch = gdb_interface_read_byte_wait(s);

		if(ch == '+')
			return;

		if(ch == '$')
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
	unsigned long long addr, len;
	unsigned long thread;
	char * p, * q;
	char ch, type;
	int size;

	while(1)
	{
		p = get_packet(s, packet);
		ch = *p++;

/*		sprintf(buf, "DEBUG:[%c][%s]\r\n", ch, packet);
		gdb_monitor_output(s, buf, strlen(buf));*/
		switch(ch)
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
			type = *p++;
			thread = strtoull(p, (char **) &p, 16);
			if(thread == -1 || thread == 0)
			{
				put_packet(s, "OK");
				break;
			}
			switch(type)
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

		case 'm':
			addr = strtoull(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			len = strtoull(p, NULL, 16);

			if(len > MAX_PACKET_LENGTH / 2)
			{
				put_packet(s, "E22");
				break;
			}

			//xxx if(target_memory_rw_debug(s->g_cpu, addr, mem, len, false) != 0)
			if(0)
			{
				put_packet(s, "E14");
			}
			else
			{
				mem_to_hex(buf, (unsigned char *)addr, len);
				put_packet(s, buf);
			}
	        break;

		case 'M':
			addr = strtoull(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			len = strtoull(p, (char **)&p, 16);
			if(*p == ':')
				p++;

			if(len > strlen(p) / 2)
			{
				put_packet(s, "E22");
				break;
			}
			hex_to_mem((unsigned char *)addr, p, len);
			//xxx if(target_memory_rw_debug(s->g_cpu, addr, mem, len, true) != 0)
			if(0)
			{
				put_packet(s, "E14");
			}
			else
			{
				put_packet(s, "OK");
			}
			break;

		case 'g':
			len = 0;
			for(addr = 0; addr < gdb_cpu_num_register(s); addr++)
			{
				size = gdb_cpu_read_register(s, mem + len, addr);
				len += size;
			}
			mem_to_hex(buf, (unsigned char *)mem, len);
			put_packet(s, buf);
			break;

	    case 'G':
	    	q = mem;
			len = strlen(p) / 2;
			hex_to_mem((unsigned char *)q, p, len);
			for(addr = 0; (addr < gdb_cpu_num_register(s)) && (len > 0); addr++)
			{
				size = gdb_cpu_write_register(s, q, addr);
				len -= size;
				q += size;
			}
			put_packet(s, "OK");
			break;
/*
 		case 'p':
			break;

		case 'P':
			break;

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

static int cpu_read_register(struct gdb_cpu_t * cpu, char * buf, int reg)
{
	if(reg < 33)
	{
		buf[0] = 0;
		buf[1] = 1;
		buf[2] = 2;
		buf[3] = 3;
		buf[4] = 4;
		buf[5] = 5;
		buf[6] = 6;
		buf[7] = 7;
		return 8;
	}
	else if(reg == 33)
	{
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		return 4;
	}

	return 0;
}

static int cpu_write_register(struct gdb_cpu_t * cpu, char * buf, int reg)
{
	if(reg < 33)
	{
		return 8;
	}
	else if(reg == 33)
	{
		return 4;
	}

	return 0;
}

struct gdb_state_t * gdbserver_init(const char * device)
{
	struct gdb_state_t * s;
	struct gdb_iterface_t * iface;
	struct gdb_cpu_t * cpu;
	struct uart_t * uart;

	uart = search_bus_uart(device);
	if(!uart)
		return NULL;

	iface = malloc(sizeof(struct gdb_iterface_t));
	if(!iface)
		return NULL;
	iface->read = gdb_interface_uart_read;
	iface->write = gdb_interface_uart_write;
	iface->flush = gdb_interface_uart_flush;
	iface->priv = uart;

	cpu = malloc(sizeof(struct gdb_cpu_t));
	if(!cpu)
	{
		free(iface);
		return NULL;
	}
	cpu->nregs = 34;
	cpu->read_register = cpu_read_register;
	cpu->write_register = cpu_write_register;
	cpu->priv = 0;

	s = malloc(sizeof(struct gdb_state_t));
	if(!s)
	{
		free(iface);
		free(cpu);
		return NULL;
	}

	s->iface = iface;
	s->cpu = cpu;
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
	gdb_handle_event(s);
}
