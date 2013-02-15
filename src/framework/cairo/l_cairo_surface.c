/*
 * framework/cairo/l_cairo_surface.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
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

#include <framework/cairo/l_cairo.h>

#if 0
static int l_cairo_new(lua_State * L)
{
	return 0;
}

static const luaL_Reg l_cairo[] = {
	{"new",			l_cairo_new},
	{NULL, NULL}
};

static int m_cairo_tostring(lua_State * L)
{
	return 0;
}

static const luaL_Reg m_cairo[] = {
	{"__tostring",	m_cairo_tostring},
	{NULL, 			NULL}
};

int luaopen_cairo(lua_State * L)
{
	luaL_newlib(L, l_cairo);

	luaL_newmetatable(L, LUA_MT_NAME_CAIRO);
	luaL_setfuncs(L, m_cairo, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	return 1;
}
#endif

