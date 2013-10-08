/*
 * framework/display/l-shape.c
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
#include <framework/display/l-display.h>

static int l_shape_new(lua_State * L)
{
	cairo_rectangle_t extents = {
		.x		= 0,
		.y		= 0,
		.width  = luaL_optnumber(L, 1, 1),
		.height = luaL_optnumber(L, 2, 1),
	};
	cairo_surface_t * surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, &extents);
	cairo_t ** cr = lua_newuserdata(L, sizeof(cairo_t *));
	*cr = cairo_create(surface);
	cairo_surface_destroy(surface);
	luaL_setmetatable(L, MT_NAME_SHAPE);
	return 1;
}

static const luaL_Reg l_shape[] = {
	{"new",	l_shape_new},
	{NULL,	NULL}
};

static int m_shape_gc(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_destroy(*cr);
	return 0;
}

static int m_shape_move_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_move_to(*cr, x, y);
	return 0;
}

static int m_shape_rel_move_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_move_to(*cr, dx, dy);
	return 0;
}

static int m_shape_line_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_line_to(*cr, x, y);
	return 0;
}

static int m_shape_rel_line_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_line_to(*cr, dx, dy);
	return 0;
}

static int m_shape_rectangle(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);
	cairo_rectangle(*cr, x, y, width, height);
	return 0;
}

static int m_shape_curve_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double x1 = luaL_checknumber(L, 2);
	double y1 = luaL_checknumber(L, 3);
	double x2 = luaL_checknumber(L, 4);
	double y2 = luaL_checknumber(L, 5);
	double x3 = luaL_checknumber(L, 6);
	double y3 = luaL_checknumber(L, 7);
	cairo_curve_to(*cr, x1, y1, x2, y2, x3, y3);
	return 0;
}

static int m_shape_rel_curve_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double dx1 = luaL_checknumber(L, 2);
	double dy1 = luaL_checknumber(L, 3);
	double dx2 = luaL_checknumber(L, 4);
	double dy2 = luaL_checknumber(L, 5);
	double dx3 = luaL_checknumber(L, 6);
	double dy3 = luaL_checknumber(L, 7);
	cairo_rel_curve_to(*cr, dx1, dy1, dx2, dy2, dx3, dy3);
	return 0;
}

static int m_shape_arc(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5);
	double angle2 = luaL_checknumber(L, 6);
	cairo_arc(*cr, xc, yc, radius, angle1, angle2);
	return 0;
}

static int m_shape_arc_negative(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5);
	double angle2 = luaL_checknumber(L, 6);
	cairo_arc_negative(*cr, xc, yc, radius, angle1, angle2);
	return 0;
}

static int m_shape_set_line_width(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double width = luaL_checknumber(L, 2);
	cairo_set_line_width(*cr, width);
	return 0;
}

static int m_shape_set_source(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_pattern_t ** pattern = luaL_checkudata(L, 2, MT_NAME_PARTTERN);
	cairo_set_source(*cr, *pattern);
	return 0;
}

static int m_shape_set_source_rgba(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double red = luaL_checknumber(L, 2);
	double green = luaL_checknumber(L, 3);
	double blue = luaL_checknumber(L, 4);
	double alpha = luaL_optnumber(L, 5, 1);
	cairo_set_source_rgba(*cr, red, green, blue, alpha);
	return 0;
}

static int m_shape_stroke(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_stroke(*cr);
	return 0;
}

static int m_shape_stroke_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_stroke_preserve(*cr);
	return 0;
}

static int m_shape_fill(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_fill(*cr);
	return 0;
}

static int m_shape_fill_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_fill_preserve(*cr);
	return 0;
}

static int m_shape_clip(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_clip(*cr);
	return 0;
}

static int m_shape_clip_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_clip_preserve(*cr);
	return 0;
}

static int m_shape_paint(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_paint(*cr);
	return 0;
}

static int m_shape_paint_with_alpha(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	double alpha = luaL_checknumber(L, 2);
	cairo_paint_with_alpha(*cr, alpha);
	return 0;
}

static int m_shape_save(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_save(*cr);
	return 0;
}

static int m_shape_restore(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_restore(*cr);
	return 0;
}

static int m_shape_new_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_new_path(*cr);
	return 0;
}

static int m_shape_new_sub_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_new_sub_path(*cr);
	return 0;
}

static int m_shape_close_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_close_path(*cr);
	return 0;
}

static int m_shape_size(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_SHAPE);
	cairo_surface_t * surface = cairo_get_target(*cr);
	cairo_rectangle_t r;
	if(cairo_recording_surface_get_extents(surface, &r))
	{
		lua_pushnumber(L, r.width);
		lua_pushnumber(L, r.height);
	}
	else
	{
		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);
	}
	return 2;
}

static const luaL_Reg m_shape[] = {
	{"__gc",				m_shape_gc},
	{"arc",					m_shape_arc},
	{"arc_negative",		m_shape_arc_negative},
	{"clip",				m_shape_clip},
	{"clip_preserve",		m_shape_clip_preserve},
	{"close_path",			m_shape_close_path},
	{"curve_to",			m_shape_curve_to},
	{"fill",				m_shape_fill},
	{"fill_preserve",		m_shape_fill_preserve},
	{"line_to",				m_shape_line_to},
	{"move_to",				m_shape_move_to},
	{"new_path",			m_shape_new_path},
	{"new_sub_path",		m_shape_new_sub_path},
	{"paint",				m_shape_paint},
	{"paint_with_alpha",	m_shape_paint_with_alpha},
	{"rectangle",			m_shape_rectangle},
	{"rel_curve_to",		m_shape_rel_curve_to},
	{"rel_line_to",			m_shape_rel_line_to},
	{"rel_move_to",			m_shape_rel_move_to},
	{"restore",				m_shape_restore},
	{"save",				m_shape_save},
	{"set_line_width",		m_shape_set_line_width},
	{"set_source_rgba",		m_shape_set_source_rgba},
	{"set_source",			m_shape_set_source},
	{"stroke",				m_shape_stroke},
	{"stroke_preserve",		m_shape_stroke_preserve},
	{"size",				m_shape_size},
	{NULL,					NULL}
};

int luaopen_shape(lua_State * L)
{
	luaL_newlib(L, l_shape);
	luahelper_create_metatable(L, MT_NAME_SHAPE, m_shape);
	return 1;
}
