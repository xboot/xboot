/*
 * framework/hardware/l-light.c
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

#include <light/light.h>
#include <framework/hardware/l-hardware.h>

static int l_light_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct light_t * light = name ? search_light(name) : search_first_light();
	if(!light)
		return 0;
	lua_pushlightuserdata(L, light);
	luaL_setmetatable(L, MT_HARDWARE_LIGHT);
	return 1;
}

static int l_light_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct light_t * light;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_LIGHT], head)
	{
		light = (struct light_t *)(pos->priv);
		if(!light)
			continue;
		lua_pushlightuserdata(L, light);
		luaL_setmetatable(L, MT_HARDWARE_LIGHT);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_light[] = {
	{"new",		l_light_new},
	{"list",	l_light_list},
	{NULL, NULL}
};

static int m_light_tostring(lua_State * L)
{
	struct light_t * light = luaL_checkudata(L, 1, MT_HARDWARE_LIGHT);
	lua_pushstring(L, light->name);
	return 1;
}

static int m_light_get_illuminance(lua_State * L)
{
	struct light_t * light = luaL_checkudata(L, 1, MT_HARDWARE_LIGHT);
	int lux = light_get_illuminance(light);
	lua_pushinteger(L, lux);
	return 1;
}

static const luaL_Reg m_light[] = {
	{"__tostring",		m_light_tostring},
	{"getIlluminance",	m_light_get_illuminance},
	{NULL, NULL}
};

int luaopen_hardware_light(lua_State * L)
{
	luaL_newlib(L, l_light);
	luahelper_create_metatable(L, MT_HARDWARE_LIGHT, m_light);
	return 1;
}
