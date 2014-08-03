/*
 * exynos4412-tick.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <exynos4412/reg-timer.h>

static void timer_interrupt(void * data)
{
	tick_interrupt();

	/* Clear interrupt status bit */
	writel(EXYNOS4412_TINT_CSTAT, (readl(EXYNOS4412_TINT_CSTAT) & ~(0x1f<<5)) | (0x01<<9));
}

static bool_t tick_timer_init(void)
{
	u64_t pclk = 66 * 1000 * 1000;

/*
	if(!clk_get_rate("psys-pclk", &pclk))
	{
		LOG("can't get the clock of 'pclk'");
		return FALSE;
	}

	if(!request_irq("TIMER4", timer_interrupt, NULL))
	{
		LOG("can't request irq 'TIMER4'");
		return FALSE;
	}
*/

	/* Using pwm timer 4, prescaler for timer 4 is 16 */
	writel(EXYNOS4412_TCFG0, (readl(EXYNOS4412_TCFG0) & ~(0xff<<8)) | (0x0f<<8));

	/* Select mux input for pwm timer4 is 1/2 */
	writel(EXYNOS4412_TCFG1, (readl(EXYNOS4412_TCFG1) & ~(0xf<<16)) | (0x01<<16));

	/* Load value for 10 ms timeout */
	writel(EXYNOS4412_TCNTB4, (u32_t)(pclk / (2 * 16 * 100)));

	/* Auto load, manaual update of timer 4 and stop timer4 */
	writel(EXYNOS4412_TCON, (readl(EXYNOS4412_TCON) & ~(0x7<<20)) | (0x06<<20));

	/* Enable timer4 interrupt and clear interrupt status bit */
	writel(EXYNOS4412_TINT_CSTAT, (readl(EXYNOS4412_TINT_CSTAT) & ~(0x1<<4)) | (0x01<<4) | (0x01<<9));

	/* Start timer4 */
	writel(EXYNOS4412_TCON, (readl(EXYNOS4412_TCON) & ~(0x7<<20)) | (0x05<<20));

	return TRUE;
}

static struct tick_t exynos4412_tick = {
	.hz			= 100,
	.init		= tick_timer_init,
};

static __init void exynos4412_tick_init(void)
{
	if(register_tick(&exynos4412_tick))
		LOG("Register tick");
	else
		LOG("Failed to register tick");
}
//postcore_initcall(exynos4412_tick_init);
