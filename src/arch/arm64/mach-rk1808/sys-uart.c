/*
 * sys-uart.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
	virtual_addr_t addr;
	u32_t val, udiv;

	/* Config GPIO4_C4 and GPIO4_C3 to txd2 and rxd2 */
/*
	addr = 0xff770000 + 0xe020 + ((20 >> 3) << 2);
	val = (((0x3 << 16) | (0x1 & 0x3)) << ((20 & 0x7) << 1));
	write32(addr, val);
	addr = 0xff770000 + 0xe020 + ((19 >> 3) << 2);
	val = (((0x3 << 16) | (0x1 & 0x3)) << ((19 & 0x7) << 1));
	write32(addr, val);
*/

	/* Config uart2 to 115200-8-1-0 */
	addr = 0xff550000;
	write32(addr + 0x88, (1 << 0) | (1 << 1) | (1 << 2));
	write32(addr + 0x04, 0x0);
	write32(addr + 0x10, 0x0);
	write32(addr + 0x98, 0x1);
	write32(addr + 0x9c, 0x3);
	write32(addr + 0xa0, 0x1);

	val = read32(addr + 0x0c);
	val |= (1 << 7);
	write32(addr + 0x0c, val);
	udiv = 24000000 / (16 * 115200);
	write32(addr + 0x00, udiv & 0xff);
	write32(addr + 0x04, (udiv >> 8) & 0xff);
	val = read32(addr + 0x0c);
	val &= ~(1 << 7);
	write32(addr + 0x0c, val);
	val = read32(addr + 0x0c);
	val &= ~0x1f;
	val |= (0x3 << 0) | (0x0 << 2) | (0x0 << 3);
	write32(addr + 0x0c, val);
}

void sys_uart_putc(char c)
{
	virtual_addr_t addr = 0xff550000;

	while((read32(addr + 0x7c) & (0x1 << 1)) == 0);
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
