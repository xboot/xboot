/*
 * arch/arm/mach-sbc2410x/mach-sbc2410x.c
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
#include <io.h>
#include <shell/env.h>
#include <shell/menu.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <terminal/terminal.h>
#include <s3c2410/reg-wdg.h>


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
 * reset the cpu by setting up the watchdog timer and let him time out
 */
static x_bool mach_reset(void)
{
	/* disable watchdog */
	writel(S3C2410_WTCON, 0x0000);

	/* initialize watchdog timer count register */
	writel(S3C2410_WTCNT, 0x0001);

	/* enable watchdog timer; assert reset at timer timeout */
	writel(S3C2410_WTCON, 0x0021);

	return TRUE;
}

/*
 * system initial, like power lock
 */
static void mach_init(void)
{
	return;
}

/*
 * clean up system before running os.
 */
x_bool mach_cleanup(void)
{
	x_u32 old, tmp;

	/* disable all interrupt */
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (tmp)
			     :
			     : "memory");

	/* disable I-D cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (tmp));
	tmp &= ~(1<<12 | 1<<2);
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (tmp));

	/* flush I/D cache */
	tmp = 0;
	asm ("mcr p15, 0, %0, c7, c7, 0": :"r" (tmp));

	return TRUE;
}

/*
 * for anti-piracy
 */
x_bool mach_genuine(void)
{
	return TRUE;
}

/*
 * change default mode to menu mode.
 */
x_bool mach_menumode(void)
{
	return FALSE;
}

/*
 * default stdin console. must place NULL at the end.
 */
static struct stdin default_stdin[] = {
	{
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
		.name		= "tty-uart0"
	}, {
		.name		= "tty-fb0"
	}, {
		.name		= NULL
	}
};

/*
 * system menu, must place NULL at the end.
 */
static struct menu_item default_menu[] = {
	{
		.name		= "Boot Linux",
		.context	= "clear; echo boot linux;"
	}, {
		.name		= "Shell Command Line",
		.context	= "clear; version; exit -s;"
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
		.value	= "1328"
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
static struct machine sbc2410x = {
	.info = {
		.board_name 		= "sbc2410x",
		.board_desc 		= "frendly arm's single board computer",
		.board_id			= "0",

		.cpu_name			= "s3c2410a",
		.cpu_desc			= "based on arm920t by samsung",
		.cpu_id				= "0x32410002",
	},

	.res = {
		.mem_start			= 0x30000000,
		.mem_end			= 0x30000000 + SZ_64M - 1,

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
		.halt				= NULL,
		.reset				= mach_reset,
	},

	.misc = {
		.cleanup			= mach_cleanup,
		.genuine			= mach_genuine,
		.menumode			= mach_menumode,
	},

	.priv					= NULL,
};

static __init void mach_sbc2410x_init(void)
{
	if(!machine_register(&sbc2410x))
		LOG_E("failed to register machine 'sbc2410x'");
}

module_init(mach_sbc2410x_init, LEVEL_MACH);
