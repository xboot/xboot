/*
 * framework/cairo/l_cairo.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
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

static int l_cairo_create(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	cairo_t ** cr = lua_newuserdata(L, sizeof(cairo_t *));

	*cr = cairo_create(*cs);
	luaL_setmetatable(L, MT_NAME_CAIRO);
	return 1;
}

static int l_cairo_image_surface_create(lua_State * L)
{
	cairo_format_t format = (cairo_format_t)luaL_checkint(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	cairo_surface_t ** cs = lua_newuserdata(L, sizeof(cairo_surface_t *));

	*cs = cairo_image_surface_create(format, width, height);
	luaL_setmetatable(L, MT_NAME_CAIRO_SURFACE);
	return 1;
}

static const luaL_Reg l_cairo[] = {
	{"create",					l_cairo_create},
	{"image_surface_create",	l_cairo_image_surface_create},
	{NULL, NULL}
};

static int m_cairo_gc(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_destroy(*cr); *cr = 0;
	return 0;
}

static int m_cairo_tostring(lua_State * L)
{
	return 0;
}

static int m_cairo_set_line_width(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double width = luaL_checknumber(L, 2);
	cairo_set_line_width(*cr, width);
	return 0;
}

static int m_cairo_set_source_rgb(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double red = luaL_checknumber(L, 2);
	double green = luaL_checknumber(L, 3);
	double blue = luaL_checknumber(L, 4);
	cairo_set_source_rgb(*cr, red, green, blue);
	return 0;
}

static int m_cairo_set_source_rgba(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double red = luaL_checknumber(L, 2);
	double green = luaL_checknumber(L, 3);
	double blue = luaL_checknumber(L, 4);
	double alpha = luaL_checknumber(L, 5);
	cairo_set_source_rgba(*cr, red, green, blue, alpha);
	return 0;
}

static int m_cairo_arc(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5);
	double angle2 = luaL_checknumber(L, 6);
	cairo_arc(*cr, xc, yc, radius, angle1, angle2);
	return 0;
}

static int m_cairo_arc_negative(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5);
	double angle2 = luaL_checknumber(L, 6);
	cairo_arc_negative(*cr, xc, yc, radius, angle1, angle2);
	return 0;
}

static int m_cairo_move_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_move_to(*cr, x, y);
	return 0;
}

static int m_cairo_rel_move_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_move_to(*cr, dx, dy);
	return 0;
}

static int m_cairo_line_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_line_to(*cr, x, y);
	return 0;
}

static int m_cairo_rel_line_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_line_to(*cr, dx, dy);
	return 0;
}

static int m_cairo_curve_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x1 = luaL_checknumber(L, 2);
	double y1 = luaL_checknumber(L, 3);
	double x2 = luaL_checknumber(L, 4);
	double y2 = luaL_checknumber(L, 5);
	double x3 = luaL_checknumber(L, 6);
	double y3 = luaL_checknumber(L, 7);
	cairo_curve_to(*cr, x1, y1, x2, y2, x3, y3);
	return 0;
}

static int m_cairo_rel_curve_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double dx1 = luaL_checknumber(L, 2);
	double dy1 = luaL_checknumber(L, 3);
	double dx2 = luaL_checknumber(L, 4);
	double dy2 = luaL_checknumber(L, 5);
	double dx3 = luaL_checknumber(L, 6);
	double dy3 = luaL_checknumber(L, 7);
	cairo_rel_curve_to(*cr, dx1, dy1, dx2, dy2, dx3, dy3);
	return 0;
}

static int m_cairo_rectangle(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);
	cairo_rectangle(*cr, x, y, width, height);
	return 0;
}

static int m_cairo_stroke(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_stroke(*cr);
	return 0;
}

static int m_cairo_stroke_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_stroke_preserve(*cr);
	return 0;
}

static int m_cairo_fill(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_fill(*cr);
	return 0;
}

static int m_cairo_fill_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_fill_preserve(*cr);
	return 0;
}

static int m_cairo_paint(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_paint(*cr);
	return 0;
}

static int m_cairo_paint_with_alpha(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double alpha = luaL_checknumber(L, 2);
	cairo_paint_with_alpha(*cr, alpha);
	return 0;
}

static int m_cairo_save(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_save(*cr);
	return 0;
}

static int m_cairo_restore(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_restore(*cr);
	return 0;
}

static int m_cairo_new_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_new_path(*cr);
	return 0;
}

static int m_cairo_new_sub_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_new_sub_path(*cr);
	return 0;
}

static int m_cairo_close_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_close_path(*cr);
	return 0;
}

static int m_cairo_clip(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_clip(*cr);
	return 0;
}

static int m_cairo_translate(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double tx = luaL_checknumber(L, 2);
	double ty = luaL_checknumber(L, 3);
	cairo_translate(*cr, tx, ty);
	return 0;
}

static int m_cairo_scale(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double sx = luaL_checknumber(L, 2);
	double sy = luaL_checknumber(L, 3);
	cairo_scale(*cr, sx, sy);
	return 0;
}

static int m_cairo_rotate(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double angle = luaL_checknumber(L, 2);
	cairo_rotate(*cr, angle);
	return 0;
}

static int m_cairo_set_fill_rule(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_fill_rule_t fill_rule = (cairo_fill_rule_t) luaL_checkint(L, 2);
	cairo_set_fill_rule(*cr, fill_rule);
	return 0;
}

static int m_cairo_set_line_cap(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_line_cap_t line_cap = (cairo_line_cap_t) luaL_checkint(L, 2);
	cairo_set_line_cap(*cr, line_cap);
	return 0;
}

static int m_cairo_set_line_join(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_line_join_t line_join = (cairo_line_join_t) luaL_checkint(L, 2);
	cairo_set_line_join(*cr, line_join);
	return 0;
}

static const luaL_Reg m_cairo[] = {
	{"__gc",				m_cairo_gc},
	{"__tostring",			m_cairo_tostring},
	{"arc",					m_cairo_arc},
	{"arc_negative",		m_cairo_arc_negative},
	{"set_line_width",		m_cairo_set_line_width},
	{"set_source_rgb",		m_cairo_set_source_rgb},
	{"set_source_rgba",		m_cairo_set_source_rgba},
	{"move_to",				m_cairo_move_to},
	{"line_to",				m_cairo_line_to},
	{"curve_to",			m_cairo_curve_to},
	{"rectangle",			m_cairo_rectangle},
	{"rel_move_to",			m_cairo_rel_move_to},
	{"rel_line_to",			m_cairo_rel_line_to},
	{"rel_curve_to",		m_cairo_rel_curve_to},
	{"stroke",				m_cairo_stroke},
	{"stroke_preserve",		m_cairo_stroke_preserve},
	{"fill",				m_cairo_fill},
	{"fill_preserve",		m_cairo_fill_preserve},
	{"paint",				m_cairo_paint},
	{"paint_with_alpha",	m_cairo_paint_with_alpha},
	{"save",				m_cairo_save},
	{"restore",				m_cairo_restore},
	{"new_path",			m_cairo_new_path},
	{"new_sub_path",		m_cairo_new_sub_path},
	{"close_path",			m_cairo_close_path},
	{"clip",				m_cairo_clip},
	{"translate",			m_cairo_translate},
	{"scale",				m_cairo_scale},
	{"rotate",				m_cairo_rotate},
	{"set_fill_rule",		m_cairo_set_fill_rule},
	{"set_line_cap",		m_cairo_set_line_cap},
	{"set_line_join",		m_cairo_set_line_join},
	{NULL, 					NULL}
};

/*
 * xxx cairo_surface
 */
static int m_cairo_surface_gc(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	cairo_surface_destroy(*cs);
	return 0;
}

extern void show_to_framebuffer(cairo_surface_t * surface);
static int m_cairo_surface_show(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	show_to_framebuffer(*cs);
	return 0;
}

static const luaL_Reg m_cairo_surface[] = {
	{"__gc",				m_cairo_surface_gc},
	{"show",				m_cairo_surface_show},
	{NULL, 					NULL}
};


//CAIRO_FORMAT_ARGB32;

int luaopen_cairo(lua_State * L)
{
	luaL_newlib(L, l_cairo);

	luaL_newmetatable(L, MT_NAME_CAIRO);
	luaL_setfuncs(L, m_cairo, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	luaL_newmetatable(L, MT_NAME_CAIRO_SURFACE);
	luaL_setfuncs(L, m_cairo_surface, 0);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pop(L, 1);

	return 1;
}
