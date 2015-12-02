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
#include <shell/system.h>
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

static inline void gdb_cpu_continue(struct gdb_state_t * s)
{
	s->trapping = 0;
}

static inline int gdb_cpu_nregs(struct gdb_state_t * s)
{
	return s->cpu->nregs;
}

static inline void gdb_cpu_save_register(struct gdb_state_t * s, void * regs)
{
	s->cpu->save_register(s->cpu, regs);
}

static inline void gdb_cpu_restore_register(struct gdb_state_t * s, void * regs)
{
	s->cpu->restore_register(s->cpu, regs);
}

static inline int gdb_cpu_read_register(struct gdb_state_t * s, char * buf, int n)
{
	return s->cpu->read_register(s->cpu, buf, n);
}

static inline int gdb_cpu_write_register(struct gdb_state_t * s, char * buf, int n)
{
	return s->cpu->write_register(s->cpu, buf, n);
}

static inline int gdb_cpu_set_program_counter(struct gdb_state_t * s, virtual_addr_t addr)
{
	return s->cpu->set_program_counter(s->cpu, addr);
}

static inline int gdb_cpu_access_memory(struct gdb_state_t * s, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return s->cpu->acess_memory(s->cpu, addr, size, rw);
}

static inline int gdb_cpu_breakpoint_insert(struct gdb_state_t * s, virtual_addr_t addr, virtual_size_t size, int type)
{
	return s->cpu->breakpoint_insert(s->cpu, addr, size, type);
}

static inline int gdb_cpu_breakpoint_remove(struct gdb_state_t * s, virtual_addr_t addr, virtual_size_t size, int type)
{
	return s->cpu->breakpoint_remove(s->cpu, addr, size, type);
}

static inline int gdb_cpu_breakpoint_remove_all(struct gdb_state_t * s)
{
	return s->cpu->breakpoint_remove_all(s->cpu);
}

static inline void gdb_cpu_breakpoint(struct gdb_state_t * s)
{
	s->cpu->breakpoint(s->cpu);
}

