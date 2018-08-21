/*
 * framework/hardware/l-gmeter.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <gmeter/gmeter.h>
#include <framework/hardware/l-hardware.h>

static int l_gmeter_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct gmeter_t * g = name ? search_gmeter(name) : search_first_gmeter();
	if(!g)
		return 0;
	lua_pushlightuserdata(L, g);
	luaL_setmetatable(L, MT_HARDWARE_GMETER);
	return 1;
}

static int l_gmeter_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct gmeter_t * g;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_GMETER], head)
	{
		g = (struct gmeter_t *)(pos->priv);
		if(!g)
			continue;
		lua_pushlightuserdata(L, g);
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

static int m_gmeter_tostring(lua_State * L)
{
	struct gmeter_t * g = luaL_checkudata(L, 1, MT_HARDWARE_GMETER);
	lua_pushstring(L, g->name);
	return 1;
}

static int m_gmeter_get_acceleration(lua_State * L)
{
	struct gmeter_t * g = luaL_checkudata(L, 1, MT_HARDWARE_GMETER);
	int x, y, z;
	if(gmeter_get_acceleration(g, &x, &y, &z))
	{
		lua_pushnumber(L, (lua_Number)x / 1000000);
		lua_pushnumber(L, (lua_Number)y / 1000000);
		lua_pushnumber(L, (lua_Number)z / 1000000);
		return 3;
	}
	return 0;
}

static const luaL_Reg m_gmeter[] = {
	{"__tostring",		m_gmeter_tostring},
	{"getAcceleration",	m_gmeter_get_acceleration},
	{NULL, NULL}
};

int luaopen_hardware_gmeter(lua_State * L)
{
	luaL_newlib(L, l_gmeter);
	luahelper_create_metatable(L, MT_HARDWARE_GMETER, m_gmeter);
	return 1;
}
