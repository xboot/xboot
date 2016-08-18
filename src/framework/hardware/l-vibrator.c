/*
 * framework/hardware/l-vibrator.c
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

#include <vibrator/vibrator.h>
#include <framework/hardware/l-hardware.h>

static int l_vibrator_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct vibrator_t * vib = name ? search_vibrator(name) : search_first_vibrator();
	if(!vib)
		return 0;
	lua_pushlightuserdata(L, vib);
	luaL_setmetatable(L, MT_HARDWARE_VIBRATOR);
	return 1;
}

static int l_vibrator_list(lua_State * L)
{
	struct device_list_t * dl;
	struct hlist_node * pos, * n;
	struct vibrator_t * vib;

	lua_newtable(L);
	hlist_for_each_entry_safe(dl, pos, n, &__device_hash[DEVICE_TYPE_VIBRATOR], node)
	{
		vib = (struct vibrator_t *)(dl->device->priv);
		if(!vib)
			continue;
		lua_pushlightuserdata(L, vib);
		luaL_setmetatable(L, MT_HARDWARE_VIBRATOR);
		lua_setfield(L, -2, dl->device->name);
	}
	return 1;
}

static const luaL_Reg l_vibrator[] = {
	{"new",		l_vibrator_new},
	{"list",	l_vibrator_list},
	{NULL,	NULL}
};

static int m_vibrator_set_state(lua_State * L)
{
	struct vibrator_t * vib = luaL_checkudata(L, 1, MT_HARDWARE_VIBRATOR);
	int state = luaL_checkinteger(L, 2);
	vibrator_set_state(vib, state);
	lua_settop(L, 1);
	return 1;
}

static int m_vibrator_get_state(lua_State * L)
{
	struct vibrator_t * vib = luaL_checkudata(L, 1, MT_HARDWARE_VIBRATOR);
	int state = vibrator_get_state(vib);
	lua_pushinteger(L, state);
	return 1;
}

static int m_vibrator_vibrate(lua_State * L)
{
	struct vibrator_t * vib = luaL_checkudata(L, 1, MT_HARDWARE_VIBRATOR);
	int state = luaL_checkinteger(L, 2);
	int ms = luaL_checkinteger(L, 3);
	vibrator_vibrate(vib, state, ms);
	lua_settop(L, 1);
	return 1;
}

static int m_vibrator_play(lua_State * L)
{
	struct vibrator_t * vib = luaL_checkudata(L, 1, MT_HARDWARE_VIBRATOR);
	const char * morse = luaL_optstring(L, 1, NULL);
	vibrator_play(vib, morse);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_vibrator[] = {
	{"setState",	m_vibrator_set_state},
	{"getState",	m_vibrator_get_state},
	{"vibrate",		m_vibrator_vibrate},
	{"play",		m_vibrator_play},
	{NULL,	NULL}
};

int luaopen_hardware_vibrator(lua_State * L)
{
	luaL_newlib(L, l_vibrator);
	luahelper_create_metatable(L, MT_HARDWARE_VIBRATOR, m_vibrator);
	return 1;
}
