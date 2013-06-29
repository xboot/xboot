/*
 * drivers/fb/cursor.c
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

#include <fb/cursor.h>

static s32_t __cursor_xpos = 0;
static s32_t __cursor_ypos = 0;
static s32_t __display_left = 0;
static s32_t __display_top = 0;
static s32_t __display_right = 800 - 1;
static s32_t __display_bottom = 480 - 1;

s32_t cursor_xpos_with_offset(s32_t relx)
{
	__cursor_xpos = __cursor_xpos + relx;

	if (__cursor_xpos < __display_left)
		__cursor_xpos = __display_left;
	if (__cursor_xpos > __display_right)
		__cursor_xpos = __display_right;

	return __cursor_xpos;
}

s32_t cursor_ypos_with_offset(s32_t rely)
{
	__cursor_ypos = __cursor_ypos + rely;

	if (__cursor_ypos < __display_top)
		__cursor_ypos = __display_top;
	if (__cursor_ypos > __display_bottom)
		__cursor_ypos = __display_bottom;

	return __cursor_ypos;
}

s32_t get_cursor_xpos(void)
{
	return cursor_xpos_with_offset(0);
}

s32_t get_cursor_ypos(void)
{
	return cursor_ypos_with_offset(0);
}
