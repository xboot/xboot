/*
 * arch/arm/mach-realview/mach-realview.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <stddef.h>
#include <macros.h>
#include <io.h>
#include <mode/mode.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <realview/reg-system.h>
#include <realview-cp15.h>


extern u8_t	__text_start[];
extern u8_t __text_end[];
extern u8_t __ramdisk_start[];
extern u8_t __ramdisk_end[];
extern u8_t __data_shadow_start[];
extern u8_t __data_shadow_end[];
extern u8_t __data_start[];
extern u8_t __data_end[];
extern u8_t __bss_start[];
extern u8_t __bss_end[];
extern u8_t __heap_start[];
extern u8_t __heap_end[];
extern u8_t __stack_start[];
extern u8_t __stack_end[];

/*
 * system initial, like power lock
 */
static void mach_init(void)
{

}

/*
 * system halt, shutdown machine.
 */
static bool_t mach_halt(void)
{
	return FALSE;
}

/*
 * reset the cpu by setting up the watchdog timer and let him time out
 */
static bool_t mach_reset(void)
{
	return FALSE;
}

/*
 * get system mode
 */
static enum mode mach_getmode(void)
{
	return MODE_MENU;
}

/*
 * clean up system before running os
 */
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

	return TRUE;
}

/*
 * for anti-piracy
 */
static bool_t mach_genuine(void)
{
	return TRUE;
}

/*
 * a portable data interface for machine.
 */
static struct machine realview = {
	.info = {
		.board_name 		= "realview",
		.board_desc 		= "arm realview emulation baseboard",
		.board_id			= "0xc1400400",

		.cpu_name			= "arm926ejs",
		.cpu_desc			= "arm926ejs",
		.cpu_id				= "0x02000000",
	},

	.res = {
		.mem_banks = {
			[0] = {
				.start		= (void *)(0x00000000),
				.end		= (void *)(0x00000000 + SZ_128M - 1),
			},

			[1] = {
				.start		= 0,
				.end		= 0,
			},
		},

		.xtal				= 12*1000*1000,
	},

	.link = {
		.text_start			= (const void *)__text_start,
		.text_end			= (const void *)__text_end,

		.ramdisk_start		= (const void *)__ramdisk_start,
		.ramdisk_end		= (const void *)__ramdisk_end,

		.data_shadow_start	= (const void *)__data_shadow_start,
		.data_shadow_end	= (const void *)__data_shadow_end,

		.data_start			= (const void *)__data_start,
		.data_end			= (const void *)__data_end,

		.bss_start			= (const void *)__bss_start,
		.bss_end			= (const void *)__bss_end,

		.heap_start			= (const void *)__heap_start,
		.heap_end			= (const void *)__heap_end,

		.stack_start		= (const void *)__stack_start,
		.stack_end			= (const void *)__stack_end,
	},

	.pm = {
		.init 				= mach_init,
		.suspend			= NULL,
		.resume				= NULL,
		.halt				= mach_halt,
		.reset				= mach_reset,
	},

	.misc = {
		.getmode			= mach_getmode,
		.cleanup			= mach_cleanup,
		.genuine			= mach_genuine,
	},

	.priv					= NULL,
};

static __init void mach_realview_init(void)
{
	if(!machine_register(&realview))
		LOG_E("failed to register machine 'realview'");
}

module_init(mach_realview_init, LEVEL_MACH);
