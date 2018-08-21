/*
 * framework/hardware/l-adc.c
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

#include <adc/adc.h>
#include <framework/hardware/l-hardware.h>

static int l_adc_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct adc_t * adc = search_adc(name);
	if(!adc)
		return 0;
	lua_pushlightuserdata(L, adc);
	luaL_setmetatable(L, MT_HARDWARE_ADC);
	return 1;
}

static int l_adc_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct adc_t * adc;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_ADC], head)
	{
		adc = (struct adc_t *)(pos->priv);
		if(!adc)
			continue;
		lua_pushlightuserdata(L, adc);
		luaL_setmetatable(L, MT_HARDWARE_ADC);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_adc[] = {
	{"new",		l_adc_new},
	{"list",	l_adc_list},
	{NULL,	NULL}
};

static int m_adc_tostring(lua_State * L)
{
	struct adc_t * adc = luaL_checkudata(L, 1, MT_HARDWARE_ADC);
	lua_pushstring(L, adc->name);
	return 1;
}

static int m_adc_channels(lua_State * L)
{
	struct adc_t * adc = luaL_checkudata(L, 1, MT_HARDWARE_ADC);
	lua_pushinteger(L, adc->nchannel);
	return 1;
}

static int m_adc_read_raw(lua_State * L)
{
	struct adc_t * adc = luaL_checkudata(L, 1, MT_HARDWARE_ADC);
	int channel = luaL_checkinteger(L, 2);
	u32_t raw = adc_read_raw(adc, channel);
	lua_pushinteger(L, raw);
	return 1;
}

static int m_adc_read_voltage(lua_State * L)
{
	struct adc_t * adc = luaL_checkudata(L, 1, MT_HARDWARE_ADC);
	int channel = luaL_checkinteger(L, 2);
	int voltage = adc_read_voltage(adc, channel);
	lua_pushnumber(L, voltage / (lua_Number)(1000000));
	return 1;
}

static const luaL_Reg m_adc[] = {
	{"__tostring",	m_adc_tostring},
	{"channels",	m_adc_channels},
	{"readRaw",		m_adc_read_raw},
	{"readVoltage",	m_adc_read_voltage},
	{NULL,	NULL}
};

int luaopen_hardware_adc(lua_State * L)
{
	luaL_newlib(L, l_adc);
	luahelper_create_metatable(L, MT_HARDWARE_ADC, m_adc);
	return 1;
}
