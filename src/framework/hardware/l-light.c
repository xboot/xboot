/*
 * framework/hardware/l-light.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

static int m_light_get_illuminance(lua_State * L)
{
	struct light_t * light = luaL_checkudata(L, 1, MT_HARDWARE_LIGHT);
	int lux = light_get_illuminance(light);
	lua_pushinteger(L, lux);
	return 1;
}

static const luaL_Reg m_light[] = {
	{"getIlluminance",	m_light_get_illuminance},
	{NULL, NULL}
};

int luaopen_hardware_light(lua_State * L)
{
	luaL_newlib(L, l_light);
	luahelper_create_metatable(L, MT_HARDWARE_LIGHT, m_light);
	return 1;
}
