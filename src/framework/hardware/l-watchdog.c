/*
 * framework/hardware/l-watchdog.c
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
