/*
 * s5p6818-timer.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <s5p6818-timer.h>

#define TCON_CHANNEL(ch)		(ch ? ch * 4 + 4 : 0)
#define TCON_START(ch)			(0x1 << TCON_CHANNEL(ch))
#define TCON_MANUALUPDATE(ch)	(0x2 << TCON_CHANNEL(ch))
#define TCON_INVERT(ch)			(0x4 << TCON_CHANNEL(ch))
#define TCON_AUTORELOAD(ch)		(0x8 << TCON_CHANNEL(ch))

void s5p6818_timer_reset(void)
{
	s5p6818_ip_reset(RESET_ID_TIMER, 0);
}

void s5p6818_timer_enable(virtual_addr_t virt, int ch, int irqon)
{
	u32_t val;

	if(ch < 2)
		clk_enable("DIV-TIMER-PRESCALER0");
	else
		clk_enable("DIV-TIMER-PRESCALER1");

	val = read32(virt + TIMER_TSTAT);
	val &= ~(0x1f << 5 | 0x1 << ch);
	val |= (0x1 << (ch + 5)) | ((irqon ? 1 : 0) << ch);
	write32(virt + TIMER_TSTAT, val);
}

void s5p6818_timer_disable(virtual_addr_t virt, int ch)
{
	u32_t val;

	val = read32(virt + TIMER_TSTAT);
	val &= ~(0x1f << 5 | 0x1 << ch);
	val |= (0x1 << (ch + 5));
	write32(virt + TIMER_TSTAT, val);

	val = read32(virt + TIMER_TCON);
	val &= ~(TCON_START(ch));
	write32(virt + TIMER_TCON, val);

	if(ch< 2)
		clk_disable("DIV-TIMER-PRESCALER0");
	else
		clk_disable("DIV-TIMER-PRESCALER1");
}

void s5p6818_timer_start(virtual_addr_t virt, int ch, int oneshot)
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

void s5p6818_timer_stop(virtual_addr_t virt, int ch)
{
	u32_t val;

	val = read32(virt + TIMER_TCON);
	val &= ~(TCON_START(ch));
	write32(virt + TIMER_TCON, val);
}

u64_t s5p6818_timer_calc_tin(virtual_addr_t virt, int ch, u32_t period)
{
	u64_t rate, freq = 1000000000L / period;
	u8_t div, shift;

	if(ch < 2)
		rate = clk_get_rate("DIV-TIMER-PRESCALER0");
	else
		rate = clk_get_rate("DIV-TIMER-PRESCALER1");

	for(div = 0; div < 4; div++)
	{
		if((rate >> div) <= freq)
			break;
	}

	shift = ch * 4;
	write32(virt + TIMER_TCFG1, (read32(virt + TIMER_TCFG1) & ~(0xf<<shift)) | (div<<shift));

	return (rate >> div);
}

void s5p6818_timer_count(virtual_addr_t virt, int ch, u32_t cnt)
{
	write32(virt + TIMER_TCNTB(ch), cnt);
	write32(virt + TIMER_TCMPB(ch), cnt);
}

u32_t s5p6818_timer_read(virtual_addr_t virt, int ch)
{
	return read32(virt + TIMER_TCNTO(ch));
}

void s5p6818_timer_irq_clear(virtual_addr_t virt, int ch)
{
	u32_t val;

	val = read32(virt + TIMER_TSTAT);
	val &= ~(0x1f << 5);
	val |= (0x1 << (ch + 5));
	write32(virt + TIMER_TSTAT, val);
}
