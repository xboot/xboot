/*
 * framework/hardware/l-battery.c
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

#include <battery/battery.h>
#include <framework/hardware/l-hardware.h>

static int l_battery_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct battery_t * bat = name ? search_battery(name) : search_first_battery();
	if(!bat)
		return 0;
	lua_pushlightuserdata(L, bat);
	luaL_setmetatable(L, MT_HARDWARE_BATTERY);
	return 1;
}

static int l_battery_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct battery_t * bat;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BATTERY], head)
	{
		bat = (struct battery_t *)(pos->priv);
		if(!bat)
			continue;
		lua_pushlightuserdata(L, bat);
		luaL_setmetatable(L, MT_HARDWARE_BATTERY);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_battery[] = {
	{"new",		l_battery_new},
	{"list",	l_battery_list},
	{NULL,	NULL}
};

static int m_battery_tostring(lua_State * L)
{
	struct battery_t * bat = luaL_checkudata(L, 1, MT_HARDWARE_BATTERY);
	lua_pushstring(L, bat->name);
	return 1;
}

static int m_battery_update(lua_State * L)
{
	struct battery_t * bat = luaL_checkudata(L, 1, MT_HARDWARE_BATTERY);
	struct battery_info_t info;

	lua_newtable(L);
	if(battery_update(bat, &info))
	{
		lua_pushstring(L, power_supply_string(info.supply));
		lua_setfield(L, -2, "supply");
		lua_pushstring(L, battery_status_string(info.status));
		lua_setfield(L, -2, "status");
		lua_pushstring(L, battery_health_string(info.health));
		lua_setfield(L, -2, "health");
		lua_pushinteger(L, info.design_capacity);
		lua_setfield(L, -2, "designCapacity");
		lua_pushinteger(L, info.design_voltage);
		lua_setfield(L, -2, "designVoltage");
		lua_pushinteger(L, info.voltage);
		lua_setfield(L, -2, "voltage");
		lua_pushinteger(L, info.current);
		lua_setfield(L, -2, "current");
		lua_pushnumber(L, (lua_Number)info.temperature / 1000);
		lua_setfield(L, -2, "temperature");
		lua_pushinteger(L, info.cycle);
		lua_setfield(L, -2, "cycle");
		lua_pushinteger(L, info.level);
		lua_setfield(L, -2, "level");
	}
	return 1;
}

static const luaL_Reg m_battery[] = {
	{"__tostring",	m_battery_tostring},
	{"update",		m_battery_update},
	{NULL,	NULL}
};

int luaopen_hardware_battery(lua_State * L)
{
	luaL_newlib(L, l_battery);
	luahelper_create_metatable(L, MT_HARDWARE_BATTERY, m_battery);
	return 1;
}
