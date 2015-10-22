/*
 * framework/display/l-matrix.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <cairo.h>
#include <framework/display/l-display.h>

static int l_matrix_new(lua_State * L)
{
	cairo_matrix_t * matrix = lua_newuserdata(L, sizeof(cairo_matrix_t));
	cairo_matrix_init_identity(matrix);
	luaL_setmetatable(L, MT_MATRIX);
	return 1;
}

static const luaL_Reg l_matrix[] = {
	{"new",	l_matrix_new},
	{NULL,	NULL}
};

static int m_matrix_init(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_MATRIX);
	double xx = luaL_optnumber(L, 2, 1);
	double yx = luaL_optnumber(L, 3, 0);
	double xy = luaL_optnumber(L, 4, 0);
	double yy = luaL_optnumber(L, 5, 1);
	double x0 = luaL_optnumber(L, 6, 0);
	double y0 = luaL_optnumber(L, 7, 0);
	cairo_matrix_init(matrix, xx, yx, xy, yy, x0, y0);
	return 0;
}

static int m_matrix_invert(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_MATRIX);
	cairo_status_t v = cairo_matrix_invert(matrix);
	lua_pushinteger(L, v);
	return 1;
}

static int m_matrix_multiply(lua_State * L)
{
	cairo_matrix_t * result = luaL_checkudata(L, 1, MT_MATRIX);
	const cairo_matrix_t * a = luaL_checkudata(L, 2, MT_MATRIX);
	const cairo_matrix_t * b = luaL_checkudata(L, 3, MT_MATRIX);
	cairo_matrix_multiply(result, a, b);
	return 0;
}

static int m_matrix_rotate(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_MATRIX);
	double radians = luaL_checknumber(L, 2);
	cairo_matrix_rotate(matrix, radians);
	return 0;
}

static int m_matrix_scale(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_MATRIX);
	double sx = luaL_checknumber(L, 2);
	double sy = luaL_checknumber(L, 3);
	cairo_matrix_scale(matrix, sx, sy);
	return 0;
}

static int m_matrix_transform_distance(lua_State * L)
{
	const cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_MATRIX);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_matrix_transform_distance(matrix, &dx, &dy);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, dy);
	return 2;
}

static int m_matrix_transform_point(lua_State * L)
{
	const cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_MATRIX);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_transform_point(matrix, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_matrix_translate(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_MATRIX);
	double tx = luaL_checknumber(L, 2);
	double ty = luaL_checknumber(L, 3);
	cairo_matrix_translate(matrix, tx, ty);
	return 0;
}

static const luaL_Reg m_matrix[] = {
	{"init",				m_matrix_init},
	{"invert",				m_matrix_invert},
	{"multiply",			m_matrix_multiply},
	{"rotate",				m_matrix_rotate},
	{"scale",				m_matrix_scale},
	{"transformDistance",	m_matrix_transform_distance},
	{"transformPoint",		m_matrix_transform_point},
	{"translate",			m_matrix_translate},
	{NULL,					NULL}
};

int luaopen_matrix(lua_State * L)
{
	luaL_newlib(L, l_matrix);
	luahelper_create_metatable(L, MT_MATRIX, m_matrix);
	return 1;
}
