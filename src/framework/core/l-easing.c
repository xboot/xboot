/*
 * framework/core/l-easing.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <core/l-easing.h>

struct leasing_t {
	struct bezier_t bezier;
	double start;
	double stop;
	double duration;
};

static int l_new(lua_State * L)
{
	double start = luaL_checknumber(L, 1);
	double stop = luaL_checknumber(L, 2);
	double duration = luaL_checknumber(L, 3);
	struct leasing_t * e = lua_newuserdata(L, sizeof(struct leasing_t));
	e->start = start;
	e->stop = stop;
	e->duration = duration;
	if(lua_type(L, 4) == LUA_TSTRING)
	{
		const char * type = luaL_checkstring(L, 4);
		switch(shash(type))
		{
		case 0x0b7641e0: /* "linear" */
			bezier_init(&e->bezier, 0, 0, 1, 1);
			break;
		case 0x95154938: /* "sine-in" */
			bezier_init(&e->bezier, 0.47, 0, 0.75, 0.72);
			break;
		case 0x37be8b19: /* "sine-out" */
			bezier_init(&e->bezier, 0.39, 0.57, 0.56, 1);
			break;
		case 0x44a1b9fd: /* "sine-in-out" */
			bezier_init(&e->bezier, 0.45, 0.05, 0.55, 0.95);
			break;
		case 0x1632ec54: /* "quad-in" */
			bezier_init(&e->bezier, 0.55, 0.08, 0.68, 0.53);
			break;
		case 0xdc9091b5: /* "quad-out" */
			bezier_init(&e->bezier, 0.25, 0.46, 0.45, 0.94);
			break;
		case 0x93d38b19: /* "quad-in-out" */
			bezier_init(&e->bezier, 0.46, 0.03, 0.52, 0.96);
			break;
		case 0xf3c312af: /* "cubic-in" */
			bezier_init(&e->bezier, 0.55, 0.06, 0.68, 0.19);
			break;
		case 0x6c258370: /* "cubic-out" */
			bezier_init(&e->bezier, 0.22, 0.61, 0.36, 1);
			break;
		case 0x6f7566f4: /* "cubic-in-out" */
			bezier_init(&e->bezier, 0.65, 0.05, 0.36, 1);
			break;
		case 0xddb3bd56: /* "quart-in" */
			bezier_init(&e->bezier, 0.9, 0.03, 0.69, 0.22);
			break;
		case 0x942b82f7: /* "quart-out" */
			bezier_init(&e->bezier, 0.17, 0.84, 0.44, 1);
			break;
		case 0xe1790d1b: /* "quart-in-out" */
			bezier_init(&e->bezier, 0.77, 0, 0.18, 1);
			break;
		case 0xf014a05a: /* "quint-in" */
			bezier_init(&e->bezier, 0.76, 0.05, 0.86, 0.06);
			break;
		case 0xf2a8c67b: /* "quint-out" */
			bezier_init(&e->bezier, 0.23, 1, 0.32, 1);
			break;
		case 0x2fdbd21f: /* "quint-in-out" */
			bezier_init(&e->bezier, 0.79, 0.14, 0.15, 0.86);
			break;
		case 0x828d04e5: /* "expo-in" */
			bezier_init(&e->bezier, 0.95, 0.05, 0.8, 0.04);
			break;
		case 0xd42dbc66: /* "expo-out" */
			bezier_init(&e->bezier, 0.19, 1, 0.22, 1);
			break;
		case 0x59b9842a: /* "expo-in-out" */
			bezier_init(&e->bezier, 0.86, 0, 0.07, 1);
			break;
		case 0xc5b8c66a: /* "circ-in" */
			bezier_init(&e->bezier, 0.6, 0.04, 0.98, 0.34);
			break;
		case 0x7cd1ae8b: /* "circ-out" */
			bezier_init(&e->bezier, 0.08, 0.82, 0.17, 1);
			break;
		case 0xe844802f: /* "circ-in-out" */
			bezier_init(&e->bezier, 1, 0, 0, 1);
			break;
		case 0x650a381a: /* "back-in" */
			bezier_init(&e->bezier, 0.6, -0.28, 0.74, 0.05);
			break;
		case 0x0651563b: /* "back-out" */
			bezier_init(&e->bezier, 0.18, 0.89, 0.32, 1.27);
			break;
		case 0xd15749df: /* "back-in-out" */
			bezier_init(&e->bezier, 0.68, -0.55, 0.27, 1.55);
			break;
		default:
			bezier_init(&e->bezier, 0, 0, 1, 1);
			break;
		}
	}
	else if(lua_istable(L, 4) && (lua_rawlen(L, 4) == 4))
	{
		double x1, y1;
		double x2, y2;
		lua_rawgeti(L, 4, 1); x1 = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 4, 2); y1 = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 4, 3); x2 = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 4, 4); y2 = lua_tonumber(L, -1); lua_pop(L, 1);
		bezier_init(&e->bezier, x1, y1, x2, y2);
	}
	else
	{
		bezier_init(&e->bezier, 0, 0, 1, 1);
	}
	luaL_setmetatable(L, MT_EASING);
	return 1;
}

static const luaL_Reg l_easing[] = {
	{"new", l_new},
	{NULL, NULL}
};

static int m_easing_call(lua_State * L)
{
	struct leasing_t * e = luaL_checkudata(L, 1, MT_EASING);
	double t = luaL_checknumber(L, 2);
	double r = (e->stop - e->start) * bezier_calc(&e->bezier, t / e->duration) + e->start;
	lua_pushnumber(L, r);
	return 1;
}

static const luaL_Reg m_easing[] = {
	{"__call",	m_easing_call},
	{NULL,		NULL}
};

int luaopen_easing(lua_State * L)
{
	luaL_newlib(L, l_easing);
	luahelper_create_metatable(L, MT_EASING, m_easing);
	return 1;
}
