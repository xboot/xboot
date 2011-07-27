/*
 * arch/arm/mach-smdkv310/mach-smdkv310.c
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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
#include <xboot/machine.h>
#include <s5pv310/reg-wdg.h>

extern u8_t	__text_start[];
extern u8_t __text_end[];
extern u8_t __romdisk_start[];
extern u8_t __romdisk_end[];
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
	return TRUE;
}

/*
 * reset the cpu by setting up the watchdog timer and let him time out
 */
static bool_t mach_reset(void)
{
	/* disable watchdog */
	writel(S5PV310_WTCON, 0x0000);

	/* initialize watchdog timer count register */
	writel(S5PV310_WTCNT, 0x0001);

	/* enable watchdog timer; assert reset at timer timeout */
	writel(S5PV310_WTCON, 0x0021);

	return TRUE;
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
static struct machine smdkv310 = {
	.info = {
		.board_name 		= "SMDKV310",
		.board_desc 		= "Samsung s5pv310 development kit",
		.board_id			= "0x00000000",

		.cpu_name			= "s5pv310",
		.cpu_desc			= "a9 dual-core",
		.cpu_id				= "0x00000000",
	},

	.res = {
		.mem_banks = {
			[0] = {
				.start		= 0x40000000,
				.end		= 0x40000000 + SZ_256M - 1,
			},

			[1] = {
				.start		= 0x50000000,
				.end		= 0x50000000 + SZ_256M - 1,
			},

			[2] = {
				.start		= 0x60000000,
				.end		= 0x60000000 + SZ_256M - 1,
			},

			[3] = {
				.start		= 0x70000000,
				.end		= 0x70000000 + (SZ_256M - 1),
			},

			[4] = {
				.start		= 0x80000000,
				.end		= 0x80000000 + SZ_256M - 1,
			},

			[5] = {
				.start		= 0x90000000,
				.end		= 0x90000000 + SZ_256M - 1,
			},

			[6] = {
				.start		= 0xa0000000,
				.end		= 0xa0000000 + SZ_256M - 1,
			},

			[7] = {
				.start		= 0xb0000000,
				.end		= 0xb0000000 + SZ_256M - 1,
			},

			[8] = {
				.start		= 0,
				.end		= 0,
			},
		},

		.xtal				= 24*1000*1000,
	},

	.link = {
		.text_start			= (const ptrdiff_t)__text_start,
		.text_end			= (const ptrdiff_t)__text_end,

		.romdisk_start		= (const ptrdiff_t)__romdisk_start,
		.romdisk_end		= (const ptrdiff_t)__romdisk_end,

		.data_shadow_start	= (const ptrdiff_t)__data_shadow_start,
		.data_shadow_end	= (const ptrdiff_t)__data_shadow_end,

		.data_start			= (const ptrdiff_t)__data_start,
		.data_end			= (const ptrdiff_t)__data_end,

		.bss_start			= (const ptrdiff_t)__bss_start,
		.bss_end			= (const ptrdiff_t)__bss_end,

		.heap_start			= (const ptrdiff_t)__heap_start,
		.heap_end			= (const ptrdiff_t)__heap_end,

		.stack_start		= (const ptrdiff_t)__stack_start,
		.stack_end			= (const ptrdiff_t)__stack_end,
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

static __init void mach_smdkv310_init(void)
{
	if(!register_machine(&smdkv310))
		LOG_E("failed to register machine 'smdkv310'");
}

module_init(mach_smdkv310_init, LEVEL_MACH);
