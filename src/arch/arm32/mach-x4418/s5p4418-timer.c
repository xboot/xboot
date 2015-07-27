/*
 * s5p4418-timer.c
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

#include <xboot.h>
#include <s5p4418-rstcon.h>
#include <s5p4418/reg-timer.h>
#include <s5p4418-timer.h>

#define TCON_CHANNEL(ch)		(ch ? ch * 4 + 4 : 0)
#define TCON_START(ch)			(0x1 << TCON_CHANNEL(ch))
#define TCON_MANUALUPDATE(ch)	(0x2 << TCON_CHANNEL(ch))
#define TCON_INVERT(ch)			(0x4 << TCON_CHANNEL(ch))
#define TCON_AUTORELOAD(ch)		(0x8 << TCON_CHANNEL(ch))

static inline physical_addr_t s5p4418_timer_base(int ch)
{
	switch(ch)
	{
	case 0:
		return S5P4418_TIMER0_BASE;
	case 1:
		return S5P4418_TIMER1_BASE;
	case 2:
		return S5P4418_TIMER2_BASE;
	case 3:
		return S5P4418_TIMER3_BASE;
	default:
		break;
	}
	return S5P4418_TIMER0_BASE;
}

void s5p4418_timer_reset(void)
{
	s5p4418_ip_reset(RESET_ID_TIMER, 0);
}

void s5p4418_timer_enable(int ch, int irqon)
{
	u32_t val;

	if(ch < 2)
		clk_enable("DIV-TIMER-PRESCALER0");
	else
		clk_enable("DIV-TIMER-PRESCALER1");

	val = read32(phys_to_virt(S5P4418_TIMER_TSTAT));
	val &= ~(0x1f << 5 | 0x1 << ch);
	val |= (0x1 << (ch + 5)) | ((irqon ? 1 : 0) << ch);
	write32(phys_to_virt(S5P4418_TIMER_TSTAT), val);
}

void s5p4418_timer_disable(int ch)
{
	u32_t val;

	val = read32(phys_to_virt(S5P4418_TIMER_TSTAT));
	val &= ~(0x1f << 5 | 0x1 << ch);
	val |= (0x1 << (ch + 5));
	write32(phys_to_virt(S5P4418_TIMER_TSTAT), val);

	val = read32(phys_to_virt(S5P4418_TIMER_TCON));
	val &= ~(TCON_START(ch));
	write32(phys_to_virt(S5P4418_TIMER_TCON), val);

	if(ch< 2)
		clk_disable("DIV-TIMER-PRESCALER0");
	else
		clk_disable("DIV-TIMER-PRESCALER1");
}

void s5p4418_timer_start(int ch, int oneshot)
{
	u32_t val;

	val = read32(phys_to_virt(S5P4418_TIMER_TCON));
	val &= ~(TCON_AUTORELOAD(ch) | TCON_START(ch));
	if(!oneshot)
		val |= TCON_AUTORELOAD(ch);
	val |= TCON_MANUALUPDATE(ch);
	write32(phys_to_virt(S5P4418_TIMER_TCON), val);

	val = read32(phys_to_virt(S5P4418_TIMER_TCON));
	val &= ~(TCON_AUTORELOAD(ch) | TCON_MANUALUPDATE(ch));
	if(!oneshot)
		val |= TCON_AUTORELOAD(ch);
	val |= TCON_START(ch);
	write32(phys_to_virt(S5P4418_TIMER_TCON), val);
}

void s5p4418_timer_stop(int ch)
{
	u32_t val;

	val = read32(phys_to_virt(S5P4418_TIMER_TCON));
	val &= ~(TCON_START(ch));
	write32(phys_to_virt(S5P4418_TIMER_TCON), val);
}

u64_t s5p4418_timer_calc_tin(int ch, u32_t period)
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
	write32(phys_to_virt(S5P4418_TIMER_TCFG1), (read32(phys_to_virt(S5P4418_TIMER_TCFG1)) & ~(0xf<<shift)) | (div<<shift));

	return (rate >> div);
}

void s5p4418_timer_count(int ch, u32_t cnt)
{
	physical_addr_t base = s5p4418_timer_base(ch);

	write32(phys_to_virt(base + TIMER_TCNTB), cnt);
	write32(phys_to_virt(base + TIMER_TCMPB), cnt);
}

u32_t s5p4418_timer_read(int ch)
{
	physical_addr_t base = s5p4418_timer_base(ch);
	return read32(phys_to_virt(base + TIMER_TCNTO));
}

void s5p4418_timer_irq_clear(int ch)
{
	u32_t val;

	val = read32(phys_to_virt(S5P4418_TIMER_TSTAT));
	val &= ~(0x1f << 5);
	val |= (0x1 << (ch + 5));
	write32(phys_to_virt(S5P4418_TIMER_TSTAT), val);
}
