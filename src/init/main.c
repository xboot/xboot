/*
 * init/main.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
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
#include <xboot.h>
#include <init.h>
#include <mode.h>
#include <shell/shell.h>
#include <shell/menu.h>
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/machine.h>
#include <xboot/panic.h>

/*
 * the entry of main function.
 */
int xboot_main(int argc, char *argv[])
{
	/* do all init calls */
	do_initcalls();

	/* do system xtime */
	do_system_xtime();

	/* mount root filesystem */
	do_system_rootfs();

	/* load system configure */
	do_system_cfg();

	/* do anti piracy */
	do_anti_piracy();

	/* wait a moment */
	do_system_wait();

	/* run loop */
	while(1)
	{
		/*
		 * normal mode for booting first menu item's context
		 * and no wait, for the product of final user.
		 */
		if(xboot_get_mode() == MODE_NORMAL)
		{
			run_normal_mode();
		}

		/*
		 * menu mode display a menu for choosing, it can
		 * be used to recovery system, testing, and others,
		 * depend your menu's configure. usually, serviceman
		 * using this mode for repairing or updating machine.
		 */
		else if(xboot_get_mode() == MODE_MENU)
		{
			run_menu_mode();
		}

		/*
		 * shell mode is very powerful. usually, it be used
		 * to debugging machine by engineer, so, with this
		 * mode, you must be careful and familiar with shell
		 * commands. ofcourse, if your are an engineer, just
		 * do what you want to do.
		 */
		else if(xboot_get_mode() == MODE_SHELL)
		{
			run_shell_mode();
		}

		/*
		 * other mode is not supported and system panic.
		 */
		else
		{
			/*
			 * system panic.
			 */
			panic("do not support this mode(%ld), system panic", xboot_get_mode());
		}
	}

	/* do all exit calls */
	do_exitcalls();

	/* xboot return */
	return 0;
}
