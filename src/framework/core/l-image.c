/*
 * framework/core/l-image.c
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
#include <framework/core/l-color.h>
#include <framework/core/l-matrix.h>
#include <framework/core/l-pattern.h>
#include <framework/core/l-image.h>

static int l_image_new(lua_State * L)
{
	struct surface_t * s = NULL;
	if((lua_gettop(L) == 2) && lua_isnumber(L, 1) && lua_isnumber(L, 2))
	{
		int width = luaL_checkinteger(L, 1);
		int height = luaL_checkinteger(L, 2);
		s = surface_alloc(width, height, NULL);
	}
	else
	{
		const char * filename = luaL_checkstring(L, 1);
		s = surface_alloc_from_xfs(((struct vmctx_t *)luahelper_vmctx(L))->xfs, filename);
	}
	if(s)
	{
		struct limage_t * image = lua_newuserdata(L, sizeof(struct limage_t));
		image->s = s;
		luaL_setmetatable(L, MT_IMAGE);
		return 1;
	}
	return 0;
}

static const luaL_Reg l_image[] = {
	{"new",	l_image_new},
	{NULL,	NULL}
};

static int m_image_gc(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_free(img->s);
	return 0;
}

static int m_image_tostring(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct surface_t * s = img->s;
	int width = surface_get_width(s);
	int height = surface_get_height(s);
	int stride = surface_get_stride(s);
	unsigned char * pixel = surface_get_pixels(s);
	lua_pushfstring(L, "image(%d,%d,%d,%p)", width, height, stride, pixel);
	return 1;
}

static int m_image_get_width(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	lua_pushnumber(L, surface_get_width(img->s));
	return 1;
}

static int m_image_get_height(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	lua_pushnumber(L, surface_get_height(img->s));
	return 1;
}

static int m_image_get_size(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	lua_pushnumber(L, surface_get_width(img->s));
	lua_pushnumber(L, surface_get_height(img->s));
	return 2;
}

static int m_image_clone(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct surface_t * c;
	struct region_t r;
	if(luaL_testudata(L, 2, MT_MATRIX))
	{
		struct matrix_t * m = lua_touserdata(L, 2);
		matrix_transform_region(m, surface_get_width(img->s), surface_get_height(img->s), &r);
		c = surface_alloc(r.w, r.h, NULL);
		if(!c)
			return 0;
		surface_blit(c, NULL, m, img->s, RENDER_TYPE_GOOD);
		struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
		subimg->s = c;
		luaL_setmetatable(L, MT_IMAGE);
	}
	else
	{
		r.x = luaL_optinteger(L, 2, 0);
		r.y = luaL_optinteger(L, 3, 0);
		r.w = luaL_optinteger(L, 4, surface_get_width(img->s));
		r.h = luaL_optinteger(L, 5, surface_get_height(img->s));
		if(r.w <= 0)
			r.w = surface_get_width(img->s);
		if(r.h <= 0)
			r.h = surface_get_height(img->s);
		c = surface_clone(img->s, &r);
		if(!c)
			return 0;
		struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
		subimg->s = c;
		luaL_setmetatable(L, MT_IMAGE);
	}
	return 1;
}

static int m_image_clear(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	struct region_t r;
	int x = luaL_optinteger(L, 3, 0);
	int y = luaL_optinteger(L, 4, 0);
	int w = luaL_optinteger(L, 5, surface_get_width(img->s) - x);
	int h = luaL_optinteger(L, 6, surface_get_height(img->s) - y);
	if(w <= 0)
		w = surface_get_width(img->s);
	if(h <= 0)
		h = surface_get_height(img->s);
	region_init(&r, x, y, w, h);
	surface_clear(img->s, c, &r);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_pixel(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	struct color_t * c = luaL_checkudata(L, 4, MT_COLOR);
	surface_set_pixel(img->s, x, y, c);
	lua_settop(L, 1);
	return 1;
}

static int m_image_get_pixel(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	struct color_t * c = lua_newuserdata(L, sizeof(struct color_t));
	surface_get_pixel(img->s, x, y, c);
	luaL_setmetatable(L, MT_COLOR);
	return 1;
}

static int m_image_haldclut(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct limage_t * clut = luaL_checkudata(L, 2, MT_IMAGE);
	const char * type = luaL_optstring(L, 3, "nearest");
	surface_filter_haldclut(img->s, clut->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_grayscale(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_filter_grayscale(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_sepia(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_filter_sepia(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_invert(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_filter_invert(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_threshold(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int threshold = luaL_optinteger(L, 2, 128);
	int value = luaL_optinteger(L, 3, 255);
	const char * type = luaL_optstring(L, 4, "binary");
	surface_filter_threshold(img->s, type, threshold, value);
	lua_settop(L, 1);
	return 1;
}

static int m_image_colorize(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	const char * type = luaL_optstring(L, 2, "parula");
	surface_filter_colorize(img->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_hue(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int angle = luaL_optinteger(L, 2, 0);
	surface_filter_hue(img->s, angle);
	lua_settop(L, 1);
	return 1;
}

static int m_image_saturate(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int saturate = luaL_optinteger(L, 2, 0);
	surface_filter_saturate(img->s, saturate);
	lua_settop(L, 1);
	return 1;
}

static int m_image_brightness(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int brightness = luaL_optinteger(L, 2, 0);
	surface_filter_brightness(img->s, brightness);
	lua_settop(L, 1);
	return 1;
}

static int m_image_contrast(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int contrast = luaL_optinteger(L, 2, 0);
	surface_filter_contrast(img->s, contrast);
	lua_settop(L, 1);
	return 1;
}

static int m_image_opacity(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int alpha = luaL_optinteger(L, 2, 100);
	surface_filter_opacity(img->s, alpha);
	lua_settop(L, 1);
	return 1;
}

static int m_image_blur(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int radius = luaL_optinteger(L, 2, 0);
	surface_filter_blur(img->s, radius);
	lua_settop(L, 1);
	return 1;
}

static int m_image_save(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_save(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_restore(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_restore(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_push_group(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_push_group(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_pop_group(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_pop_group(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_pop_group_to_source(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_pop_group_to_source(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_new_path(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_new_path(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_new_sub_path(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_new_sub_path(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_close_path(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_close_path(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_operator(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	const char * type = luaL_optstring(L, 2, "over");
	surface_shape_set_operator(img->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_source(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct lpattern_t * pattern = luaL_checkudata(L, 2, MT_PATTERN);
	surface_shape_set_source(img->s, pattern->pattern);
	lua_settop(L, 1);
	return 1;
}

static int m_image_get_source(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = surface_shape_get_source(img->s);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int m_image_set_source_color(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double r = luaL_optnumber(L, 2, 1);
	double g = luaL_optnumber(L, 3, 1);
	double b = luaL_optnumber(L, 4, 1);
	double a = luaL_optnumber(L, 5, 1);
	surface_shape_set_source_color(img->s, r, g, b, a);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_source_surface(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct limage_t * other = luaL_checkudata(L, 2, MT_IMAGE);
	double x = luaL_optnumber(L, 3, 0);
	double y = luaL_optnumber(L, 4, 0);
	surface_shape_set_source_surface(img->s, other->s, x, y);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_tolerance(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double tolerance = luaL_checknumber(L, 2);
	surface_shape_set_tolerance(img->s, tolerance);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_miter_limit(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double limit = luaL_checknumber(L, 2);
	surface_shape_set_miter_limit(img->s, limit);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_antialias(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	const char * type = luaL_optstring(L, 2, "default");
	surface_shape_set_antialias(img->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_fill_rule(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	const char * type = luaL_optstring(L, 2, "winding");
	surface_shape_set_fill_rule(img->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_line_width(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double width = luaL_checknumber(L, 2);
	surface_shape_set_line_width(img->s, width);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_line_cap(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	const char * type = luaL_optstring(L, 2, "butt");
	surface_shape_set_line_cap(img->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_line_join(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	const char * type = luaL_optstring(L, 2, "miter");
	surface_shape_set_line_join(img->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_dash(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
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
		surface_shape_set_dash(img->s, dashes, ndashes, offset);
		free(dashes);
	}
	lua_settop(L, 1);
	return 1;
}

static int m_image_identity(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_identity(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_translate(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double tx = luaL_optnumber(L, 2, 0);
	double ty = luaL_optnumber(L, 3, 0);
	surface_shape_translate(img->s, tx, ty);
	lua_settop(L, 1);
	return 1;
}

static int m_image_scale(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double sx = luaL_optnumber(L, 2, 0);
	double sy = luaL_optnumber(L, 3, 0);
	surface_shape_scale(img->s, sx, sy);
	lua_settop(L, 1);
	return 1;
}

static int m_image_rotate(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double angle = luaL_optnumber(L, 2, 0);
	surface_shape_rotate(img->s, angle);
	lua_settop(L, 1);
	return 1;
}

static int m_image_transform(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	surface_shape_transform(img->s, m);
	lua_settop(L, 1);
	return 1;
}

static int m_image_set_matrix(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	surface_shape_set_matrix(img->s, m);
	lua_settop(L, 1);
	return 1;
}

static int m_image_get_matrix(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = lua_newuserdata(L, sizeof(struct matrix_t));
	surface_shape_get_matrix(img->s, m);
	luaL_setmetatable(L, MT_MATRIX);
	return 1;
}

static int m_image_move_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	surface_shape_move_to(img->s, x, y);
	lua_settop(L, 1);
	return 1;
}

static int m_image_rel_move_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	surface_shape_rel_move_to(img->s, dx, dy);
	lua_settop(L, 1);
	return 1;
}

static int m_image_line_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	surface_shape_line_to(img->s, x, y);
	lua_settop(L, 1);
	return 1;
}

static int m_image_rel_line_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	surface_shape_rel_line_to(img->s, dx, dy);
	lua_settop(L, 1);
	return 1;
}

static int m_image_curve_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double x1 = luaL_checknumber(L, 2);
	double y1 = luaL_checknumber(L, 3);
	double x2 = luaL_checknumber(L, 4);
	double y2 = luaL_checknumber(L, 5);
	double x3 = luaL_checknumber(L, 6);
	double y3 = luaL_checknumber(L, 7);
	surface_shape_curve_to(img->s, x1, y1, x2, y2, x3, y3);
	lua_settop(L, 1);
	return 1;
}

static int m_image_rel_curve_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double dx1 = luaL_checknumber(L, 2);
	double dy1 = luaL_checknumber(L, 3);
	double dx2 = luaL_checknumber(L, 4);
	double dy2 = luaL_checknumber(L, 5);
	double dx3 = luaL_checknumber(L, 6);
	double dy3 = luaL_checknumber(L, 7);
	surface_shape_rel_curve_to(img->s, dx1, dy1, dx2, dy2, dx3, dy3);
	lua_settop(L, 1);
	return 1;
}

static int m_image_rectangle(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double w = luaL_checknumber(L, 4);
	double h = luaL_checknumber(L, 5);
	surface_shape_rectangle(img->s, x, y, w, h);
	lua_settop(L, 1);
	return 1;
}

static int m_image_rounded_rectangle(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	double w = luaL_checknumber(L, 4);
	double h = luaL_checknumber(L, 5);
	double r = luaL_optnumber(L, 6, 0);
	surface_shape_rounded_rectangle(img->s, x, y, w, h, r);
	lua_settop(L, 1);
	return 1;
}

static int m_image_arc(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double r = luaL_checknumber(L, 4);
	double a1 = luaL_checknumber(L, 5);
	double a2 = luaL_checknumber(L, 6);
	surface_shape_arc(img->s, xc, yc, r, a1, a2);
	lua_settop(L, 1);
	return 1;
}

static int m_image_arc_negative(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double xc = luaL_checknumber(L, 2);
	double yc = luaL_checknumber(L, 3);
	double r = luaL_checknumber(L, 4);
	double a1 = luaL_checknumber(L, 5);
	double a2 = luaL_checknumber(L, 6);
	surface_shape_arc_negative(img->s, xc, yc, r, a1, a2);
	lua_settop(L, 1);
	return 1;
}

static int m_image_stroke(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_stroke(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_stroke_preserve(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_stroke_preserve(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_fill(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_fill(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_fill_preserve(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_fill_preserve(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_reset_clip(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_reset_clip(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_clip(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_clip(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_clip_preserve(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_clip_preserve(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_mask(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct lpattern_t * pattern = luaL_checkudata(L, 2, MT_PATTERN);
	surface_shape_mask(img->s, pattern->pattern);
	lua_settop(L, 1);
	return 1;
}

static int m_image_mask_surface(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct limage_t * other = luaL_checkudata(L, 2, MT_IMAGE);
	double x = luaL_optnumber(L, 3, 0);
	double y = luaL_optnumber(L, 4, 0);
	surface_shape_mask_surface(img->s, other->s, x, y);
	lua_settop(L, 1);
	return 1;
}

static int m_image_paint(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double alpha = luaL_optnumber(L, 2, 1);
	surface_shape_paint(img->s, alpha);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_image[] = {
	{"__gc",				m_image_gc},
	{"__tostring",			m_image_tostring},
	{"getWidth",			m_image_get_width},
	{"getHeight",			m_image_get_height},
	{"getSize",				m_image_get_size},

	{"clone",				m_image_clone},

	{"clear",				m_image_clear},
	{"setPixel",			m_image_set_pixel},
	{"getPixel",			m_image_get_pixel},

	{"haldclut",			m_image_haldclut},
	{"grayscale",			m_image_grayscale},
	{"sepia",				m_image_sepia},
	{"invert",				m_image_invert},
	{"threshold",			m_image_threshold},
	{"colorize",			m_image_colorize},
	{"hue",					m_image_hue},
	{"saturate",			m_image_saturate},
	{"brightness",			m_image_brightness},
	{"contrast",			m_image_contrast},
	{"opacity",				m_image_opacity},
	{"blur",				m_image_blur},

	{"save",				m_image_save},
	{"restore",				m_image_restore},
	{"pushGroup",			m_image_push_group},
	{"popGroup",			m_image_pop_group},
	{"popGroupToSource",	m_image_pop_group_to_source},
	{"newPath",				m_image_new_path},
	{"newSubPath",			m_image_new_sub_path},
	{"closePath",			m_image_close_path},
	{"setOperator",			m_image_set_operator},
	{"setSource",			m_image_set_source},
	{"getSource",			m_image_get_source},
	{"setSourceColor",		m_image_set_source_color},
	{"setSourceSurface",	m_image_set_source_surface},
	{"setTolerance",		m_image_set_tolerance},
	{"setMiterLimit",		m_image_set_miter_limit},
	{"setAntialias",		m_image_set_antialias},
	{"setFillRule",			m_image_set_fill_rule},
	{"setLineWidth",		m_image_set_line_width},
	{"setLineCap",			m_image_set_line_cap},
	{"setLineJoin",			m_image_set_line_join},
	{"setDash",				m_image_set_dash},
	{"identity",			m_image_identity},
	{"translate",			m_image_translate},
	{"scale",				m_image_scale},
	{"rotate",				m_image_rotate},
	{"transform",			m_image_transform},
	{"setMatrix",			m_image_set_matrix},
	{"getMatrix",			m_image_get_matrix},
	{"moveTo",				m_image_move_to},
	{"relMoveTo",			m_image_rel_move_to},
	{"lineTo",				m_image_line_to},
	{"relLineTo",			m_image_rel_line_to},
	{"curveTo",				m_image_curve_to},
	{"relCurveTo",			m_image_rel_curve_to},
	{"rectangle",			m_image_rectangle},
	{"roundedRectangle",	m_image_rounded_rectangle},
	{"arc",					m_image_arc},
	{"arcNegative",			m_image_arc_negative},
	{"stroke",				m_image_stroke},
	{"strokePreserve",		m_image_stroke_preserve},
	{"fill",				m_image_fill},
	{"fillPreserve",		m_image_fill_preserve},
	{"resetClip",			m_image_reset_clip},
	{"clip",				m_image_clip},
	{"clipPreserve",		m_image_clip_preserve},
	{"mask",				m_image_mask},
	{"maskSurface",			m_image_mask_surface},
	{"paint",				m_image_paint},

	{NULL, NULL}
};

int luaopen_image(lua_State * L)
{
	luaL_newlib(L, l_image);
	luahelper_create_metatable(L, MT_IMAGE, m_image);
	return 1;
}
