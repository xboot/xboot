/*
 * framework/cairo/l_cairo_matrix.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <framework/cairo/l_cairo.h>

int l_cairo_matrix_create(lua_State * L)
{
	double xx = luaL_optnumber(L, 1, 0);
	double yx = luaL_optnumber(L, 2, 0);
	double xy = luaL_optnumber(L, 3, 0);
	double yy = luaL_optnumber(L, 4, 0);
	double x0 = luaL_optnumber(L, 5, 0);
	double y0 = luaL_optnumber(L, 6, 0);
	cairo_matrix_t * matrix = lua_newuserdata(L, sizeof(cairo_matrix_t));
	cairo_matrix_init(matrix, xx, yx, xy, yy, x0, y0);
	luaL_setmetatable(L, MT_NAME_CAIRO_MATRIX);
	return 1;
}

static int m_cairo_matrix_init(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double xx = luaL_checknumber(L, 2);
	double yx = luaL_checknumber(L, 3);
	double xy = luaL_checknumber(L, 4);
	double yy = luaL_checknumber(L, 5);
	double x0 = luaL_checknumber(L, 6);
	double y0 = luaL_checknumber(L, 7);
	cairo_matrix_init(matrix, xx, yx, xy, yy, x0, y0);
	return 0;
}

static int m_cairo_matrix_init_identity(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	cairo_matrix_init_identity(matrix);
	return 0;
}

static int m_cairo_matrix_init_rotate(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double radians = luaL_checknumber(L, 2);
	cairo_matrix_init_rotate(matrix, radians);
	return 0;
}

static int m_cairo_matrix_init_scale(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double sx = luaL_checknumber(L, 2);
	double sy = luaL_checknumber(L, 3);
	cairo_matrix_init_scale(matrix, sx, sy);
	return 0;
}

static int m_cairo_matrix_init_translate(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double tx = luaL_checknumber(L, 2);
	double ty = luaL_checknumber(L, 3);
	cairo_matrix_init_translate(matrix, tx, ty);
	return 0;
}

static int m_cairo_matrix_invert(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	cairo_status_t v = cairo_matrix_invert(matrix);
	lua_pushinteger(L, v);
	return 1;
}

static int m_cairo_matrix_multiply(lua_State * L)
{
	cairo_matrix_t * result = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	const cairo_matrix_t * a = luaL_checkudata(L, 2, MT_NAME_CAIRO_MATRIX);
	const cairo_matrix_t * b = luaL_checkudata(L, 3, MT_NAME_CAIRO_MATRIX);
	cairo_matrix_multiply(result, a, b);
	return 0;
}

static int m_cairo_matrix_rotate(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double radians = luaL_checknumber(L, 2);
	cairo_matrix_rotate(matrix, radians);
	return 0;
}

static int m_cairo_matrix_scale(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double sx = luaL_checknumber(L, 2);
	double sy = luaL_checknumber(L, 3);
	cairo_matrix_scale(matrix, sx, sy);
	return 0;
}

static int m_cairo_matrix_transform_distance(lua_State * L)
{
	const cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_matrix_transform_distance(matrix, &dx, &dy);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, dy);
	return 2;
}

static int m_cairo_matrix_transform_point(lua_State * L)
{
	const cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_transform_point(matrix, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_cairo_matrix_translate(lua_State * L)
{
	cairo_matrix_t * matrix = luaL_checkudata(L, 1, MT_NAME_CAIRO_MATRIX);
	double tx = luaL_checknumber(L, 2);
	double ty = luaL_checknumber(L, 3);
	cairo_matrix_translate(matrix, tx, ty);
	return 0;
}

const luaL_Reg m_cairo_matrix[] = {
	{"init",				m_cairo_matrix_init},
	{"init_identity",		m_cairo_matrix_init_identity},
	{"init_rotate",			m_cairo_matrix_init_rotate},
	{"init_scale",			m_cairo_matrix_init_scale},
	{"init_translate",		m_cairo_matrix_init_translate},
	{"invert",				m_cairo_matrix_invert},
	{"multiply",			m_cairo_matrix_multiply},
	{"rotate",				m_cairo_matrix_rotate},
	{"scale",				m_cairo_matrix_scale},
	{"transform_distance",	m_cairo_matrix_transform_distance},
	{"transform_point",		m_cairo_matrix_transform_point},
	{"translate",			m_cairo_matrix_translate},
	{NULL,					NULL}
};
