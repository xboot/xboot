/*
 * framework/hardware/l-pwm.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <pwm/pwm.h>
#include <framework/hardware/l-hardware.h>

static int l_pwm_new(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	struct pwm_t * pwm = search_pwm(name);
	if(!pwm)
		return 0;
	lua_pushlightuserdata(L, pwm);
	luaL_setmetatable(L, MT_NAME_HARDWARE_PWM);
	return 1;
}

static int l_pwm_list(lua_State * L)
{
	struct pwm_list_t * pos, * n;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &(__pwm_list.entry), entry)
	{
		lua_pushlightuserdata(L, pos->pwm);
		luaL_setmetatable(L, MT_NAME_HARDWARE_PWM);
		lua_setfield(L, -2, pos->pwm->name);
	}
	return 1;
}

static const luaL_Reg l_hardware_pwm[] = {
	{"new",		l_pwm_new},
	{"list",	l_pwm_list},
	{NULL, NULL}
};

static int m_pwm_enable(lua_State * L)
{
	struct pwm_t * pwm = luaL_checkudata(L, 1, MT_NAME_HARDWARE_PWM);
	pwm_enable(pwm);
	lua_pushlightuserdata(L, pwm);
	luaL_setmetatable(L, MT_NAME_HARDWARE_PWM);
	return 1;
}

static int m_pwm_disable(lua_State * L)
{
	struct pwm_t * pwm = luaL_checkudata(L, 1, MT_NAME_HARDWARE_PWM);
	pwm_disable(pwm);
	lua_pushlightuserdata(L, pwm);
	luaL_setmetatable(L, MT_NAME_HARDWARE_PWM);
	return 1;
}

static int m_pwm_config(lua_State * L)
{
	struct pwm_t * pwm = luaL_checkudata(L, 1, MT_NAME_HARDWARE_PWM);
	u32_t duty = luaL_optinteger(L, 2, pwm->__duty);
	u32_t period = luaL_optinteger(L, 3, pwm->__period);
	pwm_config(pwm, duty, period);
	lua_pushlightuserdata(L, pwm);
	luaL_setmetatable(L, MT_NAME_HARDWARE_PWM);
	return 1;
}

static const luaL_Reg m_hardware_pwm[] = {
	{"enable",		m_pwm_enable},
	{"disable",		m_pwm_disable},
	{"config",		m_pwm_config},
	{NULL, NULL}
};

int luaopen_hardware_pwm(lua_State * L)
{
	luaL_newlib(L, l_hardware_pwm);
	luahelper_create_metatable(L, MT_NAME_HARDWARE_PWM, m_hardware_pwm);
	return 1;
}
