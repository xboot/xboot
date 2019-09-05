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

static int m_image_extend(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int width = luaL_checkinteger(L, 2);
	int height = luaL_checkinteger(L, 3);
	const char * type = luaL_optstring(L, 3, "repeat");
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

static int m_image_blit(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	struct limage_t * o = luaL_checkudata(L, 3, MT_IMAGE);
	surface_blit(img->s, NULL, m, o->s, RENDER_TYPE_GOOD);
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
		surface_fill(img->s, NULL, m, w, h, c, RENDER_TYPE_GOOD);
	lua_settop(L, 1);
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
	int x = luaL_checknumber(L, 2);
	int y = luaL_checknumber(L, 3);
	struct color_t * c = luaL_checkudata(L, 4, MT_COLOR);
	surface_set_pixel(img->s, x, y, c);
	lua_settop(L, 1);
	return 1;
}

static int m_image_get_pixel(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_checknumber(L, 2);
	int y = luaL_checknumber(L, 3);
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

static const luaL_Reg m_image[] = {
	{"__gc",		m_image_gc},
	{"__tostring",	m_image_tostring},
	{"getWidth",	m_image_get_width},
	{"getHeight",	m_image_get_height},
	{"getSize",		m_image_get_size},

	{"clone",		m_image_clone},
	{"extend",		m_image_extend},

	{"blit",		m_image_blit},
	{"fill",		m_image_fill},
	{"clear",		m_image_clear},
	{"setPixel",	m_image_set_pixel},
	{"getPixel",	m_image_get_pixel},

	{"haldclut",	m_image_haldclut},
	{"grayscale",	m_image_grayscale},
	{"sepia",		m_image_sepia},
	{"invert",		m_image_invert},
	{"threshold",	m_image_threshold},
	{"colorize",	m_image_colorize},
	{"hue",			m_image_hue},
	{"saturate",	m_image_saturate},
	{"brightness",	m_image_brightness},
	{"contrast",	m_image_contrast},
	{"opacity",		m_image_opacity},
	{"blur",		m_image_blur},

	{NULL, NULL}
};

int luaopen_image(lua_State * L)
{
	luaL_newlib(L, l_image);
	luahelper_create_metatable(L, MT_IMAGE, m_image);
	return 1;
}
