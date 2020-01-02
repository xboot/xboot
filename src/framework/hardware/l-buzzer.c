/*
 * framework/hardware/l-buzzer.c
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
	struct device_t * pos, * n;
	struct buzzer_t * buzzer;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BUZZER], head)
	{
		buzzer = (struct buzzer_t *)(pos->priv);
		if(!buzzer)
			continue;
		lua_pushlightuserdata(L, buzzer);
		luaL_setmetatable(L, MT_HARDWARE_BUZZER);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_buzzer[] = {
	{"new",		l_buzzer_new},
	{"list",	l_buzzer_list},
	{NULL,	NULL}
};

static int m_buzzer_tostring(lua_State * L)
{
	struct buzzer_t * buzzer = luaL_checkudata(L, 1, MT_HARDWARE_BUZZER);
	lua_pushstring(L, buzzer->name);
	return 1;
}

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
	const char * rtttl = luaL_optstring(L, 2, NULL);
	buzzer_play(buzzer, rtttl);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_buzzer[] = {
	{"__tostring",		m_buzzer_tostring},
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
