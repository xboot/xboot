/*
 * framework/hardware/l-ledstrip.c
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

#include <led/ledstrip.h>
#include <framework/hardware/l-hardware.h>

static int l_ledstrip_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct ledstrip_t * strip = search_ledstrip(name);
	if(!strip)
		return 0;
	lua_pushlightuserdata(L, strip);
	luaL_setmetatable(L, MT_HARDWARE_LEDSTRIP);
	return 1;
}

static int l_ledstrip_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct ledstrip_t * strip;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_LEDSTRIP], head)
	{
		strip = (struct ledstrip_t *)(pos->priv);
		if(!strip)
			continue;
		lua_pushlightuserdata(L, strip);
		luaL_setmetatable(L, MT_HARDWARE_LEDSTRIP);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_ledstrip[] = {
	{"new",		l_ledstrip_new},
	{"list",	l_ledstrip_list},
	{NULL,	NULL}
};

static int m_ledstrip_tostring(lua_State * L)
{
	struct ledstrip_t * strip = luaL_checkudata(L, 1, MT_HARDWARE_LEDSTRIP);
	lua_pushstring(L, strip->name);
	return 1;
}

static int m_ledstrip_set_count(lua_State * L)
{
	struct ledstrip_t * strip = luaL_checkudata(L, 1, MT_HARDWARE_LEDSTRIP);
	int c = luaL_checkinteger(L, 2);
	ledstrip_set_count(strip, c);
	lua_settop(L, 1);
	return 1;
}

static int m_ledstrip_get_count(lua_State * L)
{
	struct ledstrip_t * strip = luaL_checkudata(L, 1, MT_HARDWARE_LEDSTRIP);
	lua_pushinteger(L, ledstrip_get_count(strip));
	return 1;
}

static int m_ledstrip_set_color(lua_State * L)
{
	struct ledstrip_t * strip = luaL_checkudata(L, 1, MT_HARDWARE_LEDSTRIP);
	int i = luaL_checkinteger(L, 2);
	double red = luaL_checknumber(L, 3);
	double green = luaL_checknumber(L, 4);
	double blue = luaL_checknumber(L, 5);
	uint8_t r = (uint8_t)(red * 0xff) & 0xff;
	uint8_t g = (uint8_t)(green * 0xff) & 0xff;
	uint8_t b = (uint8_t)(blue * 0xff) & 0xff;
	uint32_t color = (r << 16) | (g << 8) | (b << 0);
	ledstrip_set_color(strip, i, color);
	lua_settop(L, 1);
	return 1;
}

static int m_ledstrip_get_color(lua_State * L)
{
	struct ledstrip_t * strip = luaL_checkudata(L, 1, MT_HARDWARE_LEDSTRIP);
	int i = luaL_checkinteger(L, 2);
	uint32_t color = ledstrip_get_color(strip, i);
	double red = (double)((color >> 16) & 0xff) / 0xff;
	double green = (double)((color >> 8) & 0xff) / 0xff;
	double blue = (double)((color >> 0) & 0xff) / 0xff;
	lua_pushnumber(L, red);
	lua_pushnumber(L, green);
	lua_pushnumber(L, blue);
	return 3;
}

static int m_ledstrip_refresh(lua_State * L)
{
	struct ledstrip_t * strip = luaL_checkudata(L, 1, MT_HARDWARE_LEDSTRIP);
	ledstrip_refresh(strip);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_ledstrip[] = {
	{"__tostring",	m_ledstrip_tostring},
	{"setCount",	m_ledstrip_set_count},
	{"getCount",	m_ledstrip_get_count},
	{"setColor",	m_ledstrip_set_color},
	{"getColor",	m_ledstrip_get_color},
	{"refresh",		m_ledstrip_refresh},
	{NULL,	NULL}
};

int luaopen_hardware_ledstrip(lua_State * L)
{
	luaL_newlib(L, l_ledstrip);
	luahelper_create_metatable(L, MT_HARDWARE_LEDSTRIP, m_ledstrip);
	return 1;
}
