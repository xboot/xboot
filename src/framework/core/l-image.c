/*
 * framework/core/l-image.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <core/l-color.h>
#include <core/l-matrix.h>
#include <core/l-text.h>
#include <core/l-icon.h>
#include <core/l-vision.h>
#include <core/l-image.h>

static int l_image_new(lua_State * L)
{
	struct surface_t * s = NULL;
	if(lua_gettop(L) == 2)
	{
		if((lua_type(L, 1) == LUA_TNUMBER) && (lua_type(L, 2) == LUA_TNUMBER))
		{
			int width = luaL_checkinteger(L, 1);
			int height = luaL_checkinteger(L, 2);
			s = surface_alloc(width, height, NULL);
		}
		else if((lua_type(L, 1) == LUA_TSTRING) && (lua_type(L, 2) == LUA_TNUMBER))
		{
			const char * txt = luaL_checkstring(L, 1);
			int pixsz = luaL_checkinteger(L, 2);
			s = surface_alloc_qrcode(txt, pixsz);
		}
	}
	else
	{
		if(lua_type(L, 1) == LUA_TSTRING)
		{
			const char * filename = luaL_checkstring(L, 1);
			s = surface_alloc_from_xfs(((struct vmctx_t *)luahelper_vmctx(L))->xfs, filename);
		}
		else if(luaL_testudata(L, 1, MT_VISION))
		{
			struct lvision_t * vision = lua_touserdata(L, 1);
			s = surface_alloc(vision_get_width(vision->v), vision_get_height(vision->v), NULL);
			if(s)
				surface_apply_vision(s, vision->v);
		}
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
	if(luaL_testudata(L, 2, MT_MATRIX))
	{
		struct matrix_t * m = lua_touserdata(L, 2);
		struct region_t r;
		matrix_transform_region(m, surface_get_width(img->s), surface_get_height(img->s), &r);
		c = surface_alloc(r.w, r.h, NULL);
		if(!c)
			return 0;
		surface_blit(c, NULL, m, img->s);
		struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
		subimg->s = c;
		luaL_setmetatable(L, MT_IMAGE);
	}
	else
	{
		int x = luaL_optinteger(L, 2, 0);
		int y = luaL_optinteger(L, 3, 0);
		int w = luaL_optinteger(L, 4, 0);
		int h = luaL_optinteger(L, 5, 0);
		c = surface_clone(img->s, x, y, w, h);
		if(!c)
			return 0;
		struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
		subimg->s = c;
		luaL_setmetatable(L, MT_IMAGE);
	}
	return 1;
}

static int m_image_extend(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int width = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	const char * type = luaL_optstring(L, 4, "repeat");
	if(width <= 0)
		width = surface_get_width(img->s);
	if(height <= 0)
		height = surface_get_height(img->s);
	struct surface_t * c = surface_extend(img->s, width, height, type);
	if(!c)
		return 0;
	struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
	subimg->s = c;
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
}

static int m_image_apply(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct lvision_t * vison = luaL_checkudata(L, 2, MT_VISION);
	surface_apply_vision(img->s, vison->v);
	lua_settop(L, 1);
	return 1;
}

static int m_image_clear(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct color_t * c = luaL_testudata(L, 2, MT_COLOR) ? lua_touserdata(L, 2) : &(struct color_t){0, 0, 0, 0};
	int x = luaL_optinteger(L, 3, 0);
	int y = luaL_optinteger(L, 4, 0);
	int w = luaL_optinteger(L, 5, 0);
	int h = luaL_optinteger(L, 6, 0);
	surface_clear(img->s, c, x, y, w, h);
	lua_settop(L, 1);
	return 1;
}

static int m_image_blit(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	struct limage_t * o = luaL_checkudata(L, 3, MT_IMAGE);
	surface_blit(img->s, NULL, m, o->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_fill(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	int w = luaL_checkinteger(L, 3);
	int h = luaL_checkinteger(L, 4);
	struct color_t * c = luaL_checkudata(L, 5, MT_COLOR);
	if((w > 0) && (h > 0))
		surface_fill(img->s, NULL, m, w, h, c);
	lua_settop(L, 1);
	return 1;
}

static int m_image_text(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	struct ltext_t * text = luaL_checkudata(L, 3, MT_TEXT);
	surface_text(img->s, NULL, m, &text->txt);
	lua_settop(L, 1);
	return 1;
}

static int m_image_icon(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	struct licon_t * icon = luaL_checkudata(L, 3, MT_ICON);
	surface_icon(img->s, NULL, m, &icon->ico);
	lua_settop(L, 1);
	return 1;
}

static int m_image_effect_glass(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_optinteger(L, 2, 0);
	int y = luaL_optinteger(L, 3, 0);
	int w = luaL_optinteger(L, 4, 0);
	int h = luaL_optinteger(L, 5, 0);
	int radius = luaL_optinteger(L, 6, 0);
	surface_effect_glass(img->s, NULL, x, y, w, h, radius);
	lua_settop(L, 1);
	return 1;
}

static int m_image_effect_shadow(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_optinteger(L, 2, 0);
	int y = luaL_optinteger(L, 3, 0);
	int w = luaL_optinteger(L, 4, 0);
	int h = luaL_optinteger(L, 5, 0);
	int radius = luaL_optinteger(L, 6, 0);
	struct color_t * c = luaL_testudata(L, 7, MT_COLOR) ? lua_touserdata(L, 7) : &(struct color_t){0, 0, 0, 255};
	surface_effect_shadow(img->s, NULL, x, y, w, h, radius, c);
	lua_settop(L, 1);
	return 1;
}

static int m_image_effect_gradient(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_checknumber(L, 2);
	int y = luaL_checknumber(L, 3);
	int w = luaL_checknumber(L, 4);
	int h = luaL_checknumber(L, 5);
	struct color_t * lt = luaL_checkudata(L, 6, MT_COLOR);
	struct color_t * rt = luaL_checkudata(L, 7, MT_COLOR);
	struct color_t * rb = luaL_checkudata(L, 8, MT_COLOR);
	struct color_t * lb = luaL_checkudata(L, 9, MT_COLOR);
	surface_effect_gradient(img->s, NULL, x, y, w, h, lt, rt, rb, lb);
	lua_settop(L, 1);
	return 1;
}

static int m_image_effect_checkerboard(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_optinteger(L, 2, 0);
	int y = luaL_optinteger(L, 3, 0);
	int w = luaL_optinteger(L, 4, 0);
	int h = luaL_optinteger(L, 5, 0);
	surface_effect_checkerboard(img->s, NULL, x, y, w, h);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_gray(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_filter_gray(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_sepia(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_filter_sepia(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_invert(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_filter_invert(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_coloring(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	surface_filter_coloring(img->s, c);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_hue(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int angle = luaL_optinteger(L, 2, 0);
	surface_filter_hue(img->s, angle);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_saturate(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int saturate = luaL_optinteger(L, 2, 0);
	surface_filter_saturate(img->s, saturate);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_brightness(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int brightness = luaL_optinteger(L, 2, 0);
	surface_filter_brightness(img->s, brightness);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_contrast(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int contrast = luaL_optinteger(L, 2, 0);
	surface_filter_contrast(img->s, contrast);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_opacity(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int alpha = luaL_optinteger(L, 2, 100);
	surface_filter_opacity(img->s, alpha);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_haldclut(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct limage_t * clut = luaL_checkudata(L, 2, MT_IMAGE);
	const char * type = luaL_optstring(L, 3, "nearest");
	surface_filter_haldclut(img->s, clut->s, type);
	lua_settop(L, 1);
	return 1;
}

static int m_image_filter_blur(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int radius = luaL_optinteger(L, 2, 0);
	surface_filter_blur(img->s, radius);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_save(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_save(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_restore(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_restore(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_set_source(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct limage_t * o = luaL_checkudata(L, 2, MT_IMAGE);
	double x = luaL_checknumber(L, 3);
	double y = luaL_checknumber(L, 4);
	surface_shape_set_source(img->s, o->s, x, y);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_set_source_color(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	surface_shape_set_source_color(img->s, c);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_set_line_width(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double w = luaL_checknumber(L, 2);
	surface_shape_set_line_width(img->s, w);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_set_matrix(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	surface_shape_set_matrix(img->s, m);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_new_path(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_new_path(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_close_path(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_close_path(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_move_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	surface_shape_move_to(img->s, x, y);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_line_to(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	surface_shape_line_to(img->s, x, y);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_curve_to(lua_State * L)
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

static int m_image_shape_rectangle(lua_State * L)
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

static int m_image_shape_arc(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double cx = luaL_checknumber(L, 2);
	double cy = luaL_checknumber(L, 3);
	double r = luaL_checknumber(L, 4);
	double a0 = luaL_checknumber(L, 5);
	double a1 = luaL_checknumber(L, 6);
	surface_shape_arc(img->s, cx, cy, r, a0, a1);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_arc_negative(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double cx = luaL_checknumber(L, 2);
	double cy = luaL_checknumber(L, 3);
	double r = luaL_checknumber(L, 4);
	double a0 = luaL_checknumber(L, 5);
	double a1 = luaL_checknumber(L, 6);
	surface_shape_arc_negative(img->s, cx, cy, r, a0, a1);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_circle(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double cx = luaL_checknumber(L, 2);
	double cy = luaL_checknumber(L, 3);
	double r = luaL_checknumber(L, 4);
	surface_shape_circle(img->s, cx, cy, r);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_ellipse(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	double cx = luaL_checknumber(L, 2);
	double cy = luaL_checknumber(L, 3);
	double rx = luaL_checknumber(L, 4);
	double ry = luaL_checknumber(L, 5);
	surface_shape_ellipse(img->s, cx, cy, rx, ry);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_clip(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_clip(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_clip_preserve(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_clip_preserve(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_fill(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_fill(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_fill_preserve(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_fill_preserve(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_stroke(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_stroke(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_stroke_preserve(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_stroke_preserve(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_shape_paint(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_shape_paint(img->s);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_image[] = {
	{"__gc",					m_image_gc},
	{"__tostring",				m_image_tostring},
	{"getWidth",				m_image_get_width},
	{"getHeight",				m_image_get_height},
	{"getSize",					m_image_get_size},

	{"clone",					m_image_clone},
	{"extend",					m_image_extend},
	{"apply",					m_image_apply},
	{"clear",					m_image_clear},

	{"blit",					m_image_blit},
	{"fill",					m_image_fill},
	{"text",					m_image_text},
	{"icon",					m_image_icon},

	{"glass",					m_image_effect_glass},
	{"shadow",					m_image_effect_shadow},
	{"gradient",				m_image_effect_gradient},
	{"checkerboard",			m_image_effect_checkerboard},

	{"gray",					m_image_filter_gray},
	{"sepia",					m_image_filter_sepia},
	{"invert",					m_image_filter_invert},
	{"coloring",				m_image_filter_coloring},
	{"hue",						m_image_filter_hue},
	{"saturate",				m_image_filter_saturate},
	{"brightness",				m_image_filter_brightness},
	{"contrast",				m_image_filter_contrast},
	{"opacity",					m_image_filter_opacity},
	{"haldclut",				m_image_filter_haldclut},
	{"blur",					m_image_filter_blur},

	{"shapeSave",				m_image_shape_save},
	{"shapeRestore",			m_image_shape_restore},
	{"shapeSetSource",			m_image_shape_set_source},
	{"shapeSetSourceColor",		m_image_shape_set_source_color},
	{"shapeSetLineWidth",		m_image_shape_set_line_width},
	{"shapeSetMatrix",			m_image_shape_set_matrix},
	{"shapeNewPath",			m_image_shape_new_path},
	{"shapeClosePath",			m_image_shape_close_path},
	{"shapeMoveTo",				m_image_shape_move_to},
	{"shapeLineTo",				m_image_shape_line_to},
	{"shapeCurveTo",			m_image_shape_curve_to},
	{"shapeRectangle",			m_image_shape_rectangle},
	{"shapeArc",				m_image_shape_arc},
	{"shapeArcNegative",		m_image_shape_arc_negative},
	{"shapeCircle",				m_image_shape_circle},
	{"shapeEllipse",			m_image_shape_ellipse},
	{"shapeClip",				m_image_shape_clip},
	{"shapeClipPreserve",		m_image_shape_clip_preserve},
	{"shapeFill",				m_image_shape_fill},
	{"shapeFillPreserve",		m_image_shape_fill_preserve},
	{"shapeStroke",				m_image_shape_stroke},
	{"shapeStrokePreserve",		m_image_shape_stroke_preserve},
	{"shapePaint",				m_image_shape_paint},

	{NULL, NULL}
};

int luaopen_image(lua_State * L)
{
	luaL_newlib(L, l_image);
	luahelper_create_metatable(L, MT_IMAGE, m_image);
	return 1;
}
