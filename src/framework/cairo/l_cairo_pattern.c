/*
 * framework/cairo/l_cairo_pattern.c
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

int l_cairo_pattern_create_rgb(lua_State * L)
{
	double red = luaL_checknumber(L, 1);
	double green = luaL_checknumber(L, 2);
	double blue = luaL_checknumber(L, 3);
	cairo_pattern_t ** pattern = lua_newuserdata(L, sizeof(cairo_pattern_t *));
	*pattern = cairo_pattern_create_rgb(red, green, blue);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATTERN);
	return 1;
}

int l_cairo_pattern_create_rgba(lua_State * L)
{
	double red = luaL_checknumber(L, 1);
	double green = luaL_checknumber(L, 2);
	double blue = luaL_checknumber(L, 3);
	double alpha = luaL_checknumber(L, 3);
	cairo_pattern_t ** pattern = lua_newuserdata(L, sizeof(cairo_pattern_t *));
	*pattern = cairo_pattern_create_rgba(red, green, blue, alpha);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATTERN);
	return 1;
}

int l_cairo_pattern_create_for_surface(lua_State * L)
{
	cairo_surface_t ** surface = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	cairo_pattern_t ** pattern = lua_newuserdata(L, sizeof(cairo_pattern_t *));
	*pattern = cairo_pattern_create_for_surface(*surface);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATTERN);
	return 1;
}

int l_cairo_pattern_create_linear(lua_State * L)
{
	double x0 = luaL_checknumber(L, 1);
	double y0 = luaL_checknumber(L, 2);
	double x1 = luaL_checknumber(L, 3);
	double y1 = luaL_checknumber(L, 4);
	cairo_pattern_t ** pattern = lua_newuserdata(L, sizeof(cairo_pattern_t *));
	*pattern = cairo_pattern_create_linear(x0, y0, x1, y1);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATTERN);
	return 1;
}

int l_cairo_pattern_create_radial(lua_State * L)
{
	double cx0 = luaL_checknumber(L, 1);
	double cy0 = luaL_checknumber(L, 2);
	double radius0 = luaL_checknumber(L, 3);
	double cx1 = luaL_checknumber(L, 4);
	double cy1 = luaL_checknumber(L, 5);
	double radius1 = luaL_checknumber(L, 6);
	cairo_pattern_t ** pattern = lua_newuserdata(L, sizeof(cairo_pattern_t *));
	*pattern = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATTERN);
	return 1;
}

static int m_cairo_pattern_eq(lua_State * L)
{
	cairo_pattern_t ** pattern1 = luaL_checkudata(L, 1, MT_NAME_CAIRO_PATTERN);
	cairo_pattern_t ** pattern2 = luaL_checkudata(L, 2, MT_NAME_CAIRO_PATTERN);
	lua_pushboolean(L, ((*pattern1) == (*pattern2)));
	return 1;
}

static int m_cairo_pattern_gc(lua_State * L)
{
	cairo_pattern_t ** pattern = luaL_checkudata(L, 1, MT_NAME_CAIRO_PATTERN);
	cairo_pattern_destroy(*pattern);
	return 0;
}

static int m_cairo_pattern_add_color_stop_rgb(lua_State * L)
{
	cairo_pattern_t ** pattern = luaL_checkudata(L, 1, MT_NAME_CAIRO_PATTERN);
	double offset = luaL_checknumber(L, 2);
	double red = luaL_checknumber(L, 3);
	double green = luaL_checknumber(L, 4);
	double blue = luaL_checknumber(L, 5);
	cairo_pattern_add_color_stop_rgb(*pattern, offset, red, green, blue);
	return 0;
}

static int m_cairo_pattern_add_color_stop_rgba(lua_State * L)
{
	cairo_pattern_t ** pattern = luaL_checkudata(L, 1, MT_NAME_CAIRO_PATTERN);
	double offset = luaL_checknumber(L, 2);
	double red = luaL_checknumber(L, 3);
	double green = luaL_checknumber(L, 4);
	double blue = luaL_checknumber(L, 5);
	double alpha = luaL_checknumber(L, 6);
	cairo_pattern_add_color_stop_rgba(*pattern, offset, red, green, blue, alpha);
	return 0;
}

static int m_cairo_pattern_set_matrix(lua_State * L)
{
	cairo_pattern_t ** pattern = luaL_checkudata(L, 1, MT_NAME_CAIRO_PATTERN);
	cairo_matrix_t * matrix = luaL_checkudata(L, 2, MT_NAME_CAIRO_MATRIX);
	cairo_pattern_set_matrix(*pattern, matrix);
	return 0;
}

static int m_cairo_pattern_set_extend(lua_State * L)
{
	cairo_pattern_t ** pattern = luaL_checkudata(L, 1, MT_NAME_CAIRO_PATTERN);
	cairo_extend_t extend = (cairo_extend_t)luaL_checkinteger(L, 2);
	cairo_pattern_set_extend(*pattern, extend);
	return 0;
}

const luaL_Reg m_cairo_pattern[] = {
	{"__eq",				m_cairo_pattern_eq},
	{"__gc",				m_cairo_pattern_gc},
	{"add_color_stop_rgb",	m_cairo_pattern_add_color_stop_rgb},
	{"add_color_stop_rgba",	m_cairo_pattern_add_color_stop_rgba},
	{"set_matrix",			m_cairo_pattern_set_matrix},
	{"set_extend",			m_cairo_pattern_set_extend},
	{NULL, 					NULL}
};
