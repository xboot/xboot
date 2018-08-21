/*
 * framework/hardware/l-watchdog.c
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

#include <watchdog/watchdog.h>
#include <framework/hardware/l-hardware.h>

static int l_watchdog_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct watchdog_t * wdg = name ? search_watchdog(name) : search_first_watchdog();
	if(!wdg)
		return 0;
	lua_pushlightuserdata(L, wdg);
	luaL_setmetatable(L, MT_HARDWARE_WATCHDOG);
	return 1;
}

static int l_watchdog_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct watchdog_t * wdg;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_WATCHDOG], head)
	{
		wdg = (struct watchdog_t *)(pos->priv);
		if(!wdg)
			continue;
		lua_pushlightuserdata(L, wdg);
		luaL_setmetatable(L, MT_HARDWARE_WATCHDOG);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_watchdog[] = {
	{"new",		l_watchdog_new},
	{"list",	l_watchdog_list},
	{NULL, NULL}
};

static int m_watchdog_tostring(lua_State * L)
{
	struct watchdog_t * wdg = luaL_checkudata(L, 1, MT_HARDWARE_WATCHDOG);
	lua_pushstring(L, wdg->name);
	return 1;
}

static int m_watchdog_set_timeout(lua_State * L)
{
	struct watchdog_t * wdg = luaL_checkudata(L, 1, MT_HARDWARE_WATCHDOG);
	int timeout = luaL_checkinteger(L, 2);
	watchdog_set_timeout(wdg, timeout);
	lua_settop(L, 1);
	return 1;
}

static int m_watchdog_get_timeout(lua_State * L)
{
	struct watchdog_t * wdg = luaL_checkudata(L, 1, MT_HARDWARE_WATCHDOG);
	int timeout = watchdog_get_timeout(wdg);
	lua_pushinteger(L, timeout);
	return 1;
}

static const luaL_Reg m_watchdog[] = {
	{"__tostring",	m_watchdog_tostring},
	{"setTimeout",	m_watchdog_set_timeout},
	{"getTimeout",	m_watchdog_get_timeout},
	{NULL, NULL}
};

int luaopen_hardware_watchdog(lua_State * L)
{
	luaL_newlib(L, l_watchdog);
	luahelper_create_metatable(L, MT_HARDWARE_WATCHDOG, m_watchdog);
	return 1;
}
