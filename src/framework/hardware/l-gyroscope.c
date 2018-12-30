/*
 * framework/hardware/l-gyroscope.c
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