static inline void gdb_cpu_single_step(struct gdb_state_t * s)
{
	s->cpu->singlestep(s->cpu);
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

static void gdb_handle_exception(struct gdb_state_t * s, void * regs)
{
	char packet[MAX_PACKET_LENGTH];
	char buf[MAX_PACKET_LENGTH];
	char mem[MAX_PACKET_LENGTH];
	virtual_addr_t addr;
	virtual_size_t size;
	char c, * p, * q;
	int v, n, len;

	gdb_cpu_save_register(s, regs);

	if(s->connected)
	{
		sprintf(buf, "S%02x", 5);
		put_packet(s, buf);
	}

	s->trapping = 1;
	while(s->trapping)
	{
		p = get_packet(s, packet);
		c = *p++;
		s->connected = 1;

		switch(c)
		{
		case '?':
			sprintf(buf, "S%02x", 5);
			put_packet(s, buf);
			gdb_cpu_breakpoint_remove_all(s);
			break;

		case 'g':
			for(len = 0, v = 0; v < gdb_cpu_nregs(s); v++)
			{
				n = gdb_cpu_read_register(s, mem + len, v);
				len += n;
			}
			mem_to_hex(buf, (unsigned char *)mem, len);
			put_packet(s, buf);
			break;

		case 'G':
			len = strlen(p) / 2;
			hex_to_mem((unsigned char *)mem, p, len);
			for(q = mem, v = 0; (v < gdb_cpu_nregs(s)) && (len > 0); v++)
			{
				n = gdb_cpu_write_register(s, q, v);
				len -= n;
				q += n;
			}
			put_packet(s, "OK");
			break;

		case 'p':
			v = strtoull(p, (char **)&p, 16);
			n = gdb_cpu_read_register(s, mem, v);
			if(n > 0)
			{
				mem_to_hex(buf, (unsigned char *)mem, n);
				put_packet(s, buf);
			}
			else
			{
				put_packet(s, "E01");
			}
			break;

		case 'P':
			v = strtoull(p, (char **)&p, 16);
			if(*p == '=')
				p++;
			n = strlen(p) / 2;
			hex_to_mem((unsigned char *)mem, p, n);
			gdb_cpu_write_register(s, mem, v);
			put_packet(s, "OK");
			break;

		case 'm':
			addr = strtoull(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			size = strtoull(p, NULL, 16);

			if(size > MAX_PACKET_LENGTH / 2)
			{
				put_packet(s, "E01");
				break;
			}
			if(gdb_cpu_access_memory(s, addr, size, 0) < 0)
			{
				put_packet(s, "E01");
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
				put_packet(s, "E01");
				break;
			}
			if(gdb_cpu_access_memory(s, addr, size, 1) < 0)
			{
				put_packet(s, "E01");
			}
			else
			{
				hex_to_mem((unsigned char *)addr, p, size);
				put_packet(s, "OK");
			}
			break;

	    case 'z':
			v = strtoul(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			addr = strtoull(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			size = strtoull(p, (char **)&p, 16);
			if(gdb_cpu_breakpoint_remove(s, addr, size, v) < 0)
				put_packet(s, "");
			else
				put_packet(s, "OK");
			break;

	    case 'Z':
			v = strtoul(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			addr = strtoull(p, (char **)&p, 16);
			if(*p == ',')
				p++;
			size = strtoull(p, (char **)&p, 16);
			if(gdb_cpu_breakpoint_insert(s, addr, size, v) < 0)
				put_packet(s, "");
			else
				put_packet(s, "OK");
			break;

		case 'c':
			if(*p != '\0')
			{
				addr = strtoull(p, (char **)&p, 16);
				gdb_cpu_set_program_counter(s, addr);
			}
			gdb_cpu_continue(s);
			break;

		case 's':
			if(*p != '\0')
			{
				addr = strtoull(p, (char **)&p, 16);
				gdb_cpu_set_program_counter(s, addr);
			}
			gdb_cpu_single_step(s);
			gdb_cpu_continue(s);
			break;

		case 'k':
			gdb_cpu_breakpoint_remove_all(s);
			gdb_cpu_continue(s);
			s->connected = 0;
			break;

		case 'D':
			gdb_cpu_breakpoint_remove_all(s);
			gdb_cpu_continue(s);
			s->connected = 0;
			put_packet(s, "OK");
			break;

		case 'q':
	    case 'Q':
	    	if(is_query_packet(p, "Supported", ':'))
			{
				snprintf(buf, sizeof(buf), "PacketSize=%x", MAX_PACKET_LENGTH);
				put_packet(s, buf);
				break;
			}
	        else if(strncmp(p, "Rcmd,", 5) == 0)
	        {
	        	len = strlen(p + 5);
				if((len % 2) != 0)
				{
					put_packet(s, "E01");
					break;
				}
				len = len / 2;
				hex_to_mem((unsigned char *)mem, p + 5, len);
				mem[len++] = 0;
				system(mem);
				put_packet(s, "OK");
				break;
			}
	        else if(strcmp(p, "Offsets") == 0)
			{
				sprintf(buf, "Text=%016x;Data=%016x;Bss=%016x", 0, 0, 0);
				put_packet(s, buf);
				break;
			}
	        else
	        {
	        	goto emptypacket;
	        }

	    default:
emptypacket:
			buf[0] = 0;
			put_packet(s, buf);
			break;
		}
	}

	gdb_cpu_restore_register(s, regs);
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

static struct gdb_state_t * gdbserver_alloc(const char * device)
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

	s->trapping = 0;
	s->connected = 0;
	s->cpu = cpu;
	s->iface = iface;
	return s;
}

static void gdbserver_free(struct gdb_state_t * s)
{
	if(!s)
		return;
	free(s->iface);
	free(s);
}

static struct gdb_state_t * gs = 0;

int gdbserver_start(const char * device)
{
	if(!arch_gdb_cpu())
		return -2;

	gdbserver_stop();
	gs = gdbserver_alloc(device);
	if(!gs)
		return -1;
	gdb_cpu_breakpoint(gs);
	return 0;
}

void gdbserver_stop(void)
{
	if(!gs)
		return;
	gdbserver_free(gs);
	gs = 0;
}

void gdbserver_handle_exception(void * regs)
{
	if(!gs)
		return;
	gdb_handle_exception(gs, regs);
}
