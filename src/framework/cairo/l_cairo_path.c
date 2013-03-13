/*
 * framework/cairo/l_cairo_path.c
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

#include <cairo.h>
#include <framework/cairo/l_cairo.h>

static int m_cairo_path_gc(lua_State * L)
{
	cairo_path_t ** path = luaL_checkudata(L, 1, MT_NAME_CAIRO_PATH);
	cairo_path_destroy(*path);
	return 0;
}

const luaL_Reg m_cairo_path[] = {
	{"__gc",				m_cairo_path_gc},
	{NULL, 					NULL}
};
