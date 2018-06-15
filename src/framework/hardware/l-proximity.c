/*
 * framework/hardware/l-proximity.c
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

#include <proximity/proximity.h>
#include <framework/hardware/l-hardware.h>

static int l_proximity_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct proximity_t * p = name ? search_proximity(name) : search_first_proximity();
	if(!p)
		return 0;
	lua_pushlightuserdata(L, p);
	luaL_setmetatable(L, MT_HARDWARE_PROXIMITY);
	return 1;
}

static int l_proximity_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct proximity_t * p;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_PRESSURE], head)
	{
		p = (struct proximity_t *)(pos->priv);
		if(!p)
			continue;
		lua_pushlightuserdata(L, p);
		luaL_setmetatable(L, MT_HARDWARE_PROXIMITY);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_proximity[] = {
	{"new",		l_proximity_new},
	{"list",	l_proximity_list},
	{NULL, NULL}
};

static int m_proximity_tostring(lua_State * L)
{
	struct proximity_t * p = luaL_checkudata(L, 1, MT_HARDWARE_PROXIMITY);
	lua_pushstring(L, p->name);
	return 1;
}

static int m_proximity_get_distance(lua_State * L)
{
	struct proximity_t * p = luaL_checkudata(L, 1, MT_HARDWARE_PROXIMITY);
	lua_pushnumber(L, (lua_Number)(proximity_get_distance(p)));
	return 1;
}

static const luaL_Reg m_proximity[] = {
	{"__tostring",	m_proximity_tostring},
	{"getDistance",	m_proximity_get_distance},
	{NULL, NULL}
};

int luaopen_hardware_proximity(lua_State * L)
{
	luaL_newlib(L, l_proximity);
	luahelper_create_metatable(L, MT_HARDWARE_PROXIMITY, m_proximity);
	return 1;
}
