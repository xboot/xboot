/*
 * framework/core/l-matrix.c
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
#include <framework/core/l-matrix.h>

static int l_matrix_new(lua_State * L)
{
	struct matrix_t * m;
	if(lua_istable(L, 1) && lua_rawlen(L, 1) == 6)
	{
		struct matrix_t t;
		lua_rawgeti(L, 1, 1); t.a = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 2); t.b = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 3); t.c = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 4); t.d = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 5); t.tx = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 6); t.ty = lua_tonumber(L, -1); lua_pop(L, 1);
		m = lua_newuserdata(L, sizeof(struct matrix_t));
		memcpy(m, &t, sizeof(struct matrix_t));
	}
	else if(luaL_testudata(L, 1, MT_MATRIX))
	{
		struct matrix_t * p = lua_touserdata(L, 1);
		m = lua_newuserdata(L, sizeof(struct matrix_t));
		memcpy(m, p, sizeof(struct matrix_t));
	}
	else
	{
		m = lua_newuserdata(L, sizeof(struct matrix_t));
		matrix_init_identity(m);
	}
	luaL_setmetatable(L, MT_MATRIX);
	return 1;
}

static const luaL_Reg l_matrix[] = {
	{"new",	l_matrix_new},
	{NULL,	NULL}
};

static int m_matrix_tostring(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	lua_pushfstring(L, "matrix(%f,%f,%f,%f,%f,%f)", m->a, m->b, m->c, m->d, m->tx, m->ty);
	return 1;
}

static int m_matrix_invert(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	matrix_invert(m);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_multiply(lua_State * L)
{
	struct matrix_t * m1 = luaL_checkudata(L, 1, MT_MATRIX);
	struct matrix_t * m2 = luaL_checkudata(L, 2, MT_MATRIX);
	if(!lua_toboolean(L, 3))
		matrix_multiply(m1, m1, m2);
	else
		matrix_multiply(m1, m2, m1);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_translate(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double tx = luaL_checknumber(L, 2);
	double ty = luaL_checknumber(L, 3);
	matrix_translate(m, tx, ty);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_rotate(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double radians = luaL_checknumber(L, 2);
	matrix_rotate(m, radians);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_scale(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double sx = luaL_checknumber(L, 2);
	double sy = luaL_checknumber(L, 3);
	matrix_scale(m, sx, sy);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_distance(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	matrix_transform_distance(m, &dx, &dy);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, dy);
	return 2;
}

static int m_matrix_point(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	matrix_transform_point(m, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_matrix_bounds(lua_State * L)
{
	struct matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double x1, y1;
	double x2, y2;
	matrix_transform_bounds(m, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2 - x1);
	lua_pushnumber(L, y2 - y1);
	return 4;
}

static const luaL_Reg m_matrix[] = {
	{"__tostring",	m_matrix_tostring},
	{"invert",		m_matrix_invert},
	{"multiply",	m_matrix_multiply},
	{"translate",	m_matrix_translate},
	{"rotate",		m_matrix_rotate},
	{"scale",		m_matrix_scale},
	{"distance",	m_matrix_distance},
	{"point",		m_matrix_point},
	{"bounds",		m_matrix_bounds},
	{NULL,	NULL}
};

int luaopen_matrix(lua_State * L)
{
	luaL_newlib(L, l_matrix);
	luahelper_create_metatable(L, MT_MATRIX, m_matrix);
	return 1;
}
