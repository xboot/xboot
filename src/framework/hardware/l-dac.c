/*
 * framework/hardware/l-dac.c
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
	struct device_t * pos, * n;
	struct dac_t * dac;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_DAC], head)
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

static int m_dac_tostring(lua_State * L)
{
	struct dac_t * dac = luaL_checkudata(L, 1, MT_HARDWARE_DAC);
	lua_pushstring(L, dac->name);
	return 1;
}

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
	{"__tostring",	m_dac_tostring},
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
