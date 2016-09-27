/*
 * framework/hardware/l-dac.c
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

#include <dac/dac.h>
#include <framework/hardware/l-hardware.h>

static int l_dac_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct dac_t * dac = search_dac(name);
	if(!dac)
		return 0;
	lua_pushlightuserdata(L, dac);
	luaL_setmetatable(L, MT_HARDWARE_DAC);
	return 1;
}

static int l_dac_list(lua_State * L)
{
	struct device_t * pos;
	struct hlist_node * n;
	struct dac_t * dac;

	lua_newtable(L);
	hlist_for_each_entry_safe(pos, n, &__device_hash[DEVICE_TYPE_DAC], node)
	{
		dac = (struct dac_t *)(pos->priv);
		if(!dac)
			continue;
		lua_pushlightuserdata(L, dac);
		luaL_setmetatable(L, MT_HARDWARE_DAC);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_dac[] = {
	{"new",		l_dac_new},
	{"list",	l_dac_list},
	{NULL,	NULL}
};

static int m_dac_channels(lua_State * L)
{
	struct dac_t * dac = luaL_checkudata(L, 1, MT_HARDWARE_DAC);
	lua_pushinteger(L, dac->nchannel);
	return 1;
}

static int m_dac_write_raw(lua_State * L)
{
	struct dac_t * dac = luaL_checkudata(L, 1, MT_HARDWARE_DAC);
	int channel = luaL_checkinteger(L, 2);
	u32_t raw = luaL_checkinteger(L, 3);
	dac_write_raw(dac, channel, raw);
	lua_settop(L, 1);
	return 1;
}

static int m_dac_write_voltage(lua_State * L)
{
	struct dac_t * dac = luaL_checkudata(L, 1, MT_HARDWARE_DAC);
	int channel = luaL_checkinteger(L, 2);
	int voltage = luaL_checknumber(L, 3) * (lua_Number)(1000000);
	dac_write_voltage(dac, channel, voltage);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_dac[] = {
	{"channels",	m_dac_channels},
	{"writeRaw",	m_dac_write_raw},
	{"writeVoltage",m_dac_write_voltage},
	{NULL,	NULL}
};

int luaopen_hardware_dac(lua_State * L)
{
	luaL_newlib(L, l_dac);
	luahelper_create_metatable(L, MT_HARDWARE_DAC, m_dac);
	return 1;
}
