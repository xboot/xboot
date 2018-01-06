/*
 * framework/display/l-matrix.c
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

#include <cairo.h>
#include <cairoint.h>
#include <framework/display/l-display.h>

static int l_matrix_new(lua_State * L)
{
	cairo_matrix_t * m = lua_newuserdata(L, sizeof(cairo_matrix_t));

	if(lua_istable(L, 1) && lua_rawlen(L, 1) == 6)
	{
		lua_rawgeti(L, 1, 1); m->xx = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 2); m->yx = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 3); m->xy = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 4); m->yy = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 5); m->x0 = lua_tonumber(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 6); m->y0 = lua_tonumber(L, -1); lua_pop(L, 1);
	}
	else if(luaL_testudata(L, 1, MT_MATRIX))
		memcpy(m, lua_touserdata(L, 1), sizeof(cairo_matrix_t));
	else
		cairo_matrix_init_identity(m);
	luaL_setmetatable(L, MT_MATRIX);
	return 1;
}

static const luaL_Reg l_matrix[] = {
	{"new",	l_matrix_new},
	{NULL,	NULL}
};

static int m_matrix_tostring(lua_State * L)
{
	cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	lua_pushfstring(L, "matrix(%f,%f,%f,%f,%f,%f)", m->xx, m->yx, m->xy, m->yy, m->x0, m->y0);
	return 1;
}

static int m_matrix_invert(lua_State * L)
{
	cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	cairo_matrix_invert(m);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_multiply(lua_State * L)
{
	cairo_matrix_t * m1 = luaL_checkudata(L, 1, MT_MATRIX);
	cairo_matrix_t * m2 = luaL_checkudata(L, 2, MT_MATRIX);
	if(!lua_toboolean(L, 3))
		cairo_matrix_multiply(m1, m1, m2);
	else
		cairo_matrix_multiply(m1, m2, m1);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_translate(lua_State * L)
{
	cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double tx = luaL_checknumber(L, 2);
	double ty = luaL_checknumber(L, 3);
	cairo_matrix_translate(m, tx, ty);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_rotate(lua_State * L)
{
	cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double radians = luaL_checknumber(L, 2);
	cairo_matrix_rotate(m, radians);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_scale(lua_State * L)
{
	cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double sx = luaL_checknumber(L, 2);
	double sy = luaL_checknumber(L, 3);
	cairo_matrix_scale(m, sx, sy);
	lua_settop(L, 1);
	return 1;
}

static int m_matrix_distance(lua_State * L)
{
	const cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_matrix_transform_distance(m, &dx, &dy);
	lua_pushnumber(L, dx);
	lua_pushnumber(L, dy);
	return 2;
}

static int m_matrix_point(lua_State * L)
{
	const cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_transform_point(m, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_matrix_bounds(lua_State * L)
{
	const cairo_matrix_t * m = luaL_checkudata(L, 1, MT_MATRIX);
	double x1, y1;
	double x2, y2;
	_cairo_matrix_transform_bounding_box(m, &x1, &y1, &x2, &y2, NULL);
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
