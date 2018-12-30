/*
 * framework/hardware/l-led_trigger.c
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

#include <led/ledtrigger.h>
#include <framework/hardware/l-hardware.h>

static int l_ledtrigger_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct ledtrigger_t * trigger = search_ledtrigger(name);
	if(!trigger)
		return 0;
	lua_pushlightuserdata(L, trigger);
	luaL_setmetatable(L, MT_HARDWARE_LEDTRIGGER);
	return 1;
}

static int l_ledtrigger_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct ledtrigger_t * trigger;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_LEDTRIGGER], head)
	{
		trigger = (struct ledtrigger_t *)(pos->priv);
		if(!trigger)
			continue;
		lua_pushlightuserdata(L, trigger);
		luaL_setmetatable(L, MT_HARDWARE_LEDTRIGGER);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_ledtrigger[] = {
	{"new",		l_ledtrigger_new},
	{"list",	l_ledtrigger_list},
	{NULL, NULL}
};

static int m_ledtrigger_tostring(lua_State * L)
{
	struct ledtrigger_t * trigger = luaL_checkudata(L, 1, MT_HARDWARE_LEDTRIGGER);
	lua_pushstring(L, trigger->name);
	return 1;
}

static int m_ledtrigger_activity(lua_State * L)
{
	struct ledtrigger_t * trigger = luaL_checkudata(L, 1, MT_HARDWARE_LEDTRIGGER);
	ledtrigger_activity(trigger);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_ledtrigger[] = {
	{"__tostring",	m_ledtrigger_tostring},
	{"activity",	m_ledtrigger_activity},
	{NULL,	NULL}
};

int luaopen_hardware_ledtrigger(lua_State * L)
{
	luaL_newlib(L, l_ledtrigger);
	luahelper_create_metatable(L, MT_HARDWARE_LEDTRIGGER, m_ledtrigger);
	return 1;
}
