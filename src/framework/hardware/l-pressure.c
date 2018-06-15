/*
 * framework/hardware/l-pressure.c
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

#include <pressure/pressure.h>
#include <framework/hardware/l-hardware.h>

static int l_pressure_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct pressure_t * p = name ? search_pressure(name) : search_first_pressure();
	if(!p)
		return 0;
	lua_pushlightuserdata(L, p);
	luaL_setmetatable(L, MT_HARDWARE_PRESSURE);
	return 1;
}

static int l_pressure_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct pressure_t * p;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_PRESSURE], head)
	{
		p = (struct pressure_t *)(pos->priv);
		if(!p)
			continue;
		lua_pushlightuserdata(L, p);
		luaL_setmetatable(L, MT_HARDWARE_PRESSURE);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_pressure[] = {
	{"new",		l_pressure_new},
	{"list",	l_pressure_list},
	{NULL, NULL}
};

static int m_pressure_tostring(lua_State * L)
{
	struct pressure_t * p = luaL_checkudata(L, 1, MT_HARDWARE_PRESSURE);
	lua_pushstring(L, p->name);
	return 1;
}

static int m_pressure_get_pascal(lua_State * L)
{
	struct pressure_t * p = luaL_checkudata(L, 1, MT_HARDWARE_PRESSURE);
	lua_pushnumber(L, (lua_Number)(pressure_get_pascal(p)));
	return 1;
}

static const luaL_Reg m_pressure[] = {
	{"__tostring",	m_pressure_tostring},
	{"getPascal",	m_pressure_get_pascal},
	{NULL, NULL}
};

int luaopen_hardware_pressure(lua_State * L)
{
	luaL_newlib(L, l_pressure);
	luahelper_create_metatable(L, MT_HARDWARE_PRESSURE, m_pressure);
	return 1;
}
