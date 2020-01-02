/*
 * framework/hardware/l-pressure.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
