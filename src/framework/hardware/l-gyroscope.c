/*
 * framework/hardware/l-gyroscope.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <gyroscope/gyroscope.h>
#include <framework/hardware/l-hardware.h>

static int l_gyroscope_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct gyroscope_t * g = name ? search_gyroscope(name) : search_first_gyroscope();
	if(!g)
		return 0;
	lua_pushlightuserdata(L, g);
	luaL_setmetatable(L, MT_HARDWARE_GYROSCOPE);
	return 1;
}

static int l_gyroscope_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct gyroscope_t * g;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_GMETER], head)
	{
		g = (struct gyroscope_t *)(pos->priv);
		if(!g)
			continue;
		lua_pushlightuserdata(L, g);
		luaL_setmetatable(L, MT_HARDWARE_GYROSCOPE);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_gyroscope[] = {
	{"new",		l_gyroscope_new},
	{"list",	l_gyroscope_list},
	{NULL, NULL}
};

static int m_gyroscope_tostring(lua_State * L)
{
	struct gyroscope_t * g = luaL_checkudata(L, 1, MT_HARDWARE_GYROSCOPE);
	lua_pushstring(L, g->name);
	return 1;
}

static int m_gyroscope_get_palstance(lua_State * L)
{
	struct gyroscope_t * g = luaL_checkudata(L, 1, MT_HARDWARE_GYROSCOPE);
	int x, y, z;
	if(gyroscope_get_palstance(g, &x, &y, &z))
	{
		lua_pushnumber(L, (lua_Number)x / 1000000);
		lua_pushnumber(L, (lua_Number)y / 1000000);
		lua_pushnumber(L, (lua_Number)z / 1000000);
		return 3;
	}
	return 0;
}

static const luaL_Reg m_gyroscope[] = {
	{"__tostring",		m_gyroscope_tostring},
	{"getPalstance",	m_gyroscope_get_palstance},
	{NULL, NULL}
};

int luaopen_hardware_gyroscope(lua_State * L)
{
	luaL_newlib(L, l_gyroscope);
	luahelper_create_metatable(L, MT_HARDWARE_GYROSCOPE, m_gyroscope);
	return 1;
}
