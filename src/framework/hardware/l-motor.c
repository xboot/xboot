/*
 * framework/hardware/l-motor.c
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

#include <motor/motor.h>
#include <framework/hardware/l-hardware.h>

static int l_motor_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct motor_t * m = search_motor(name);
	if(!m)
		return 0;
	lua_pushlightuserdata(L, m);
	luaL_setmetatable(L, MT_HARDWARE_MOTOR);
	return 1;
}

static int l_motor_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct motor_t * m;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_STEPPER], head)
	{
		m = (struct motor_t *)(pos->priv);
		if(!m)
			continue;
		lua_pushlightuserdata(L, m);
		luaL_setmetatable(L, MT_HARDWARE_MOTOR);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_motor[] = {
	{"new",		l_motor_new},
	{"list",	l_motor_list},
	{NULL,	NULL}
};

static int m_motor_tostring(lua_State * L)
{
	struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
	lua_pushstring(L, m->name);
	return 1;
}

static int m_motor_enable(lua_State * L)
{
	struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
	motor_enable(m);
	lua_settop(L, 1);
	return 1;
}

static int m_motor_disable(lua_State * L)
{
	struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
	motor_disable(m);
	lua_settop(L, 1);
	return 1;
}

static int m_motor_set_speed(lua_State * L)
{
	struct motor_t * m = luaL_checkudata(L, 1, MT_HARDWARE_MOTOR);
	int speed = luaL_optinteger(L, 2, 0);
	motor_set_speed(m, speed);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_motor[] = {
	{"__tostring",	m_motor_tostring},
	{"enable",		m_motor_enable},
	{"disable",		m_motor_disable},
	{"setSpeed",	m_motor_set_speed},
	{NULL,	NULL}
};

int luaopen_hardware_motor(lua_State * L)
{
	luaL_newlib(L, l_motor);
	luahelper_create_metatable(L, MT_HARDWARE_MOTOR, m_motor);
	return 1;
}
