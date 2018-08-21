/*
 * framework/hardware/l-servo.c
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

#include <servo/servo.h>
#include <framework/hardware/l-hardware.h>

static int l_servo_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct servo_t * m = search_servo(name);
	if(!m)
		return 0;
	lua_pushlightuserdata(L, m);
	luaL_setmetatable(L, MT_HARDWARE_SERVO);
	return 1;
}

static int l_servo_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct servo_t * m;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_SERVO], head)
	{
		m = (struct servo_t *)(pos->priv);
		if(!m)
			continue;
		lua_pushlightuserdata(L, m);
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
	struct servo_t * m = luaL_checkudata(L, 1, MT_HARDWARE_SERVO);
	lua_pushstring(L, m->name);
	return 1;
}

static int m_servo_enable(lua_State * L)
{
	struct servo_t * m = luaL_checkudata(L, 1, MT_HARDWARE_SERVO);
	servo_enable(m);
	lua_settop(L, 1);
	return 1;
}

static int m_servo_disable(lua_State * L)
{
	struct servo_t * m = luaL_checkudata(L, 1, MT_HARDWARE_SERVO);
	servo_disable(m);
	lua_settop(L, 1);
	return 1;
}

static int m_servo_set_angle(lua_State * L)
{
	struct servo_t * m = luaL_checkudata(L, 1, MT_HARDWARE_SERVO);
	int angle = luaL_checkinteger(L, 2);
	servo_set_angle(m, angle);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_servo[] = {
	{"__tostring",	m_servo_tostring},
	{"enable",		m_servo_enable},
	{"disable",		m_servo_disable},
	{"setAngle",	m_servo_set_angle},
	{NULL,	NULL}
};

int luaopen_hardware_servo(lua_State * L)
{
	luaL_newlib(L, l_servo);
	luahelper_create_metatable(L, MT_HARDWARE_SERVO, m_servo);
	return 1;
}
