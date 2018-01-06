/*
 * framework/hardware/l-led_trigger.c
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

#include <led/ledtrig.h>
#include <framework/hardware/l-hardware.h>

static int l_ledtrig_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct ledtrig_t * trigger = search_ledtrig(name);
	if(!trigger)
		return 0;
	lua_pushlightuserdata(L, trigger);
	luaL_setmetatable(L, MT_HARDWARE_LEDTRIG);
	return 1;
}

static int l_ledtrig_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct ledtrig_t * trigger;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_LEDTRIG], head)
	{
		trigger = (struct ledtrig_t *)(pos->priv);
		if(!trigger)
			continue;
		lua_pushlightuserdata(L, trigger);
		luaL_setmetatable(L, MT_HARDWARE_LEDTRIG);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_ledtrig[] = {
	{"new",		l_ledtrig_new},
	{"list",	l_ledtrig_list},
	{NULL, NULL}
};

static int m_ledtrig_tostring(lua_State * L)
{
	struct ledtrig_t * trigger = luaL_checkudata(L, 1, MT_HARDWARE_LEDTRIG);
	lua_pushstring(L, trigger->name);
	return 1;
}

static int m_ledtrig_activity(lua_State * L)
{
	struct ledtrig_t * trigger = luaL_checkudata(L, 1, MT_HARDWARE_LEDTRIG);
	ledtrig_activity(trigger);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_ledtrig[] = {
	{"__tostring",	m_ledtrig_tostring},
	{"activity",	m_ledtrig_activity},
	{NULL,	NULL}
};

int luaopen_hardware_ledtrig(lua_State * L)
{
	luaL_newlib(L, l_ledtrig);
	luahelper_create_metatable(L, MT_HARDWARE_LEDTRIG, m_ledtrig);
	return 1;
}
