/*
 * driver/samsung-timer.c
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

#include <xboot.h>
#include <clk/clk.h>
#include <samsung-timer.h>

#define TIMER_TCFG0				(0x00)
#define TIMER_TCFG1				(0x04)
#define TIMER_TCON				(0x08)
#define TIMER_TSTAT				(0x44)
#define TIMER_TCNTB(x)			((x + 1) * 0xc + 0x00)
#define TIMER_TCMPB(x)			((x + 1) * 0xc + 0x04)
#define TIMER_TCNTO(x)			((x + 1) * 0xc + 0x08)
#define TCON_START(x)			(0x1 << (x ? x * 4 + 4 : 0))
#define TCON_MANUALUPDATE(x)	(0x2 << (x ? x * 4 + 4 : 0))
#define TCON_INVERT(x)			(0x4 << (x ? x * 4 + 4 : 0))
#define TCON_AUTORELOAD(x)		(0x8 << (x ? x * 4 + 4 : 0))

void samsung_timer_enable(virtual_addr_t virt, int ch, int irqon)
{
	u32_t val;

	val = read32(virt + TIMER_TSTAT);
	val &= ~(0x1f << 5 | 0x1 << ch);
	val |= (0x1 << (ch + 5)) | ((irqon ? 1 : 0) << ch);
	write32(virt + TIMER_TSTAT, val);
}

void samsung_timer_disable(virtual_addr_t virt, int ch)
{
	u32_t val;

	val = read32(virt + TIMER_TSTAT);
	val &= ~(0x1f << 5 | 0x1 << ch);
	val |= (0x1 << (ch + 5));
	write32(virt + TIMER_TSTAT, val);

	val = read32(virt + TIMER_TCON);
	val &= ~(TCON_START(ch));
	write32(virt + TIMER_TCON, val);
}

void samsung_timer_start(virtual_addr_t virt, int ch, int oneshot)
{
	u32_t val;

	val = read32(virt + TIMER_TCON);
	val &= ~(TCON_AUTORELOAD(ch) | TCON_START(ch));
	if(!oneshot)
		val |= TCON_AUTORELOAD(ch);
	val |= TCON_MANUALUPDATE(ch);
	write32(virt + TIMER_TCON, val);

	val = read32(virt + TIMER_TCON);
	val &= ~(TCON_AUTORELOAD(ch) | TCON_MANUALUPDATE(ch));
	if(!oneshot)
		val |= TCON_AUTORELOAD(ch);
	val |= TCON_START(ch);
	write32(virt + TIMER_TCON, val);
}

void samsung_timer_stop(virtual_addr_t virt, int ch)
{
	u32_t val;

	val = read32(virt + TIMER_TCON);
	val &= ~(TCON_START(ch));
	write32(virt + TIMER_TCON, val);
}

u64_t samsung_timer_calc_tin(virtual_addr_t virt, const char * clk, int ch, u32_t period)
{
	u64_t rate, freq = 1000000000L / period;
	u8_t div, shift;

	rate = clk_get_rate(clk);
	for(div = 0; div < 4; div++)
	{
		if((rate >> div) <= freq)
			break;
	}

	shift = ch * 4;
	write32(virt + TIMER_TCFG1, (read32(virt + TIMER_TCFG1) & ~(0xf<<shift)) | (div<<shift));

	return (rate >> div);
}

void samsung_timer_count(virtual_addr_t virt, int ch, u32_t cnt)
{
	write32(virt + TIMER_TCNTB(ch), cnt);
	write32(virt + TIMER_TCMPB(ch), cnt);
}

u32_t samsung_timer_read(virtual_addr_t virt, int ch)
{
	return read32(virt + TIMER_TCNTO(ch));
}

void samsung_timer_irq_clear(virtual_addr_t virt, int ch)
{
	u32_t val;

	val = read32(virt + TIMER_TSTAT);
	val &= ~(0x1f << 5);
	val |= (0x1 << (ch + 5));
	write32(virt + TIMER_TSTAT, val);
}
