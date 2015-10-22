/*
 * framework/hardware/l-buzzer.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <buzzer/buzzer.h>
#include <framework/hardware/l-hardware.h>

static int l_buzzer_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct buzzer_t * buzzer = name ? search_buzzer(name) : search_first_buzzer();
	if(!buzzer)
		return 0;
	lua_pushlightuserdata(L, buzzer);
	luaL_setmetatable(L, MT_HARDWARE_BUZZER);
	return 1;
}

static int l_buzzer_list(lua_State * L)
{
	struct device_list_t * pos, * n;
	struct buzzer_t * buzzer;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_BUZZER)
		{
			buzzer = (struct buzzer_t *)(pos->device->driver);
			if(!buzzer)
				continue;
			lua_pushlightuserdata(L, buzzer);
			luaL_setmetatable(L, MT_HARDWARE_BUZZER);
			lua_setfield(L, -2, pos->device->name);
		}
	}
	return 1;
}

static const luaL_Reg l_buzzer[] = {
	{"new",		l_buzzer_new},
	{"list",	l_buzzer_list},
	{NULL,	NULL}
};

static int m_buzzer_set_frequency(lua_State * L)
{
	struct buzzer_t * buzzer = luaL_checkudata(L, 1, MT_HARDWARE_BUZZER);
	int frequency = luaL_checkinteger(L, 2);
	buzzer_set_frequency(buzzer, frequency);
	lua_settop(L, 1);
	return 1;
}

static int m_buzzer_get_frequency(lua_State * L)
{
	struct buzzer_t * buzzer = luaL_checkudata(L, 1, MT_HARDWARE_BUZZER);
	int frequency = buzzer_get_frequency(buzzer);
	lua_pushinteger(L, frequency);
	return 1;
}

static int m_buzzer_beep(lua_State * L)
{
	struct buzzer_t * buzzer = luaL_checkudata(L, 1, MT_HARDWARE_BUZZER);
	int frequency = luaL_checkinteger(L, 2);
	int millisecond = luaL_checkinteger(L, 3);
	buzzer_beep(buzzer, frequency, millisecond);
	lua_settop(L, 1);
	return 1;
}

static int m_buzzer_play(lua_State * L)
{
	struct buzzer_t * buzzer = luaL_checkudata(L, 1, MT_HARDWARE_BUZZER);
	const char * rtttl = luaL_optstring(L, 1, NULL);
	buzzer_play(buzzer, rtttl);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_buzzer[] = {
	{"setFrequency",	m_buzzer_set_frequency},
	{"getFrequency",	m_buzzer_get_frequency},
	{"beep",			m_buzzer_beep},
	{"play",			m_buzzer_play},
	{NULL,	NULL}
};

int luaopen_hardware_buzzer(lua_State * L)
{
	luaL_newlib(L, l_buzzer);
	luahelper_create_metatable(L, MT_HARDWARE_BUZZER, m_buzzer);
	return 1;
}
