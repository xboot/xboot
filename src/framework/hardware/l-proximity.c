/*
 * framework/hardware/l-proximity.c
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
