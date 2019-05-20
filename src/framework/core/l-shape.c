/*
 * framework/core/l-shape.c
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
#include <framework/core/l-pattern.h>
#include <framework/core/l-image.h>
#include <framework/core/l-shape.h>

static int l_shape_new(lua_State * L)
{
	int width = luaL_checkinteger(L, 1);
	int height = luaL_checkinteger(L, 2);
	struct lshape_t * shape = lua_newuserdata(L, sizeof(struct lshape_t));
	shape->cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	shape->cr = cairo_create(shape->cs);
	luaL_setmetatable(L, MT_SHAPE);
	return 1;
}

static const luaL_Reg l_shape[] = {
	{"new",	l_shape_new},
	{NULL,	NULL}
};

static int m_shape_gc(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_destroy(shape->cr);
	cairo_surface_destroy(shape->cs);
	return 0;
}

static int m_shape_get_width(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	lua_pushnumber(L, cairo_image_surface_get_width(shape->cs));
	return 1;
}

static int m_shape_get_height(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	lua_pushnumber(L, cairo_image_surface_get_height(shape->cs));
	return 1;
}

static int m_shape_get_size(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	lua_pushnumber(L, cairo_image_surface_get_width(shape->cs));
	lua_pushnumber(L, cairo_image_surface_get_height(shape->cs));
	return 2;
}

static int m_shape_save(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_save(shape->cr);
	return 0;
}

static int m_shape_restore(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_restore(shape->cr);
	return 0;
}

static int m_shape_push_group(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_push_group(shape->cr);
	return 0;
}

static int m_shape_pop_group(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_pop_group(shape->cr);
	return 0;
}

static int m_shape_pop_group_to_source(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_pop_group_to_source(shape->cr);
	return 0;
}

static int m_shape_new_path(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_new_path(shape->cr);
	return 0;
}

static int m_shape_new_sub_path(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_new_sub_path(shape->cr);
	return 0;
}

static int m_shape_close_path(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_close_path(shape->cr);
	return 0;
}

static int m_shape_set_operator(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	const char * type = luaL_optstring(L, 2, "over");
	switch(shash(type))
	{
	case 0x0f3b6d8c: /* "clear" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_CLEAR);
		break;
	case 0x1c3aff76: /* "source" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_SOURCE);
		break;
	case 0x7c9bf101: /* "over" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_OVER);
		break;
	case 0x0059783c: /* "in" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_IN);
		break;
	case 0x0b889a9d: /* "out" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_OUT);
		break;
	case 0x7c943c79: /* "atop" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_ATOP);
		break;
	case 0x7c95a255: /* "dest" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_DEST);
		break;
	case 0x4f1e8f9e: /* "dest-over" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_DEST_OVER);
		break;
	case 0x0979cb99: /* "dest-in" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_DEST_IN);
		break;
	case 0x38b3599a: /* "dest-out" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_DEST_OUT);
		break;
	case 0x4f16db16: /* "dest-atop" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_DEST_ATOP);
		break;
	case 0x0b88c01e: /* "xor" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_XOR);
		break;
	case 0x0b885cce: /* "add" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_ADD);
		break;
	case 0xdf32bb4e: /* "saturate" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_SATURATE);
		break;
	case 0x46746f05: /* "multiply" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_MULTIPLY);
		break;
	case 0x1b5ffd45: /* "screen" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_SCREEN);
		break;
	case 0x7ee4b5c7: /* "overlay" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_OVERLAY);
		break;
	case 0xf83e845a: /* "darken" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_DARKEN);
		break;
	case 0x79c1c710: /* "lighten" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_LIGHTEN);
		break;
	case 0x8b9b2114: /* "color-dodge" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_COLOR_DODGE);
		break;
	case 0x61514968: /* "color-burn" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_COLOR_BURN);
		break;
	case 0xb5060029: /* "hard-light" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_HARD_LIGHT);
		break;
	case 0xfcd63c26: /* "soft-light" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_SOFT_LIGHT);
		break;
	case 0x52a92470: /* "difference" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_DIFFERENCE);
		break;
	case 0x6f499bff: /* "exclusion" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_EXCLUSION);
		break;
	case 0x5d66597b: /* "hsl-hue" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_HSL_HUE);
		break;
	case 0x1f46f0c3: /* "hsl-saturation" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_HSL_SATURATION);
		break;
	case 0x50050bf8: /* "hsl-color" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_HSL_COLOR);
		break;
	case 0x02d64a96: /* "hsl-luminosity" */
		cairo_set_operator(shape->cr, CAIRO_OPERATOR_HSL_LUMINOSITY);
		break;
	default:
		break;
	}
	return 0;
}

static int m_shape_set_source(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	struct lpattern_t * pattern = luaL_checkudata(L, 2, MT_PATTERN);
	cairo_set_source(shape->cr, pattern->pattern);
	return 0;
}

