/*
 * init/main.c
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
#include <init.h>
#include <shell/shell.h>

int xboot_main(int argc, char * argv[])
{
	struct runtime_t rt;

	/* Create runtime */
	runtime_create_save(&rt, 0, 0, 0, 0);

	/* Do initial kobj */
	do_init_kobj();

	/* Do all initial calls */
	do_initcalls();

	/* Mount root filesystem */
	do_system_rootfs();

	/* Display system logo */
	do_system_logo();

	/* System autoboot */
	do_system_autoboot();

	/* Run loop */
	while(1)
	{
		/* Run shell */
		run_shell();

		/* Yield poller task */
		schedule_poller_yield();
	}

	/* Do all exit calls */
	do_exitcalls();

	/* Destroy runtime */
	runtime_destroy_restore(&rt, 0);

	/* Xboot return */
	return 0;
}
