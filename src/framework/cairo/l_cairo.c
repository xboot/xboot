/*
 * framework/cairo/l_cairo.c
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

static int l_cairo_create(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	cairo_t ** cr = lua_newuserdata(L, sizeof(cairo_t *));
	*cr = cairo_create(*cs);
	luaL_setmetatable(L, MT_NAME_CAIRO);
	return 1;
}

static const luaL_Reg l_cairo[] = {
	{"create",							l_cairo_create},
	{"surface_create_similar",			l_cairo_surface_create_similar},
	{"image_surface_create",			l_cairo_image_surface_create},
	{"image_surface_create_from_png",	l_cairo_image_surface_create_from_png},
	{"pattern_create_rgb",				l_cairo_pattern_create_rgb},
	{"pattern_create_rgba",				l_cairo_pattern_create_rgba},
	{"pattern_create_for_surface",		l_cairo_pattern_create_for_surface},
	{"pattern_create_linear",			l_cairo_pattern_create_linear},
	{"pattern_create_radial",			l_cairo_pattern_create_radial},
	{"matrix_create",					l_cairo_matrix_create},
	{NULL, NULL}
};

static int m_cairo_gc(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_destroy(*cr);
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

static int m_cairo_append_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_path_t ** path = luaL_checkudata(L, 2, MT_NAME_CAIRO_PATH);
	cairo_append_path(*cr, *path);
	return 0;
}

static int m_cairo_copy_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_path_t ** path = lua_newuserdata(L, sizeof(cairo_path_t *));
	*path = cairo_copy_path(*cr);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATH);
	return 1;
}

static int m_cairo_copy_path_flat(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_path_t ** path = lua_newuserdata(L, sizeof(cairo_path_t *));
	*path = cairo_copy_path_flat(*cr);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATH);
	return 1;
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

static int m_cairo_stroke_extents(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x1 = luaL_optnumber(L, 2, 0.0);
	double y1 = luaL_optnumber(L, 3, 0.0);
	double x2 = luaL_optnumber(L, 4, 0.0);
	double y2 = luaL_optnumber(L, 5, 0.0);
	cairo_stroke_extents(*cr, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
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

static int m_cairo_fill_extents(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x1 = luaL_optnumber(L, 2, 0.0);
	double y1 = luaL_optnumber(L, 3, 0.0);
	double x2 = luaL_optnumber(L, 4, 0.0);
	double y2 = luaL_optnumber(L, 5, 0.0);
	cairo_fill_extents(*cr, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
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

static int m_cairo_clip_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_clip_preserve(*cr);
	return 0;
}

static int m_cairo_clip_extents(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x1 = luaL_optnumber(L, 2, 0.0);
	double y1 = luaL_optnumber(L, 3, 0.0);
	double x2 = luaL_optnumber(L, 4, 0.0);
	double y2 = luaL_optnumber(L, 5, 0.0);
	cairo_clip_extents(*cr, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
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

static int m_cairo_device_to_user(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_device_to_user(*cr, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_cairo_device_to_user_distance(lua_State *L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_device_to_user_distance(*cr, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_cairo_path_extents(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double x1 = luaL_optnumber(L, 2, 0.0);
	double y1 = luaL_optnumber(L, 3, 0.0);
	double x2 = luaL_optnumber(L, 4, 0.0);
	double y2 = luaL_optnumber(L, 5, 0.0);
	cairo_path_extents(*cr, &x1, &y1, &x2, &y2);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2);
	lua_pushnumber(L, y2);
	return 4;
}

static int m_cairo_reset_clip(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_reset_clip(*cr);
	return 0;
}

static int m_cairo_set_source_surface(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_surface_t ** cs = luaL_checkudata(L, 2, MT_NAME_CAIRO_SURFACE);
	double x = luaL_optnumber(L, 3, 0);
	double y = luaL_optnumber(L, 4, 0);
	cairo_set_source_surface(*cr, *cs, x, y);
	return 0;
}

static int m_cairo_set_source(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_pattern_t ** source = luaL_checkudata(L, 2, MT_NAME_CAIRO_PATTERN);
	cairo_set_source(*cr, *source);
	return 0;
}

static int m_cairo_set_dash(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	double offset = luaL_checknumber(L, 3);
	double * dashes;
	int num_dashes, i;

	luaL_checktype(L, 2, LUA_TTABLE);
	num_dashes = (int)lua_rawlen(L, 2);
	if(num_dashes > 0)
	{
		dashes = malloc(num_dashes * sizeof(double));

		for(i = 0; i < num_dashes; i++)
		{
			lua_rawgeti(L, 2, i + 1);
			dashes[i] = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}

		cairo_set_dash(*cr, dashes, num_dashes, offset);
		free(dashes);
	}

	return 0;
}

static int m_cairo_set_operator(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_operator_t op = (cairo_operator_t)luaL_checkinteger(L, 2);
	cairo_set_operator(*cr, op);
	return 0;
}

static int m_cairo_push_group(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
    cairo_content_t content = (cairo_content_t)luaL_optinteger(L, 2, CAIRO_CONTENT_COLOR_ALPHA);
    cairo_push_group_with_content(*cr, content);
	return 0;
}

static int m_cairo_pop_group(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_pattern_t ** pattern = lua_newuserdata(L, sizeof(cairo_pattern_t *));
	*pattern = cairo_pop_group(*cr);
	luaL_setmetatable(L, MT_NAME_CAIRO_PATTERN);
	return 1;

	cairo_pattern_t *v = cairo_pop_group(*cr);
	lua_pushlightuserdata(L, v);
	return 1;
}

static int m_cairo_pop_group_to_source(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_NAME_CAIRO);
	cairo_pop_group_to_source(*cr);
	return 0;
}

static const luaL_Reg m_cairo[] = {
	{"__gc",					m_cairo_gc},
	{"append_path",				m_cairo_append_path},
	{"copy_path",				m_cairo_copy_path},
	{"copy_path_flat",			m_cairo_copy_path_flat},
	{"arc",						m_cairo_arc},
	{"arc_negative",			m_cairo_arc_negative},
	{"set_line_width",			m_cairo_set_line_width},
	{"set_source_rgb",			m_cairo_set_source_rgb},
	{"set_source_rgba",			m_cairo_set_source_rgba},
	{"move_to",					m_cairo_move_to},
	{"line_to",					m_cairo_line_to},
	{"curve_to",				m_cairo_curve_to},
	{"rectangle",				m_cairo_rectangle},
	{"rel_move_to",				m_cairo_rel_move_to},
	{"rel_line_to",				m_cairo_rel_line_to},
	{"rel_curve_to",			m_cairo_rel_curve_to},
	{"stroke",					m_cairo_stroke},
	{"stroke_preserve",			m_cairo_stroke_preserve},
	{"stroke_extents",			m_cairo_stroke_extents},
	{"fill",					m_cairo_fill},
	{"fill_preserve",			m_cairo_fill_preserve},
	{"fill_extents",			m_cairo_fill_extents},
	{"paint",					m_cairo_paint},
	{"paint_with_alpha",		m_cairo_paint_with_alpha},
	{"save",					m_cairo_save},
	{"restore",					m_cairo_restore},
	{"new_path",				m_cairo_new_path},
	{"new_sub_path",			m_cairo_new_sub_path},
	{"close_path",				m_cairo_close_path},
	{"clip",					m_cairo_clip},
	{"clip_preserve",			m_cairo_clip_preserve},
	{"clip_extents",			m_cairo_clip_extents},
	{"translate",				m_cairo_translate},
	{"scale",					m_cairo_scale},
	{"rotate",					m_cairo_rotate},
	{"set_fill_rule",			m_cairo_set_fill_rule},
	{"set_line_cap",			m_cairo_set_line_cap},
	{"set_line_join",			m_cairo_set_line_join},
	{"device_to_user",			m_cairo_device_to_user},
	{"device_to_user_distance",	m_cairo_device_to_user_distance},
	{"path_extents",			m_cairo_path_extents},
	{"reset_clip",				m_cairo_reset_clip},
	{"set_source_surface",		m_cairo_set_source_surface},
	{"set_source",				m_cairo_set_source},
	{"set_dash",				m_cairo_set_dash},
	{"set_operator",			m_cairo_set_operator},
	{"push_group",				m_cairo_push_group},
	{"pop_group",				m_cairo_pop_group},
	{"pop_group_to_source",		m_cairo_pop_group_to_source},
	{NULL, 						NULL}
};

int luaopen_cairo(lua_State * L)
{
	luaL_newlib(L, l_cairo);

	/* cairo_format_t */
	luahelper_set_intfield(L,	"FORMAT_INVALID",			CAIRO_FORMAT_INVALID);
	luahelper_set_intfield(L,	"FORMAT_ARGB32",			CAIRO_FORMAT_ARGB32);
	luahelper_set_intfield(L,	"FORMAT_RGB24",				CAIRO_FORMAT_RGB24);
	luahelper_set_intfield(L,	"FORMAT_A8",				CAIRO_FORMAT_A8);
	luahelper_set_intfield(L,	"FORMAT_A1",				CAIRO_FORMAT_A1);
	luahelper_set_intfield(L,	"FORMAT_RGB16_565",			CAIRO_FORMAT_RGB16_565);
	luahelper_set_intfield(L,	"FORMAT_RGB30",				CAIRO_FORMAT_RGB30);

	/* cairo_fill_rule_t */
	luahelper_set_intfield(L, 	"FILL_RULE_WINDING",		CAIRO_FILL_RULE_WINDING);
	luahelper_set_intfield(L, 	"FILL_RULE_EVEN_ODD",		CAIRO_FILL_RULE_EVEN_ODD);

	/* cairo_line_cap_t */
	luahelper_set_intfield(L,	"LINE_CAP_BUTT", 			CAIRO_LINE_CAP_BUTT);
	luahelper_set_intfield(L,	"LINE_CAP_ROUND", 			CAIRO_LINE_CAP_ROUND);
	luahelper_set_intfield(L,	"LINE_CAP_SQUARE", 			CAIRO_LINE_CAP_SQUARE);

	/* cairo_line_join_t */
	luahelper_set_intfield(L,	"LINE_JOIN_MITER",			CAIRO_LINE_JOIN_MITER);
	luahelper_set_intfield(L,	"LINE_JOIN_ROUND",			CAIRO_LINE_JOIN_ROUND);
	luahelper_set_intfield(L,	"LINE_JOIN_BEVEL",			CAIRO_LINE_JOIN_BEVEL);

	/* cairo_extend_t */
	luahelper_set_intfield(L,	"EXTEND_NONE",				CAIRO_EXTEND_NONE);
	luahelper_set_intfield(L,	"EXTEND_REPEAT",			CAIRO_EXTEND_REPEAT);
	luahelper_set_intfield(L,	"EXTEND_REFLECT",			CAIRO_EXTEND_REFLECT);
	luahelper_set_intfield(L,	"EXTEND_PAD",				CAIRO_EXTEND_PAD);

	/* cairo_content_t */
	luahelper_set_intfield(L,	"CONTENT_COLOR",			CAIRO_CONTENT_COLOR);
	luahelper_set_intfield(L,	"CONTENT_ALPHA",			CAIRO_CONTENT_ALPHA);
	luahelper_set_intfield(L,	"CONTENT_COLOR_ALPHA",		CAIRO_CONTENT_COLOR_ALPHA);

	/* cairo_operator_t */
	luahelper_set_intfield(L,	"OPERATOR_CLEAR",			CAIRO_OPERATOR_CLEAR);
	luahelper_set_intfield(L,	"OPERATOR_SOURCE",			CAIRO_OPERATOR_SOURCE);
	luahelper_set_intfield(L,	"OPERATOR_OVER",			CAIRO_OPERATOR_OVER);
	luahelper_set_intfield(L,	"OPERATOR_IN",				CAIRO_OPERATOR_IN);
	luahelper_set_intfield(L,	"OPERATOR_OUT",				CAIRO_OPERATOR_OUT);
	luahelper_set_intfield(L,	"OPERATOR_ATOP",			CAIRO_OPERATOR_ATOP);
	luahelper_set_intfield(L,	"OPERATOR_DEST",			CAIRO_OPERATOR_DEST);
	luahelper_set_intfield(L,	"OPERATOR_DEST_OVER",		CAIRO_OPERATOR_DEST_OVER);
	luahelper_set_intfield(L,	"OPERATOR_DEST_IN",			CAIRO_OPERATOR_DEST_IN);
	luahelper_set_intfield(L,	"OPERATOR_DEST_OUT",		CAIRO_OPERATOR_DEST_OUT);
	luahelper_set_intfield(L,	"OPERATOR_DEST_ATOP",		CAIRO_OPERATOR_DEST_ATOP);
	luahelper_set_intfield(L,	"OPERATOR_XOR",				CAIRO_OPERATOR_XOR);
	luahelper_set_intfield(L,	"OPERATOR_ADD",				CAIRO_OPERATOR_ADD);
	luahelper_set_intfield(L,	"OPERATOR_SATURATE",		CAIRO_OPERATOR_SATURATE);
	luahelper_set_intfield(L,	"OPERATOR_MULTIPLY",		CAIRO_OPERATOR_MULTIPLY);
	luahelper_set_intfield(L,	"OPERATOR_SCREEN",			CAIRO_OPERATOR_SCREEN);
	luahelper_set_intfield(L,	"OPERATOR_OVERLAY",			CAIRO_OPERATOR_OVERLAY);
	luahelper_set_intfield(L,	"OPERATOR_DARKEN",			CAIRO_OPERATOR_DARKEN);
	luahelper_set_intfield(L,	"OPERATOR_LIGHTEN",			CAIRO_OPERATOR_LIGHTEN);
	luahelper_set_intfield(L,	"OPERATOR_COLOR_DODGE",		CAIRO_OPERATOR_COLOR_DODGE);
	luahelper_set_intfield(L,	"OPERATOR_COLOR_BURN",		CAIRO_OPERATOR_COLOR_BURN);
	luahelper_set_intfield(L,	"OPERATOR_HARD_LIGHT",		CAIRO_OPERATOR_HARD_LIGHT);
	luahelper_set_intfield(L,	"OPERATOR_SOFT_LIGHT",		CAIRO_OPERATOR_SOFT_LIGHT);
	luahelper_set_intfield(L,	"OPERATOR_DIFFERENCE",		CAIRO_OPERATOR_DIFFERENCE);
	luahelper_set_intfield(L,	"OPERATOR_EXCLUSION",		CAIRO_OPERATOR_EXCLUSION);
	luahelper_set_intfield(L,	"OPERATOR_HSL_HUE",			CAIRO_OPERATOR_HSL_HUE);
	luahelper_set_intfield(L,	"OPERATOR_HSL_SATURATION",	CAIRO_OPERATOR_HSL_SATURATION);
	luahelper_set_intfield(L,	"OPERATOR_HSL_COLOR",		CAIRO_OPERATOR_HSL_COLOR);
	luahelper_set_intfield(L,	"OPERATOR_HSL_LUMINOSITY",	CAIRO_OPERATOR_HSL_LUMINOSITY);

	luahelper_create_metatable(L, MT_NAME_CAIRO, m_cairo);
	luahelper_create_metatable(L, MT_NAME_CAIRO_SURFACE, m_cairo_surface);
	luahelper_create_metatable(L, MT_NAME_CAIRO_PATH, m_cairo_path);
	luahelper_create_metatable(L, MT_NAME_CAIRO_PATTERN, m_cairo_pattern);
	luahelper_create_metatable(L, MT_NAME_CAIRO_MATRIX, m_cairo_matrix);

	return 1;
}
