/*
 * mach-sandbox.c
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

static void mach_init(void)
{

}

static bool_t mach_sleep(void)
{
	return FALSE;
}

static bool_t mach_halt(void)
{
	return FALSE;
}

static bool_t mach_reset(void)
{
	return FALSE;
}

static enum mode_t mach_getmode(void)
{
	return MODE_SHELL;
}

static bool_t mach_batinfo(struct battery_info * info)
{
	if(!info)
		return FALSE;

	info->charging = FALSE;
	info->voltage = 3700;
	info->charge_current = 0;
	info->discharge_current = 300;
	info->temperature = 200;
	info->capacity = 3600;
	info->internal_resistance = 100;
	info->level = 100;

	return TRUE;
}

static bool_t mach_cleanup(void)
{
	return TRUE;
}

static bool_t mach_authentication(void)
{
	return FALSE;
}

/*
 * A portable machine interface.
 */
static struct machine sandbox = {
	.info = {
		.board_name 		= "sandbox",
		.board_desc 		= "arm sandbox emulation baseboard",
		.board_id			= "0xc1400400",

		.cpu_name			= "arm926ejs",
		.cpu_desc			= "arm926ejs",
		.cpu_id				= "0x02000000",
	},

	.res = {
		.mem_banks = {
			[0] = {
				.start		= 0x00000000,
				.end		= 0x00000000 + SZ_128M - 1,
			},

			[1] = {
				.start		= 0,
				.end		= 0,
			},
		},

		.xtal				= 12*1000*1000,
	},

	.link = {
		.text_start			= (const ptrdiff_t)0,
		.text_end			= (const ptrdiff_t)0,

		.romdisk_start		= (const ptrdiff_t)0,
		.romdisk_end		= (const ptrdiff_t)0,

		.data_shadow_start	= (const ptrdiff_t)0,
		.data_shadow_end	= (const ptrdiff_t)0,

		.data_start			= (const ptrdiff_t)0,
		.data_end			= (const ptrdiff_t)0,

		.bss_start			= (const ptrdiff_t)0,
		.bss_end			= (const ptrdiff_t)0,

		.heap_start			= (const ptrdiff_t)0,
		.heap_end			= (const ptrdiff_t)0,

		.stack_start		= (const ptrdiff_t)0,
		.stack_end			= (const ptrdiff_t)0,
	},

	.pm = {
		.init 				= mach_init,
		.sleep				= mach_sleep,
		.halt				= mach_halt,
		.reset				= mach_reset,
	},

	.misc = {
		.getmode			= mach_getmode,
		.batinfo			= mach_batinfo,
		.cleanup			= mach_cleanup,
		.authentication		= mach_authentication,
	},

	.priv					= NULL,
};

static __init void mach_sandbox_init(void)
{
	if(!register_machine(&sandbox))
		LOG_E("failed to register machine 'sandbox'");
}

module_init(mach_sandbox_init, LEVEL_MACH);