static int m_shape_set_source_color(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double red = luaL_checknumber(L, 2);
	double green = luaL_checknumber(L, 3);
	double blue = luaL_checknumber(L, 4);
	double alpha = luaL_optnumber(L, 5, 1);
	cairo_set_source_rgba(shape->cr, red, green, blue, alpha);
	return 0;
}

static int m_shape_set_tolerance(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double tolerance = luaL_checknumber(L, 2);
	cairo_set_tolerance(shape->cr, tolerance);
	return 0;
}

static int m_shape_set_miter_limit(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double limit = luaL_checknumber(L, 2);
	cairo_set_miter_limit(shape->cr, limit);
	return 0;
}

static int m_shape_set_antialias(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	const char * type = luaL_optstring(L, 2, "default");
	switch(shash(type))
	{
	case 0x0885548a: /* "default" */
		cairo_set_antialias(shape->cr, CAIRO_ANTIALIAS_DEFAULT);
		break;
	case 0x7c9b47f5: /* "none" */
		cairo_set_antialias(shape->cr, CAIRO_ANTIALIAS_NONE);
		break;
	case 0x7c977c78: /* "gray" */
		cairo_set_antialias(shape->cr, CAIRO_ANTIALIAS_GRAY);
		break;
	case 0xb8639511: /* "subpixel" */
		cairo_set_antialias(shape->cr, CAIRO_ANTIALIAS_SUBPIXEL);
		break;
	case 0x7c96aa13: /* "fast" */
		cairo_set_antialias(shape->cr, CAIRO_ANTIALIAS_FAST);
		break;
	case 0x7c97716e: /* "good" */
		cairo_set_antialias(shape->cr, CAIRO_ANTIALIAS_GOOD);
		break;
	case 0x7c948993: /* "best" */
		cairo_set_antialias(shape->cr, CAIRO_ANTIALIAS_BEST);
		break;
	default:
		break;
	}
	return 0;
}

static int m_shape_set_fill_rule(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	const char * type = luaL_optstring(L, 2, "winding");
	switch(shash(type))
	{
	case 0xc8feafb5: /* "winding" */
		cairo_set_fill_rule(shape->cr, CAIRO_FILL_RULE_WINDING);
		break;
	case 0x207e6937: /* "even-odd" */
		cairo_set_fill_rule(shape->cr, CAIRO_FILL_RULE_EVEN_ODD);
		break;
	default:
		break;
	}
	return 0;
}

static int m_shape_set_line_width(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double width = luaL_checknumber(L, 2);
	cairo_set_line_width(shape->cr, width);
	return 0;
}

static int m_shape_set_line_cap(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	const char * type = luaL_optstring(L, 2, "butt");
	switch(shash(type))
	{
	case 0x7c94cdc4: /* "butt" */
		cairo_set_line_cap(shape->cr, CAIRO_LINE_CAP_BUTT);
		break;
	case 0x104cc7ed: /* "round" */
		cairo_set_line_cap(shape->cr, CAIRO_LINE_CAP_ROUND);
		break;
	case 0x1c5eea16: /* "square" */
		cairo_set_line_cap(shape->cr, CAIRO_LINE_CAP_SQUARE);
		break;
	default:
		break;
	}
	return 0;
}

static int m_shape_set_line_join(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	const char * type = luaL_optstring(L, 2, "miter");
	switch(shash(type))
	{
	case 0x0feefdc6: /* "miter" */
		cairo_set_line_join(shape->cr, CAIRO_LINE_JOIN_MITER);
		break;
	case 0x104cc7ed: /* "round" */
		cairo_set_line_join(shape->cr, CAIRO_LINE_JOIN_ROUND);
		break;
	case 0x0f25c733: /* "bevel" */
		cairo_set_line_join(shape->cr, CAIRO_LINE_JOIN_BEVEL);
		break;
	default:
		break;
	}
	return 0;
}

static int m_shape_set_dash(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
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
		cairo_set_dash(shape->cr, dashes, ndashes, offset);
		free(dashes);
	}
	return 0;
}

static int m_shape_move_to(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_move_to(shape->cr, x, y);
	return 0;
}

static int m_shape_rel_move_to(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_move_to(shape->cr, dx, dy);
	return 0;
}

static int m_shape_line_to(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_line_to(shape->cr, x, y);
	return 0;
}

static int m_shape_rel_line_to(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	cairo_rel_line_to(shape->cr, dx, dy);
	return 0;
}

static int m_shape_curve_to(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double x1 = luaL_checknumber(L, 2);
	double y1 = luaL_checknumber(L, 3);
	double x2 = luaL_checknumber(L, 4);
	double y2 = luaL_checknumber(L, 5);
	double x3 = luaL_checknumber(L, 6);
	double y3 = luaL_checknumber(L, 7);
	cairo_curve_to(shape->cr, x1, y1, x2, y2, x3, y3);
	return 0;
}

