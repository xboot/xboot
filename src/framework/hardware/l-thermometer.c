/*
 * framework/hardware/l-thermometer.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

static int m_thermometer_tostring(lua_State * L)
{
	struct thermometer_t * thermometer = luaL_checkudata(L, 1, MT_HARDWARE_THERMOMETER);
	lua_pushstring(L, thermometer->name);
	return 1;
}

static int m_thermometer_get_temperature(lua_State * L)
{
	struct thermometer_t * thermometer = luaL_checkudata(L, 1, MT_HARDWARE_THERMOMETER);
	int temp = thermometer_get_temperature(thermometer);
	lua_pushnumber(L, (lua_Number)temp / 1000);
	return 1;
}

static const luaL_Reg m_thermometer[] = {
	{"__tostring",		m_thermometer_tostring},
	{"getTemperature",	m_thermometer_get_temperature},
	{NULL, NULL}
};

int luaopen_hardware_thermometer(lua_State * L)
{
	luaL_newlib(L, l_thermometer);
	luahelper_create_metatable(L, MT_HARDWARE_THERMOMETER, m_thermometer);
	return 1;
}
