/*
 * arch/arm/mach-realview/realview-tick.c
 *
 * realview tick timer, using arm sp804 timer module.
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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


#include <configs.h>
#include <default.h>
#include <types.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <time/tick.h>
#include <realview/reg-timer.h>

/*
 * tick timer interrupt.
 */
static void timer_interrupt(void)
{
	tick_interrupt();

	/* clear interrupt - timer3 */
	writel(REALVIEW_T3_ICLR, 0x0);
}

static x_bool tick_timer_init(void)
{
	x_u64 timclk;
	x_u32 count;

	if(!clk_get_rate("timclk", &timclk))
	{
		LOG_E("can't get the clock of \'timclk\'");
		return FALSE;
	}

	/* for 10 ms reload count */
	count = (x_u32)div64(timclk, 100);

	if(!request_irq("TMIER2_3", timer_interrupt))
	{
		LOG_E("can't request irq \'TMIER2_3\'");
		return FALSE;
	}

	/* using timer3 for tick, 10ms for reload value */
	writel(REALVIEW_T3_LOAD, count);

	/* setting timer controller */
	writel(REALVIEW_T3_CTRL, REALVIEW_TC_32BIT | REALVIEW_TC_DIV1 | REALVIEW_TC_IE | REALVIEW_TC_PERIODIC);

	/* clear all interrupt */
	writel(REALVIEW_T3_ICLR, 0x0);

	/* enable timer3 */
	writel(REALVIEW_T3_CTRL, readl(REALVIEW_T3_CTRL) | REALVIEW_TC_ENABLE);

	return TRUE;
}

static struct tick realview_tick = {
	.hz		= 100,
	.init	= tick_timer_init,
};

static __init void realview_tick_init(void)
{
	if(!register_tick(&realview_tick))
		LOG_E("failed to register tick");
}

module_init(realview_tick_init, LEVEL_MACH_RES);
