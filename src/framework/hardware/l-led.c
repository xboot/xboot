/*
 * framework/hardware/l-led.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <led/led.h>
#include <framework/hardware/l-hardware.h>

static int l_led_list(lua_State * L)
{
	struct device_list_t * pos, * n;
	struct led_t * led;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_LED)
		{
			led = (struct led_t *)(pos->device->driver);
			if(!led)
				continue;

			lua_pushlightuserdata(L, led);
			luaL_setmetatable(L, MT_NAME_HARDWARE_LED);
			lua_setfield(L, -2, pos->device->name);
		}
	}
	return 1;
}

static int l_led_search(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct led_t * led = search_led(name);
	if(!led)
		return 0;
	lua_pushlightuserdata(L, led);
	luaL_setmetatable(L, MT_NAME_HARDWARE_LED);
	return 1;
}

static const luaL_Reg l_hardware_led[] = {
	{"list", l_led_list},
	{"search", l_led_search},
	{NULL, NULL}
};

static int m_led_get(lua_State * L)
{
	struct led_t * led = luaL_checkudata(L, 1, MT_NAME_HARDWARE_LED);
	u32_t color = led_get_color(led);
	lua_pushunsigned(L, color);
	return 1;
}

static int m_led_set(lua_State * L)
{
	struct led_t * led = luaL_checkudata(L, 1, MT_NAME_HARDWARE_LED);
	u32_t color = luaL_checkunsigned(L, 2);
	led_set_color(led, color);
	return 0;
}

static const luaL_Reg m_hardware_led[] = {
	{"get", m_led_get},
	{"set", m_led_set},
	{NULL,	NULL}
};

int luaopen_hardware_led(lua_State * L)
{
	luaL_newlib(L, l_hardware_led);
	luahelper_create_metatable(L, MT_NAME_HARDWARE_LED, m_hardware_led);
	return 1;
}
