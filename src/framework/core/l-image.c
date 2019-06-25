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
#include <framework/core/l-matrix.h>
#include <framework/core/l-image.h>

static int l_image_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	struct limage_t * image = lua_newuserdata(L, sizeof(struct limage_t));
	image->s = surface_alloc_from_xfs(((struct vmctx_t *)luahelper_vmctx(L))->xfs, filename);
	if(!image->s)
		return 0;
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
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
/*	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	if(luaL_testudata(L, 2, MT_MATRIX))
	{
		struct matrix_t * m = lua_touserdata(L, 2);
		struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
		double x1 = 0;
		double y1 = 0;
		double x2 = cairo_image_surface_get_width(img->s);
		double y2 = cairo_image_surface_get_height(img->s);
		matrix_transform_bounds(m, &x1, &y1, &x2, &y2);
		subimg->s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, x2 - x1, y2 - y1);
		cairo_t * cr = cairo_create(subimg->s);
		cairo_set_matrix(cr, (cairo_matrix_t *)m);
		cairo_set_source_surface(cr, img->s, 0, 0);
		cairo_paint(cr);
		cairo_destroy(cr);
		luaL_setmetatable(L, MT_IMAGE);
	}
	else
	{
		int x = luaL_optinteger(L, 2, 0);
		int y = luaL_optinteger(L, 3, 0);
		int w = luaL_optinteger(L, 4, cairo_image_surface_get_width(img->s));
		int h = luaL_optinteger(L, 5, cairo_image_surface_get_height(img->s));
		int r = luaL_optinteger(L, 6, 0);
		struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
		subimg->s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
		cairo_t * cr = cairo_create(subimg->s);
		if(r > 0)
		{
			cairo_move_to(cr, r, 0);
			cairo_line_to(cr, w - r, 0);
			cairo_arc(cr, w - r, r, r, - M_PI / 2, 0);
			cairo_line_to(cr, w, h - r);
			cairo_arc(cr, w - r, h - r, r, 0, M_PI / 2);
			cairo_line_to(cr, r, h);
			cairo_arc(cr, r, h - r, r, M_PI / 2, M_PI);
			cairo_line_to(cr, 0, r);
			cairo_arc(cr, r, r, r, M_PI, M_PI + M_PI / 2);
			cairo_clip(cr);
		}
		cairo_set_source_surface(cr, img->s, -x, -y);
		cairo_paint(cr);
		cairo_destroy(cr);
		luaL_setmetatable(L, MT_IMAGE);
	}*/
	return 1;
}

static int m_image_clear(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	surface_clear(img->s);
	lua_settop(L, 1);
	return 1;
}

static int m_image_paste(lua_State * L)
{
/*	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct limage_t * other = luaL_checkudata(L, 2, MT_IMAGE);
	cairo_t * cr = cairo_create(img->s);
	cairo_set_source_surface(cr, other->s, 0, 0);
	if(luaL_testudata(L, 3, MT_IMAGE))
		cairo_mask_surface(cr, ((struct limage_t *)lua_touserdata(L, 3))->s, 0, 0);
	else
		cairo_paint(cr);
	cairo_destroy(cr);
	lua_settop(L, 1);*/
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

static int m_image_gamma(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	float gamma = luaL_optnumber(L, 2, 1);
	surface_filter_gamma(img->s, gamma);
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

static int m_image_blur(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int radius = luaL_optinteger(L, 2, 0);
	surface_filter_blur(img->s, radius);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_image[] = {
	{"__gc",		m_image_gc},
	{"__tostring",	m_image_tostring},
	{"getWidth",	m_image_get_width},
	{"getHeight",	m_image_get_height},
	{"getSize",		m_image_get_size},
	{"clone",		m_image_clone},
	{"clear",		m_image_clear},
	{"paste",		m_image_paste},
	{"grayscale",	m_image_grayscale},
	{"sepia",		m_image_sepia},
	{"invert",		m_image_invert},
	{"threshold",	m_image_threshold},
	{"colorize",	m_image_colorize},
	{"gamma",		m_image_gamma},
	{"hue",			m_image_hue},
	{"saturate",	m_image_saturate},
	{"brightness",	m_image_brightness},
	{"contrast",	m_image_contrast},
	{"blur",		m_image_blur},
	{NULL,			NULL}
};

int luaopen_image(lua_State * L)
{
	luaL_newlib(L, l_image);
	luahelper_create_metatable(L, MT_IMAGE, m_image);
	return 1;
}
