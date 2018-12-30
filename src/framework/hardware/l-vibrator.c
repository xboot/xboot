/*
 * framework/hardware/l-vibrator.c
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
	struct device_t * pos, * n;
	struct vibrator_t * vib;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_VIBRATOR], head)
	{
		vib = (struct vibrator_t *)(pos->priv);
		if(!vib)
			continue;
		lua_pushlightuserdata(L, vib);
		luaL_setmetatable(L, MT_HARDWARE_VIBRATOR);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_vibrator[] = {
	{"new",		l_vibrator_new},
	{"list",	l_vibrator_list},
	{NULL,	NULL}
};

static int m_vibrator_tostring(lua_State * L)
{
	struct vibrator_t * vib = luaL_checkudata(L, 1, MT_HARDWARE_VIBRATOR);
	lua_pushstring(L, vib->name);
	return 1;
}

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
	{"__tostring",	m_vibrator_tostring},
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
