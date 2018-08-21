/*
 * framework/display/l-shape.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
	luaL_setmetatable(L, MT_SHAPE);
	return 1;
}

static const luaL_Reg l_shape[] = {
	{"new",	l_shape_new},
	{NULL,	NULL}
};

static int m_shape_gc(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_destroy(*cr);
	return 0;
}

static int m_shape_save(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_save(*cr);
	return 0;
}

static int m_shape_restore(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_restore(*cr);
	return 0;
}

static int m_shape_push_group(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_push_group(*cr);
	return 0;
}

static int m_shape_pop_group(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_pop_group(*cr);
	return 0;
}

static int m_shape_pop_group_to_source(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_pop_group_to_source(*cr);
	return 0;
}

static int m_shape_new_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_new_path(*cr);
	return 0;
}

static int m_shape_new_sub_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_new_sub_path(*cr);
	return 0;
}

static int m_shape_close_path(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_close_path(*cr);
	return 0;
}

static int m_shape_set_operator(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_operator_t op = (cairo_operator_t)luaL_checkinteger(L, 2);
	cairo_set_operator(*cr, op);
	return 0;
}

static int m_shape_set_source(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	struct lpattern_t * pattern = luaL_checkudata(L, 2, MT_PATTERN);
	cairo_set_source(*cr, pattern->pattern);
	return 0;
}

static int m_shape_set_source_color(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double red = luaL_checknumber(L, 2);
	double green = luaL_checknumber(L, 3);
	double blue = luaL_checknumber(L, 4);
	double alpha = luaL_optnumber(L, 5, 1);
	cairo_set_source_rgba(*cr, red, green, blue, alpha);
	return 0;
}

static int m_shape_set_tolerance(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double tolerance = luaL_checknumber(L, 2);
	cairo_set_tolerance(*cr, tolerance);
	return 0;
}

static int m_shape_set_miter_limit(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double limit = luaL_checknumber(L, 2);
	cairo_set_miter_limit(*cr, limit);
	return 0;
}

static int m_shape_set_antialias(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_antialias_t antialias = (cairo_antialias_t)luaL_checkinteger(L, 2);
	cairo_set_antialias(*cr, antialias);
	return 0;
}

static int m_shape_set_fill_rule(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_fill_rule_t rule = (cairo_fill_rule_t)luaL_checkinteger(L, 2);
	cairo_set_fill_rule(*cr, rule);
	return 0;
}

static int m_shape_set_line_width(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double width = luaL_checknumber(L, 2);
	cairo_set_line_width(*cr, width);
	return 0;
}

static int m_shape_set_line_cap(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_line_cap_t cap = (cairo_line_cap_t)luaL_checkinteger(L, 2);
	cairo_set_line_cap(*cr, cap);
	return 0;
}

static int m_shape_set_line_join(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_line_join_t join = (cairo_line_join_t)luaL_checkinteger(L, 2);
	cairo_set_line_join(*cr, join);
	return 0;
}

static int m_shape_set_dash(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double offset = luaL_checknumber(L, 3);
	double * dashes;
	int ndashes, i;
	luaL_checktype(L, 2, LUA_TTABLE);
	ndashes = (int)lua_rawlen(L, 2);
	if(ndashes > 0)
	{
		dashes = malloc(ndashes * sizeof(double));
		for(i = 0; i < ndashes; i++)
		{
			lua_rawgeti(L, 2, i + 1);
			dashes[i] = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		cairo_set_dash(*cr, dashes, ndashes, offset);
		free(dashes);
	}
	return 0;
}

static int m_shape_move_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_move_to(*cr, x, y);
	return 0;
}

static int m_shape_rel_move_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_move_to(*cr, dx, dy);
	return 0;
}

static int m_shape_line_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_line_to(*cr, x, y);
	return 0;
}

static int m_shape_rel_line_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_line_to(*cr, dx, dy);
	return 0;
}

static int m_shape_curve_to(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
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
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double dx1 = luaL_checknumber(L, 2);
	double dy1 = luaL_checknumber(L, 3);
	double dx2 = luaL_checknumber(L, 4);
	double dy2 = luaL_checknumber(L, 5);
	double dx3 = luaL_checknumber(L, 6);
	double dy3 = luaL_checknumber(L, 7);
	cairo_rel_curve_to(*cr, dx1, dy1, dx2, dy2, dx3, dy3);
	return 0;
}

static int m_shape_rectangle(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);
	cairo_rectangle(*cr, x, y, width, height);
	return 0;
}

static int m_shape_arc(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
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
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5);
	double angle2 = luaL_checknumber(L, 6);
	cairo_arc_negative(*cr, xc, yc, radius, angle1, angle2);
	return 0;
}

static int m_shape_stroke(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_stroke(*cr);
	return 0;
}

static int m_shape_stroke_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_stroke_preserve(*cr);
	return 0;
}

static int m_shape_fill(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_fill(*cr);
	return 0;
}

static int m_shape_fill_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_fill_preserve(*cr);
	return 0;
}

static int m_shape_clip(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_clip(*cr);
	return 0;
}

static int m_shape_clip_preserve(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_clip_preserve(*cr);
	return 0;
}

static int m_shape_paint(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
	double alpha = luaL_optnumber(L, 2, 1.0);
	cairo_paint_with_alpha(*cr, alpha);
	return 0;
}

static int m_shape_size(lua_State * L)
{
	cairo_t ** cr = luaL_checkudata(L, 1, MT_SHAPE);
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
	{"save",				m_shape_save},
	{"restore",				m_shape_restore},
	{"pushGroup",			m_shape_push_group},
	{"popGroup",			m_shape_pop_group},
	{"popGroupToSource",	m_shape_pop_group_to_source},
	{"newPath",				m_shape_new_path},
	{"newSubPath",			m_shape_new_sub_path},
	{"closePath",			m_shape_close_path},
	{"setOperator",			m_shape_set_operator},
	{"setSource",			m_shape_set_source},
	{"setSourceColor",		m_shape_set_source_color},
	{"setTolerance",		m_shape_set_tolerance},
	{"setMiterLimit",		m_shape_set_miter_limit},
	{"setAntialias",		m_shape_set_antialias},
	{"setFillRule",			m_shape_set_fill_rule},
	{"setLineWidth",		m_shape_set_line_width},
	{"setLineCap",			m_shape_set_line_cap},
	{"setLineJoin",			m_shape_set_line_join},
	{"setDash",				m_shape_set_dash},
	{"moveTo",				m_shape_move_to},
	{"relMoveTo",			m_shape_rel_move_to},
	{"lineTo",				m_shape_line_to},
	{"relLineTo",			m_shape_rel_line_to},
	{"curveTo",				m_shape_curve_to},
	{"relCurveTo",			m_shape_rel_curve_to},
	{"rectangle",			m_shape_rectangle},
	{"arc",					m_shape_arc},
	{"arcNegative",			m_shape_arc_negative},
	{"stroke",				m_shape_stroke},
	{"strokePreserve",		m_shape_stroke_preserve},
	{"fill",				m_shape_fill},
	{"fillPreserve",		m_shape_fill_preserve},
	{"clip",				m_shape_clip},
	{"clipPreserve",		m_shape_clip_preserve},
	{"paint",				m_shape_paint},
	{"size",				m_shape_size},
	{NULL,					NULL}
};

int luaopen_shape(lua_State * L)
{
	luaL_newlib(L, l_shape);
	/* cairo_operator_t */
	luahelper_set_intfield(L, "CAIRO_OPERATOR_CLEAR",			CAIRO_OPERATOR_CLEAR);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_SOURCE",			CAIRO_OPERATOR_SOURCE);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_OVER",			CAIRO_OPERATOR_OVER);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_IN",				CAIRO_OPERATOR_IN);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_OUT",				CAIRO_OPERATOR_OUT);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_ATOP",			CAIRO_OPERATOR_ATOP);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_DEST",			CAIRO_OPERATOR_DEST);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_DEST_OVER",		CAIRO_OPERATOR_DEST_OVER);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_DEST_IN",			CAIRO_OPERATOR_DEST_IN);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_DEST_OUT",		CAIRO_OPERATOR_DEST_OUT);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_DEST_ATOP",		CAIRO_OPERATOR_DEST_ATOP);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_XOR",				CAIRO_OPERATOR_XOR);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_ADD",				CAIRO_OPERATOR_ADD);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_SATURATE",		CAIRO_OPERATOR_SATURATE);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_MULTIPLY",		CAIRO_OPERATOR_MULTIPLY);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_SCREEN",			CAIRO_OPERATOR_SCREEN);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_OVERLAY",			CAIRO_OPERATOR_OVERLAY);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_DARKEN",			CAIRO_OPERATOR_DARKEN);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_LIGHTEN",			CAIRO_OPERATOR_LIGHTEN);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_COLOR_DODGE",		CAIRO_OPERATOR_COLOR_DODGE);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_COLOR_BURN",		CAIRO_OPERATOR_COLOR_BURN);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_HARD_LIGHT", 		CAIRO_OPERATOR_HARD_LIGHT);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_SOFT_LIGHT",		CAIRO_OPERATOR_SOFT_LIGHT);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_DIFFERENCE",		CAIRO_OPERATOR_DIFFERENCE);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_EXCLUSION", 		CAIRO_OPERATOR_EXCLUSION);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_HSL_HUE",			CAIRO_OPERATOR_HSL_HUE);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_HSL_SATURATION",	CAIRO_OPERATOR_HSL_SATURATION);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_HSL_COLOR",		CAIRO_OPERATOR_HSL_COLOR);
	luahelper_set_intfield(L, "CAIRO_OPERATOR_HSL_LUMINOSITY",	CAIRO_OPERATOR_HSL_LUMINOSITY);
	/* cairo_antialias_t */
	luahelper_set_intfield(L, "ANTIALIAS_DEFAULT",				CAIRO_ANTIALIAS_DEFAULT);
	luahelper_set_intfield(L, "ANTIALIAS_NONE",					CAIRO_ANTIALIAS_NONE);
	luahelper_set_intfield(L, "ANTIALIAS_GRAY",					CAIRO_ANTIALIAS_GRAY);
	luahelper_set_intfield(L, "ANTIALIAS_SUBPIXEL",				CAIRO_ANTIALIAS_SUBPIXEL);
	luahelper_set_intfield(L, "ANTIALIAS_FAST",					CAIRO_ANTIALIAS_FAST);
	luahelper_set_intfield(L, "ANTIALIAS_GOOD",					CAIRO_ANTIALIAS_GOOD);
	luahelper_set_intfield(L, "ANTIALIAS_BEST",					CAIRO_ANTIALIAS_BEST);
    /* cairo_fill_rule_t */
	luahelper_set_intfield(L, "FILL_RULE_WINDING",				CAIRO_FILL_RULE_WINDING);
	luahelper_set_intfield(L, "FILL_RULE_EVEN_ODD",				CAIRO_FILL_RULE_EVEN_ODD);
	/* cairo_line_cap_t */
	luahelper_set_intfield(L, "LINE_CAP_BUTT",					CAIRO_LINE_CAP_BUTT);
	luahelper_set_intfield(L, "LINE_CAP_ROUND",					CAIRO_LINE_CAP_ROUND);
	luahelper_set_intfield(L, "LINE_CAP_SQUARE",				CAIRO_LINE_CAP_SQUARE);
	/* cairo_line_join_t */
	luahelper_set_intfield(L, "LINE_JOIN_MITER", 				CAIRO_LINE_JOIN_MITER);
	luahelper_set_intfield(L, "LINE_JOIN_ROUND", 				CAIRO_LINE_JOIN_ROUND);
	luahelper_set_intfield(L, "LINE_JOIN_BEVEL", 				CAIRO_LINE_JOIN_BEVEL);
	luahelper_create_metatable(L, MT_SHAPE, m_shape);
	return 1;
}
