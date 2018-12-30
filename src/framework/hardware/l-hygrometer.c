/*
 * framework/hardware/l-hygrometer.c
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
	struct device_t * pos, * n;
	struct hygrometer_t * hygrometer;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_HYGROMETER], head)
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

static int m_hygrometer_tostring(lua_State * L)
{
	struct hygrometer_t * hygrometer = luaL_checkudata(L, 1, MT_HARDWARE_HYGROMETER);
	lua_pushstring(L, hygrometer->name);
	return 1;
}

static int m_hygrometer_get_humidity(lua_State * L)
{
	struct hygrometer_t * hygrometer = luaL_checkudata(L, 1, MT_HARDWARE_HYGROMETER);
	int humidity = hygrometer_get_humidity(hygrometer);
	lua_pushnumber(L, (lua_Number)humidity / 100);
	return 1;
}

static const luaL_Reg m_hygrometer[] = {
	{"__tostring",	m_hygrometer_tostring},
	{"getHumidity",	m_hygrometer_get_humidity},
	{NULL, NULL}
};

int luaopen_hardware_hygrometer(lua_State * L)
{
	luaL_newlib(L, l_hygrometer);
	luahelper_create_metatable(L, MT_HARDWARE_HYGROMETER, m_hygrometer);
	return 1;
}
