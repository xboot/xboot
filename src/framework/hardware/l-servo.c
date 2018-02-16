/*
 * framework/hardware/l-servo.c
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

#include <servo/servo.h>
#include <framework/hardware/l-hardware.h>

static int l_servo_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct servo_t * servo = search_servo(name);
	if(!servo)
		return 0;
	lua_pushlightuserdata(L, servo);
	luaL_setmetatable(L, MT_HARDWARE_SERVO);
	return 1;
}

static int l_servo_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct servo_t * servo;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_SERVO], head)
	{
		servo = (struct servo_t *)(pos->priv);
		if(!servo)
			continue;
		lua_pushlightuserdata(L, servo);
		luaL_setmetatable(L, MT_HARDWARE_SERVO);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_servo[] = {
	{"new",		l_servo_new},
	{"list",	l_servo_list},
	{NULL,	NULL}
};

static int m_servo_tostring(lua_State * L)
{
	struct servo_t * servo = luaL_checkudata(L, 1, MT_HARDWARE_SERVO);
	lua_pushstring(L, servo->name);
	return 1;
}

static int m_servo_set_angle(lua_State * L)
{
	struct servo_t * servo = luaL_checkudata(L, 1, MT_HARDWARE_SERVO);
	int angle = luaL_checkinteger(L, 2);
	servo_set_angle(servo, angle);
	lua_settop(L, 1);
	return 1;
}

static int m_servo_get_angle(lua_State * L)
{
	struct servo_t * servo = luaL_checkudata(L, 1, MT_HARDWARE_SERVO);
	int angle = servo_get_angle(servo);
	lua_pushinteger(L, angle);
	return 1;
}

static const luaL_Reg m_servo[] = {
	{"__tostring",	m_servo_tostring},
	{"setAngle",	m_servo_set_angle},
	{"getAngle",	m_servo_get_angle},
	{NULL,	NULL}
};

int luaopen_hardware_servo(lua_State * L)
{
	luaL_newlib(L, l_servo);
	luahelper_create_metatable(L, MT_HARDWARE_SERVO, m_servo);
	return 1;
}
