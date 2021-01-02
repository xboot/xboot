/*
 * sys-uart.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>

void sys_uart_init(void)
{
}

void sys_uart_putc(char c)
{
	virtual_addr_t addr = 0x70006040;

	while((read32(addr + 0x14) & (0x1 << 5)) == 0);
	write32(addr + 0x00, c);
}

static int vpf_str_to_num(const char * fmt, int * num)
{
	const char * p;
	int	res, d, isd;

	res = 0;
	for(p = fmt; *fmt != '\0'; p++)
	{
		isd = (*p >= '0' && *p <= '9');
		if(!isd)
			break;
		d = *p - '0';
		res *= 10;
		res += d;
	}
	*num = res;
	return ((int)(p - fmt));
}

static void vpf_num_to_str(uint32_t a, int ish, int pl, int pc)
{
	char buf[32];
	uint32_t base;
	int idx, i, t;

	for(i = 0; i < sizeof(buf); i++)
		buf[i] = pc;
	base = 10;
	if(ish)
		base = 16;

	idx = 0;
	do {
		t = a % base;
		if(t >= 10)
			buf[idx] = t - 10 + 'a';
		else
			buf[idx] = t + '0';
		a /= base;
		idx++;
	} while (a > 0);

	if(pl > 0)
	{
		if(pl >= sizeof(buf))
			pl = sizeof(buf) - 1;
		if(idx < pl)
			idx = pl;
	}
	buf[idx] = '\0';

	for(i = idx - 1; i >= 0; i--)
		sys_uart_putc(buf[i]);
}

static int vpf(const char * fmt, va_list va)
{
	const char * p, * q;
	int	f, c, vai, pl, pc, i;
	unsigned char t;

	pc = ' ';
	for(p = fmt; *p != '\0'; p++)
	{
		f = 0;
		pl = 0;
		c = *p;
		q = p;
		if(*p == '%')
		{
			q = p;
			p++;
			if(*p >= '0' && *p <= '9')
				p += vpf_str_to_num(p, &pl);
			f = *p;
		}
		if((f == 'd') || (f == 'x'))
		{
			vai = va_arg(va, int);
			vpf_num_to_str(vai, f == 'x', pl, pc);
		}
		else
		{
			for(i = 0; i < (p - q); i++)
				sys_uart_putc(q[i]);
			t = (unsigned char)(f != 0 ? f : c);
			sys_uart_putc(t);
		}
	}
	return 0;
}

int sys_uart_print(const char * fmt, ...)
{
	va_list	va;

	va_start(va, fmt);
	vpf(fmt, va);
	va_end(va);
	return 0;
}
