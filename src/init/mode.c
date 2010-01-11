/*
 * init/mode.c
 *
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
#include <mode.h>

/*
 * xboot running mode.
 */
static enum mode xboot_mode = MODE_NORMAL;

/*
 * set xboot's running mode.
 */
x_bool xboot_set_mode(enum mode m)
{
	switch(m)
	{
	case MODE_NORMAL:
		xboot_mode = MODE_NORMAL;
		break;

	case MODE_MENU:
		xboot_mode = MODE_MENU;
		break;

	case MODE_SHELL:
		xboot_mode = MODE_SHELL;
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

/*
 * get xboot's running mode.
 */
enum mode xboot_get_mode(void)
{
	return xboot_mode;
}