static int m_shape_rel_curve_to(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double dx1 = luaL_checknumber(L, 2);
	double dy1 = luaL_checknumber(L, 3);
	double dx2 = luaL_checknumber(L, 4);
	double dy2 = luaL_checknumber(L, 5);
	double dx3 = luaL_checknumber(L, 6);
	double dy3 = luaL_checknumber(L, 7);
	cairo_rel_curve_to(shape->cr, dx1, dy1, dx2, dy2, dx3, dy3);
	return 0;
}

static int m_shape_rectangle(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);
	cairo_rectangle(shape->cr, x, y, width, height);
	return 0;
}

static int m_shape_rounded_rectangle(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_t * cr = shape->cr;
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double width = luaL_checknumber(L, 4);
	double height = luaL_checknumber(L, 5);
	double radius = luaL_optnumber(L, 6, 0);
	cairo_move_to(cr, x + radius, y);
	cairo_line_to(cr, x + width - radius, y);
	cairo_arc(cr, x + width - radius, y + radius, radius, - M_PI / 2, 0);
	cairo_line_to(cr, x + width, y + height - radius);
	cairo_arc(cr, x + width - radius, y + height - radius, radius, 0, M_PI / 2);
	cairo_line_to(cr, x + radius, y + height);
	cairo_arc(cr, x + radius, y + height - radius, radius, M_PI / 2, M_PI);
	cairo_line_to(cr, x, y + radius);
	cairo_arc(cr, x + radius, y + radius, radius, M_PI, M_PI + M_PI / 2);
	return 0;
}

static int m_shape_arc(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5);
	double angle2 = luaL_checknumber(L, 6);
	cairo_arc(shape->cr, xc, yc, radius, angle1, angle2);
	return 0;
}

static int m_shape_arc_negative(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double radius = luaL_checknumber(L, 4);
	double angle1 = luaL_checknumber(L, 5);
	double angle2 = luaL_checknumber(L, 6);
	cairo_arc_negative(shape->cr, xc, yc, radius, angle1, angle2);
	return 0;
}

static int m_shape_stroke(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_stroke(shape->cr);
	return 0;
}

static int m_shape_stroke_preserve(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_stroke_preserve(shape->cr);
	return 0;
}

static int m_shape_fill(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_fill(shape->cr);
	return 0;
}

static int m_shape_fill_preserve(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_fill_preserve(shape->cr);
	return 0;
}

static int m_shape_clip(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_clip(shape->cr);
	return 0;
}

static int m_shape_clip_preserve(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	cairo_clip_preserve(shape->cr);
	return 0;
}

static int m_shape_mask(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	struct lpattern_t * pattern = luaL_checkudata(L, 2, MT_PATTERN);
	cairo_mask(shape->cr, pattern->pattern);
	return 0;
}

static int m_shape_paint(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	double alpha = luaL_optnumber(L, 2, 1.0);
	cairo_paint_with_alpha(shape->cr, alpha);
	return 0;
}

static int m_shape_snapshot(lua_State * L)
{
	struct lshape_t * shape = luaL_checkudata(L, 1, MT_SHAPE);
	struct limage_t * img = lua_newuserdata(L, sizeof(struct limage_t));
	int w = cairo_image_surface_get_width(shape->cs);
	int h = cairo_image_surface_get_height(shape->cs);
	img->cs = cairo_surface_create_similar(shape->cs, cairo_surface_get_content(shape->cs), w, h);
	cairo_t * cr = cairo_create(img->cs);
	cairo_set_source_surface(cr, shape->cs, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
}

static const luaL_Reg m_shape[] = {
	{"__gc",				m_shape_gc},
	{"getWidth",			m_shape_get_width},
	{"getHeight",			m_shape_get_height},
	{"getSize",				m_shape_get_size},
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
	{"roundedRectangle",	m_shape_rounded_rectangle},
	{"arc",					m_shape_arc},
	{"arcNegative",			m_shape_arc_negative},
	{"stroke",				m_shape_stroke},
	{"strokePreserve",		m_shape_stroke_preserve},
	{"fill",				m_shape_fill},
	{"fillPreserve",		m_shape_fill_preserve},
	{"clip",				m_shape_clip},
	{"clipPreserve",		m_shape_clip_preserve},
	{"mask",				m_shape_mask},
	{"paint",				m_shape_paint},
	{"snapshot",			m_shape_snapshot},
	{NULL,					NULL}
};

int luaopen_shape(lua_State * L)
{
	luaL_newlib(L, l_shape);
	luahelper_create_metatable(L, MT_SHAPE, m_shape);
	return 1;
}
