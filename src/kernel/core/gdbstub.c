/*
 * kernel/core/gdbstub.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <uart/uart.h>
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

static inline void gdb_cpu_register_save(struct gdb_state_t * s, void * regs)
{
	s->cpu->register_save(s->cpu, regs);
}

static inline void gdb_cpu_register_restore(struct gdb_state_t * s, void * regs)
{
	s->cpu->register_restore(s->cpu, regs);
}

static inline int gdb_cpu_register_read(struct gdb_state_t * s, char * buf, int n)
{
	return s->cpu->register_read(s->cpu, buf, n);
}

static inline int gdb_cpu_register_write(struct gdb_state_t * s, char * buf, int n)
{
	return s->cpu->register_write(s->cpu, buf, n);
}

static inline int gdb_cpu_breakpoint_insert(struct gdb_state_t * s, struct gdb_breakpoint_t * bp)
{
	return s->cpu->breakpoint_insert(s->cpu, bp);
}

static inline int gdb_cpu_breakpoint_remove(struct gdb_state_t * s, struct gdb_breakpoint_t * bp)
{
	return s->cpu->breakpoint_remove(s->cpu, bp);
}

static inline void gdb_cpu_singlestep_active(struct gdb_state_t * s)
{
	s->cpu->singlestep_active(s->cpu);
}

static inline void gdb_cpu_singlestep_finish(struct gdb_state_t * s)
{
	s->cpu->singlestep_finish(s->cpu);
}

static inline int gdb_cpu_memory_access(struct gdb_state_t * s, virtual_addr_t addr, virtual_size_t size, int rw)
{
	return s->cpu->memory_acess(s->cpu, addr, size, rw);
}

static inline int gdb_cpu_processor(struct gdb_state_t * s)
{
	return s->cpu->processor(s->cpu);
}

static inline void gdb_cpu_breakpoint(struct gdb_state_t * s)
{
	s->cpu->breakpoint(s->cpu);
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

static struct gdb_breakpoint_t * gdb_breakpoint_search(struct gdb_state_t * s, virtual_addr_t addr)
{
	struct gdb_breakpoint_t * l = (struct gdb_breakpoint_t *)(&s->bp);
	struct gdb_breakpoint_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(l->entry), entry)
	{
		if(pos->addr == addr)
			return pos;
	}
	return NULL;
}

static int gdb_breakpoint_insert(struct gdb_state_t * s, enum gdb_breakpoint_type_t type, virtual_addr_t addr, virtual_size_t size)
{
	struct gdb_breakpoint_t * l = (struct gdb_breakpoint_t *)(&s->bp);
	struct gdb_breakpoint_t * bp;

	if(gdb_breakpoint_search(s, addr))
		return -1;

	bp = malloc(sizeof(struct gdb_breakpoint_t));
	if(!bp)
		return -1;

	bp->type = type;
	bp->addr = addr;
	bp->size = size;
	if(gdb_cpu_breakpoint_insert(s, bp) < 0)
	{
		free(bp);
		return -1;
	}
	list_add_tail(&bp->entry, &(l->entry));
	return 0;
}

static int gdb_breakpoint_remove(struct gdb_state_t * s, virtual_addr_t addr)
{
	struct gdb_breakpoint_t * l = (struct gdb_breakpoint_t *)(&s->bp);
	struct gdb_breakpoint_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(l->entry), entry)
	{
		if(pos->addr == addr)
		{
			gdb_cpu_breakpoint_remove(s, pos);
			list_del(&(pos->entry));
			free(pos);
		}
	}
	return 0;
}

static int gdb_breakpoint_remove_all(struct gdb_state_t * s)
{
	struct gdb_breakpoint_t * l = (struct gdb_breakpoint_t *)(&s->bp);
	struct gdb_breakpoint_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(l->entry), entry)
	{
		gdb_cpu_breakpoint_remove(s, pos);
		list_del(&(pos->entry));
		free(pos);
	}
	return 0;
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

	gdb_cpu_register_save(s, regs);
	gdb_cpu_singlestep_finish(s);

	if(s->connected)
	{
		sprintf(buf, "T%02xthread:%02x;", 5, gdb_cpu_processor(s));
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
			sprintf(buf, "T%02xthread:%02x;", 5, gdb_cpu_processor(s));
			put_packet(s, buf);
			gdb_breakpoint_remove_all(s);
			break;

		case 'g':
			for(len = 0, v = 0; v < gdb_cpu_nregs(s); v++)
			{
				n = gdb_cpu_register_read(s, mem + len, v);
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
				n = gdb_cpu_register_write(s, q, v);
				len -= n;
				q += n;
			}
			put_packet(s, "OK");
			break;

		case 'p':
			v = strtoull(p, (char **)&p, 16);
			n = gdb_cpu_register_read(s, mem, v);
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
			gdb_cpu_register_write(s, mem, v);
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
			if(gdb_cpu_memory_access(s, addr, size, 0) < 0)
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
			if(gdb_cpu_memory_access(s, addr, size, 1) < 0)
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
			if(gdb_breakpoint_remove(s, addr) < 0)
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
			if(gdb_breakpoint_insert(s, v, addr, size) < 0)
				put_packet(s, "");
			else
				put_packet(s, "OK");
			break;

		case 'v':
			if(strncmp(p, "Cont", 4) == 0)
			{
				char action = 0;
				int signal = 0, thread = 0;
				p += 4;
				if(*p == '?')
				{
					put_packet(s, "vCont;c;C;s;S");
					break;
				}
				while(*p)
				{
					if(*p++ != ';')
						break;
					c = *p++;
					if(c == 'C' || c == 'S')
					{
						signal = strtoul(p, (char **)&p, 16);
						if(signal == -1)
							signal = 0;
					}
					else if(c != 'c' && c != 's')
					{
						break;
					}
					if(*p == ':')
						thread = strtoull(p + 1, (char **)&p, 16);
					action = tolower(c);
				}
				if(action)
				{
					if((thread != -1) && (thread != 0) && (thread != gdb_cpu_processor(s)))
					{
                        put_packet(s, "E01");
                        break;
					}
					if(action == 'c')
					{
						gdb_cpu_continue(s);
						break;
					}
					else if(action == 's')
					{
						gdb_cpu_singlestep_active(s);
						gdb_cpu_continue(s);
						break;
					}
				}
				break;
			}
			else
			{
				goto emptypacket;
			}

		case 'k':
			gdb_breakpoint_remove_all(s);
			gdb_cpu_continue(s);
			s->connected = 0;
			break;

		case 'D':
			gdb_breakpoint_remove_all(s);
			gdb_cpu_continue(s);
			s->connected = 0;
			put_packet(s, "OK");
			break;

		case 'H':
			c = *p++;
			v = strtoull(p, (char **)&p, 16);
			if(v == -1 || v == 0)
			{
				put_packet(s, "OK");
				break;
			}
			if(v != gdb_cpu_processor(s))
			{
	            put_packet(s, "E01");
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
				put_packet(s, "E01");
				break;
			}
			break;

		case 'T':
			v = strtoull(p, (char **)&p, 16);
			if(v == gdb_cpu_processor(s))
				put_packet(s, "OK");
			else
				put_packet(s, "E01");
			break;

		case 'q':
		case 'Q':
			if(strcmp(p, "C") == 0)
			{
				put_packet(s, "QC1");
				break;
			}
			else if(strcmp(p, "fThreadInfo") == 0)
			{
				sprintf(buf, "m%x", gdb_cpu_processor(s));
				put_packet(s, buf);
				break;
			}
			else if(strcmp(p, "sThreadInfo") == 0)
			{
				put_packet(s, "l");
				break;
			}
			else if(strncmp(p, "ThreadExtraInfo,", 16) == 0)
			{
				v = strtoull(p + 16, (char **)&p, 16);
				if(v == gdb_cpu_processor(s))
				{
					len = sprintf(mem, "CPU#%d [%s]", v, "running");
					mem_to_hex(buf, (unsigned char *)mem, len);
					put_packet(s, buf);
				}
				break;
			}
	        else if(strcmp(p, "Offsets") == 0)
			{
				sprintf(buf, "Text=%016x;Data=%016x;Bss=%016x", 0, 0, 0);
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
	        else if(is_query_packet(p, "Supported", ':'))
			{
				snprintf(buf, sizeof(buf), "PacketSize=%x", MAX_PACKET_LENGTH);
				put_packet(s, buf);
				break;
			}
	        else if(is_query_packet(p, "Attached", ':'))
	        {
				put_packet(s, "0");
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

	gdb_cpu_register_restore(s, regs);
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

	uart = search_uart(device);
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
	init_list_head(&s->bp.entry);
	s->cpu = cpu;
	s->iface = iface;
	return s;
}

static void gdbserver_free(struct gdb_state_t * s)
{
	if(!s)
		return;
	gdb_breakpoint_remove_all(s);
	free(s->iface);
	free(s);
}

static struct gdb_state_t * gs = 0;

int gdbserver_start(const char * device)
{
	if(!arch_gdb_cpu())
		return -1;
	gdbserver_stop();
	gs = gdbserver_alloc(device);
	if(!gs)
		return -2;
	gdb_cpu_breakpoint(gs);
	return 0;
}

void gdbserver_stop(void)
{
	if(gs)
	{
		gdbserver_free(gs);
		gs = 0;
	}
}

void gdbserver_handle_exception(void * regs)
{
	if(gs)
		gdb_handle_exception(gs, regs);
}
