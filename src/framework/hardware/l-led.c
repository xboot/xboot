/*
 * framework/hardware/l-led.c
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

#include <led/led.h>
#include <framework/hardware/l-hardware.h>

static int l_led_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct led_t * led = search_led(name);
	if(!led)
		return 0;
	lua_pushlightuserdata(L, led);
	luaL_setmetatable(L, MT_HARDWARE_LED);
	return 1;
}

static int l_led_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct led_t * led;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_LED], head)
	{
		led = (struct led_t *)(pos->priv);
		if(!led)
			continue;
		lua_pushlightuserdata(L, led);
		luaL_setmetatable(L, MT_HARDWARE_LED);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_led[] = {
	{"new",		l_led_new},
	{"list",	l_led_list},
	{NULL,	NULL}
};

static int m_led_tostring(lua_State * L)
{
	struct led_t * led = luaL_checkudata(L, 1, MT_HARDWARE_LED);
	lua_pushstring(L, led->name);
	return 1;
}

static int m_led_set_brightness(lua_State * L)
{
	struct led_t * led = luaL_checkudata(L, 1, MT_HARDWARE_LED);
	int brightness = luaL_checknumber(L, 2) * (lua_Number)(CONFIG_MAX_BRIGHTNESS);
	led_set_brightness(led, brightness);
	lua_settop(L, 1);
	return 1;
}

static int m_led_get_brightness(lua_State * L)
{
	struct led_t * led = luaL_checkudata(L, 1, MT_HARDWARE_LED);
	int brightness = led_get_brightness(led);
	lua_pushnumber(L, brightness / (lua_Number)(CONFIG_MAX_BRIGHTNESS));
	return 1;
}

static const luaL_Reg m_led[] = {
	{"__tostring",		m_led_tostring},
	{"setBrightness",	m_led_set_brightness},
	{"getBrightness",	m_led_get_brightness},
	{NULL,	NULL}
};

int luaopen_hardware_led(lua_State * L)
{
	luaL_newlib(L, l_led);
	luahelper_create_metatable(L, MT_HARDWARE_LED, m_led);
	return 1;
}
