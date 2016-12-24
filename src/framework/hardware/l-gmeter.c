/*
 * framework/hardware/l-gmeter.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <gmeter/gmeter.h>
#include <framework/hardware/l-hardware.h>

static int l_gmeter_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct gmeter_t * gmeter = name ? search_gmeter(name) : search_first_gmeter();
	if(!gmeter)
		return 0;
	lua_pushlightuserdata(L, gmeter);
	luaL_setmetatable(L, MT_HARDWARE_GMETER);
	return 1;
}

static int l_gmeter_list(lua_State * L)
{
	struct device_t * pos;
	struct hlist_node * n;
	struct gmeter_t * gmeter;

	lua_newtable(L);
	hlist_for_each_entry_safe(pos, n, &__device_hash[DEVICE_TYPE_GMETER], node)
	{
		gmeter = (struct gmeter_t *)(pos->priv);
		if(!gmeter)
			continue;
		lua_pushlightuserdata(L, gmeter);
		luaL_setmetatable(L, MT_HARDWARE_GMETER);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_gmeter[] = {
	{"new",		l_gmeter_new},
	{"list",	l_gmeter_list},
	{NULL, NULL}
};

static int m_gmeter_get_acceleration(lua_State * L)
{
	struct gmeter_t * gmeter = luaL_checkudata(L, 1, MT_HARDWARE_GMETER);
	int x, y, z;
	if(!gmeter_get_acceleration(gmeter, &x, &y, &z))
	{
		lua_pushnil(L);
		return 1;
	}
	lua_pushnumber(L, (lua_Number)x);
	lua_pushnumber(L, (lua_Number)y);
	lua_pushnumber(L, (lua_Number)z);
	return 3;
}

static const luaL_Reg m_gmeter[] = {
	{"getAcceleration",	m_gmeter_get_acceleration},
	{NULL, NULL}
};

int luaopen_hardware_gmeter(lua_State * L)
{
	luaL_newlib(L, l_gmeter);
	luahelper_create_metatable(L, MT_HARDWARE_GMETER, m_gmeter);
	return 1;
}
