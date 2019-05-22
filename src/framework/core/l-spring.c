/*
 * framework/core/l-spring.c
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

#include <xboot.h>
#include <framework/core/l-spring.h>

/*
 * https://chenglou.github.io/react-motion/demos/demo5-spring-parameters-chooser/
 *
 * no-wobble: stiffness = 170, damping = 26
 * gentle: stiffness = 120, damping = 14
 * wobble: stiffness = 180, damping = 12
 * stiff: stiffness = 210, damping = 20
 */
struct lspring_t {
	double start;
	double stop;
	double velocity;
	double stiffness;
	double damping;
};

static int l_new(lua_State * L)
{
	double start = luaL_optnumber(L, 1, 0);
	double stop = luaL_optnumber(L, 2, 1);
	double velocity = luaL_optnumber(L, 3, 0);
	double stiffness = luaL_optnumber(L, 4, 170);
	double damping = luaL_optnumber(L, 5, 26);
	struct lspring_t * s = lua_newuserdata(L, sizeof(struct lspring_t));
	s->start = start;
	s->stop = stop;
	s->velocity = velocity;
	s->stiffness = stiffness;
	s->damping = damping;
	luaL_setmetatable(L, MT_SPRING);
	return 1;
}

static const luaL_Reg l_spring[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_spring_call(lua_State * L)
{
	struct lspring_t * s = luaL_checkudata(L, 1, MT_SPRING);
	double delta = luaL_checknumber(L, 2);
	double nv = s->velocity + (s->stiffness * (s->stop - s->start) - s->damping * s->velocity) * delta;
	double ns = s->start + nv * delta;
	if((abs(nv) < 0.01) && (abs(ns - s->stop) < 0.01))
	{
		s->start = s->stop;
		s->velocity = 0;
		lua_pushboolean(L, 0);
	}
	else
	{
		s->start = ns;
		s->velocity = nv;
		lua_pushboolean(L, 1);
	}
	lua_pushnumber(L, s->start);
	lua_pushnumber(L, s->velocity);
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
