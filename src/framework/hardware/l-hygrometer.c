/*
 * framework/hardware/l-hygrometer.c
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

#include <hygrometer/hygrometer.h>
#include <framework/hardware/l-hardware.h>

static int l_hygrometer_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct hygrometer_t * hygrometer = name ? search_hygrometer(name) : search_first_hygrometer();
	if(!hygrometer)
		return 0;
	lua_pushlightuserdata(L, hygrometer);
	luaL_setmetatable(L, MT_HARDWARE_HYGROMETER);
	return 1;
}

static int l_hygrometer_list(lua_State * L)
{
	struct device_t * pos;
	struct hlist_node * n;
	struct hygrometer_t * hygrometer;

	lua_newtable(L);
	hlist_for_each_entry_safe(pos, n, &__device_hash[DEVICE_TYPE_HYGROMETER], node)
	{
		hygrometer = (struct hygrometer_t *)(pos->priv);
		if(!hygrometer)
			continue;
		lua_pushlightuserdata(L, hygrometer);
		luaL_setmetatable(L, MT_HARDWARE_HYGROMETER);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_hygrometer[] = {
	{"new",		l_hygrometer_new},
	{"list",	l_hygrometer_list},
	{NULL, NULL}
};

static int m_hygrometer_get_humidity(lua_State * L)
{
	struct hygrometer_t * hygrometer = luaL_checkudata(L, 1, MT_HARDWARE_HYGROMETER);
	int humidity = hygrometer_get_humidity(hygrometer);
	lua_pushnumber(L, (lua_Number)humidity / 100);
	return 1;
}

static const luaL_Reg m_hygrometer[] = {
	{"getHumidity",	m_hygrometer_get_humidity},
	{NULL, NULL}
};

int luaopen_hardware_hygrometer(lua_State * L)
{
	luaL_newlib(L, l_hygrometer);
	luahelper_create_metatable(L, MT_HARDWARE_HYGROMETER, m_hygrometer);
	return 1;
}
