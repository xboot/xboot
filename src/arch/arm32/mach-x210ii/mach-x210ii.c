/*
 * mach-x210ii.c
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
#include <cp15.h>
#include <s5pv210/reg-wdg.h>
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-timer.h>
#include <s5pv210/reg-others.h>

static bool_t mach_detect(void)
{
	return TRUE;
}

static bool_t mach_powerup(void)
{
	writel(S5PV210_PS_HOLD_CONTROL, (readl(S5PV210_PS_HOLD_CONTROL) & ~( 0x00000301 )) | ((0x1<<0) | (0x1<<8) | (0x1<<9)));
	return TRUE;
}

static bool_t mach_shutdown(void)
{
	writel(S5PV210_PS_HOLD_CONTROL, (readl(S5PV210_PS_HOLD_CONTROL) & ~( 0x00000301 )) | ((0x1<<0) | (0x0<<8) | (0x0<<9)));
	return TRUE;
}

static bool_t mach_reset(void)
{
	/* disable watchdog */
	writel(S5PV210_WTCON, 0x0000);

	/* initialize watchdog timer count register */
	writel(S5PV210_WTCNT, 0x0001);

	/* enable watchdog timer; assert reset at timer timeout */
	writel(S5PV210_WTCON, 0x0021);

	return TRUE;
}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_cleanup(void)
{
	/* stop timer 0 ~ 4 */
	writel(S5PV210_TCON, 0x0);

	/* stop system timer */
	writel(S5PV210_SYSTIMER_TCON, 0x0);

	/* disable irq */
	irq_disable();

	/* disable fiq */
	fiq_disable();

	/* disable icache */
	icache_disable();

	/* disable dcache */
	dcache_disable();

	/* disable mmu */
	mmu_disable();

	/* disable vic */
	vic_disable();

	return TRUE;
}

static bool_t mach_authentication(void)
{
	return TRUE;
}

static enum mode_t mach_getmode(void)
{
	/* set GPH2_3 intput and pull up */
	writel(S5PV210_GPH2_BASE + S5PV210_GPIO_CON, (readl(S5PV210_GPH2_BASE + S5PV210_GPIO_CON) & ~(0xf<<12)) | (0x0<<12));
	writel(S5PV210_GPH2_BASE + S5PV210_GPIO_PUD, (readl(S5PV210_GPH2_BASE + S5PV210_GPIO_PUD) & ~(0x3<<6)) | (0x2<<6));

	if((readl(S5PV210_GPH2_BASE + S5PV210_GPIO_DAT) & (0x1<<3)) == 0)
		return MODE_MENU;
	return MODE_NORMAL;
}

static struct machine_t x210ii = {
	.name 				= "x210ii",
	.desc 				= "x210ii based on s5pv210",

	.banks = {
		[0] = {
			.start		= 0x30000000,
			.size		= SZ_256M,
		},

		[1] = {
			.start		= 0x40000000,
			.size		= SZ_256M,
		},

		[2] = {
			.start		= 0,
			.size		= 0,
		},
	},

	.detect 			= mach_detect,
	.powerup			= mach_powerup,
	.shutdown			= mach_shutdown,
	.reset				= mach_reset,
	.sleep				= mach_sleep,
	.cleanup			= mach_cleanup,
	.authentication		= mach_authentication,
	.getmode			= mach_getmode,
};

static __init void mach_x210ii_init(void)
{
	if(register_machine(&x210ii))
		LOG("Register machine '%s'", x210ii.name);
	else
		LOG("Failed to register machine '%s'", x210ii.name);
}

static __exit void mach_x210ii_exit(void)
{
	if(unregister_machine(&x210ii))
		LOG("Unregister machine '%s'", x210ii.name);
	else
		LOG("Failed to unregister machine '%s'", x210ii.name);
}

core_initcall(mach_x210ii_init);
core_exitcall(mach_x210ii_exit);
