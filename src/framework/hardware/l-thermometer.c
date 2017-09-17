/*
 * framework/hardware/l-thermometer.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <thermometer/thermometer.h>
#include <framework/hardware/l-hardware.h>

static int l_thermometer_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct thermometer_t * thermometer = name ? search_thermometer(name) : search_first_thermometer();
	if(!thermometer)
		return 0;
	lua_pushlightuserdata(L, thermometer);
	luaL_setmetatable(L, MT_HARDWARE_THERMOMETER);
	return 1;
}

static int l_thermometer_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct thermometer_t * thermometer;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_THERMOMETER], head)
	{
		thermometer = (struct thermometer_t *)(pos->priv);
		if(!thermometer)
			continue;
		lua_pushlightuserdata(L, thermometer);
		luaL_setmetatable(L, MT_HARDWARE_THERMOMETER);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_thermometer[] = {
	{"new",		l_thermometer_new},
	{"list",	l_thermometer_list},
	{NULL, NULL}
};

static int m_thermometer_get_temperature(lua_State * L)
{
	struct thermometer_t * thermometer = luaL_checkudata(L, 1, MT_HARDWARE_THERMOMETER);
	int temp = thermometer_get_temperature(thermometer);
	lua_pushnumber(L, (lua_Number)temp / 1000);
	return 1;
}

static const luaL_Reg m_thermometer[] = {
	{"getTemperature",	m_thermometer_get_temperature},
	{NULL, NULL}
};

int luaopen_hardware_thermometer(lua_State * L)
{
	luaL_newlib(L, l_thermometer);
	luahelper_create_metatable(L, MT_HARDWARE_THERMOMETER, m_thermometer);
	return 1;
}
