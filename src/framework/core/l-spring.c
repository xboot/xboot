/*
 * framework/core/l-spring.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <core/l-spring.h>

static int l_new(lua_State * L)
{
	double start = luaL_optnumber(L, 1, 0);
	double stop = luaL_optnumber(L, 2, 1);
	double velocity = luaL_optnumber(L, 3, 0);
	double tension = luaL_optnumber(L, 4, 500);
	double friction = luaL_optnumber(L, 5, 60);
	struct spring_t * s = lua_newuserdata(L, sizeof(struct spring_t));
	spring_init(s, start, stop, velocity, tension, friction);
	luaL_setmetatable(L, MT_SPRING);
	return 1;
}

static const luaL_Reg l_spring[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_spring_call(lua_State * L)
{
	struct spring_t * s = luaL_checkudata(L, 1, MT_SPRING);
	double dt = luaL_checknumber(L, 2);
	if(spring_step(s, dt))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	lua_pushnumber(L, spring_position(s));
	lua_pushnumber(L, spring_velocity(s));
	return 3;
}

static const luaL_Reg m_spring[] = {
	{"__call",	m_spring_call},
	{NULL,		NULL}
};

int luaopen_spring(lua_State * L)
{
	luaL_newlib(L, l_spring);
	luahelper_create_metatable(L, MT_SPRING, m_spring);
	return 1;
}
