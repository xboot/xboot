/*
 * arch/arm/mach-realview/mach-realview.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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
#include <macros.h>
#include <mode.h>
#include <debug.h>
#include <shell/env.h>
#include <shell/menu.h>
#include <xboot/io.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <terminal/terminal.h>
#include <realview/reg-system.h>
#include <realview-cp15.h>


extern x_u8	__text_start[];
extern x_u8 __text_end[];
extern x_u8 __data_shadow_start[];
extern x_u8 __data_shadow_end[];
extern x_u8 __data_start[];
extern x_u8 __data_end[];
extern x_u8 __bss_start[];
extern x_u8 __bss_end[];
extern x_u8 __heap_start[];
extern x_u8 __heap_end[];
extern x_u8 __stack_start[];
extern x_u8 __stack_end[];

/*
 * system initial, like power lock
 */
static void mach_init(void)
{

}

/*
 * system halt, shutdown machine.
 */
static x_bool mach_halt(void)
{
	return FALSE;
}

/*
 * reset the cpu by setting up the watchdog timer and let him time out
 */
static x_bool mach_reset(void)
{
	return FALSE;
}

/*
 * clean up system before running os.
 */
static x_bool mach_cleanup(void)
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
static x_bool mach_genuine(void)
{
	return TRUE;
}

/*
 * change default mode to menu mode.
 */
static x_bool mach_menumode(void)
{
	return FALSE;
}

/*
 * default stdin console. must place NULL at the end.
 */
static struct stdin default_stdin[] = {
	{
		.name		= "tty-kbd"
	}, {
		.name		= "tty-uart0"
	}, {
		.name		= NULL
	}
};

/*
 * default stdout console. must place NULL at the end.
 */
static struct stdout default_stdout[] = {
	{
		.name		= "tty-fb"
	}, {
		.name		= "tty-uart0"
	}, {
		.name		= NULL
	}
};

/*
 * system menu, must place NULL at the end.
 */
static struct menu_item default_menu[] = {
	{
		.name		= "Shell Command Line",
		.context	= "clear; version; exit -s;"
	}, {
		.name		= "Memory Test",
		.context	= "memtest;"
	}, {
		.name		= NULL,
		.context	= NULL
	}
};

/*
 * default environment variable, must place NULL at the end.
 */
static struct env default_env[] = {
	{
		.key	= "prompt",
		.value	= "xboot"
	}, {
		.key	= "linux-machtype",
		.value	= "827"
	}, {
		.key	= "linux-cmdline",
		.value	= "console=tty0, console=ttySAC0"
	}, {
		.key	= NULL,
		.value	= NULL
	}
};

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
		.mem_start			= 0x00000000,
		.mem_end			= 0x00000000 + SZ_128M - 1,

		.xtal				= 12*1000*1000,
	},

	.cfg = {
		.mode				= MODE_MENU,
		.stdin				= default_stdin,
		.stdout				= default_stdout,
		.menu				= default_menu,
		.env				= default_env,
	},

	.link = {
		.text_start			= (const x_sys)__text_start,
		.text_end			= (const x_sys)__text_end,

		.data_shadow_start	= (const x_sys)__data_shadow_start,
		.data_shadow_end	= (const x_sys)__data_shadow_end,

		.data_start			= (const x_sys)__data_start,
		.data_end			= (const x_sys)__data_end,

		.bss_start			= (const x_sys)__bss_start,
		.bss_end			= (const x_sys)__bss_end,

		.heap_start			= (const x_sys)__heap_start,
		.heap_end			= (const x_sys)__heap_end,

		.stack_start		= (const x_sys)__stack_start,
		.stack_end			= (const x_sys)__stack_end,
	},

	.pm = {
		.init 				= mach_init,
		.standby 			= NULL,
		.resume				= NULL,
		.halt				= mach_halt,
		.reset				= mach_reset,
	},

	.misc = {
		.cleanup			= mach_cleanup,
		.genuine			= mach_genuine,
		.menumode			= mach_menumode,
	},

	.priv					= NULL,
};

static __init void mach_realview_init(void)
{
	if(!machine_register(&realview))
		DEBUG_E("failed to register machine 'realview'");
}

module_init(mach_realview_init, LEVEL_MACH);
