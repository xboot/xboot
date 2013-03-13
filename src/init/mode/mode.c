/*
 * init/mode/mode.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
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
#include <mode/mode.h>

/*
 * xboot running mode.
 */
static enum mode_t xboot_mode = MODE_NORMAL;

/*
 * get xboot's running mode.
 */
inline enum mode_t xboot_get_mode(void)
{
	return xboot_mode;
}

/*
 * set xboot's running mode.
 */
bool_t xboot_set_mode(enum mode_t m)
{
	switch(m)
	{
	case MODE_NORMAL:
		xboot_mode = MODE_NORMAL;
		break;

	case MODE_SHELL:
		xboot_mode = MODE_SHELL;
		break;

	case MODE_MENU:
		xboot_mode = MODE_MENU;
		break;

	case MODE_GRAPHIC:
		xboot_mode = MODE_GRAPHIC;
		break;

	case MODE_APPLICATION:
		xboot_mode = MODE_APPLICATION;
		break;

	default:
		return FALSE;
	}

	return TRUE;
}
