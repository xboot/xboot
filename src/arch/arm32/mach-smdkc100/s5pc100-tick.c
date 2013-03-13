/*
 * arch/arm/mach-smdkc100/s5pc100-tick.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <types.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <time/tick.h>
#include <s5pc100/reg-timer.h>

/*
 * tick timer interrupt.
 */
static void timer_interrupt(void)
{
	tick_interrupt();

	/* clear interrupt status bit */
	writel(S5PC100_TINT_CSTAT, (readl(S5PC100_TINT_CSTAT) & ~(0x1f<<5)) | (0x01<<9));
}

static bool_t tick_timer_init(void)
{
	u64_t pclk;

	if(!clk_get_rate("pclk", &pclk))
	{
		LOG_E("can't get the clock of \'pclk\'");
		return FALSE;
	}

	if(!request_irq("TIMER4", timer_interrupt))
	{
		LOG_E("can't request irq \'TIMER4\'");
		return FALSE;
	}

	/* use pwm timer 4, prescaler for timer 4 is 16 */
	writel(S5PC100_TCFG0, (readl(S5PC100_TCFG0) & ~(0xff<<8)) | (0x0f<<8));

	/* select mux input for pwm timer4 is 1/2 */
	writel(S5PC100_TCFG1, (readl(S5PC100_TCFG1) & ~(0xf<<16)) | (0x01<<16));

	/* load value for 10 ms timeout */
	writel(S5PC100_TCNTB4, (u32_t)div64(pclk, (2 * 16 * 100)));

	/* auto load, manaual update of timer 4 and stop timer4 */
	writel(S5PC100_TCON, (readl(S5PC100_TCON) & ~(0x7<<20)) | (0x06<<20));

	/* enable timer4 interrupt and clear interrupt status bit */
	writel(S5PC100_TINT_CSTAT, (readl(S5PC100_TINT_CSTAT) & ~(0x1<<4)) | (0x01<<4) | (0x01<<9));

	/* start timer4 */
	writel(S5PC100_TCON, (readl(S5PC100_TCON) & ~(0x7<<20)) | (0x05<<20));

	return TRUE;
}

static struct tick s5pc100_tick = {
	.hz			= 100,
	.init		= tick_timer_init,
};

static __init void s5pc100_tick_init(void)
{
	if(!register_tick(&s5pc100_tick))
		LOG_E("failed to register tick");
}
core_initcall(s5pc100_tick_init);
