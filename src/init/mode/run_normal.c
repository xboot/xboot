/*
 * init/mode/run_normal.c
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


#include <configs.h>
#include <default.h>
#include <xboot/menu.h>
#include <shell/exec.h>
#include <mode/mode.h>

/*
 * running the normal mode
 */
void run_normal_mode(void)
{
	struct menu_item * item;

	do {
		/*
		 * set to shell mode
		 */
		xboot_set_mode(MODE_SHELL);

		/*
		 * get the first menu item
		 */
		item = get_menu_indexof_item(0);

		/*
		 * check the item and exec command
		 */
		if(item && item->title && item->command)
		{
			exec_cmdline((const x_s8 *)item->command);
		}
	} while(xboot_get_mode() == MODE_NORMAL);
}
