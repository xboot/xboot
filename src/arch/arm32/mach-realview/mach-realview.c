/*
 * mach-realview.c
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

static bool_t mach_detect(void)
{
	return TRUE;
}

static bool_t mach_powerup(void)
{
	return TRUE;
}

static bool_t mach_shutdown(void)
{
	return FALSE;
}

static bool_t mach_reset(void)
{
	return FALSE;
}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_cleanup(void)
{
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

static struct machine_t realview = {
	.name 				= "realview-pb-a8",
	.desc 				= "ARM RealView Platform Baseboard for Cortex-A8",

	.banks = {
		[0] = {
			.start		= 0x70000000,
			.size		= SZ_256M,
		},

		[1] = {
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
};

static __init void mach_realview_init(void)
{
	if(register_machine(&realview))
		LOG("Register machine '%s'", realview.name);
	else
		LOG("Failed to register machine '%s'", realview.name);
}

static __exit void mach_realview_exit(void)
{
	if(unregister_machine(&realview))
		LOG("Unregister machine '%s'", realview.name);
	else
		LOG("Failed to unregister machine '%s'", realview.name);
}

core_initcall(mach_realview_init);
core_exitcall(mach_realview_exit);
