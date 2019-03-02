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
#include <framework/core/l-image.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

#ifndef CLIP3
#define CLIP3(x, min, max)	((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#endif

static cairo_status_t xfs_read_func(void * closure, unsigned char * data, unsigned int size)
{
	struct xfs_file_t * file = closure;
	size_t len = 0, n;

	while(size > 0)
	{
		n = xfs_read(file, data, size);
		if(n <= 0)
			break;
		size -= n;
		len += n;
		data += n;
	}
	if(len > 0)
		return CAIRO_STATUS_SUCCESS;
	return _cairo_error(CAIRO_STATUS_READ_ERROR);
}

static cairo_surface_t * cairo_image_surface_create_from_png_xfs(lua_State * L, const char * filename)
{
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
	struct xfs_file_t * file;
	cairo_surface_t * cs;

	file = xfs_open_read(ctx, filename);
	if(!file)
		return _cairo_surface_create_in_error(_cairo_error(CAIRO_STATUS_FILE_NOT_FOUND));
	cs = cairo_image_surface_create_from_png_stream(xfs_read_func, file);
	xfs_close(file);
	return cs;
}

static int l_image_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	struct limage_t * image = lua_newuserdata(L, sizeof(struct limage_t));
	image->cs = cairo_image_surface_create_from_png_xfs(L, filename);
	if(cairo_surface_status(image->cs) != CAIRO_STATUS_SUCCESS)
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
	cairo_surface_destroy(img->cs);
	return 0;
}

static int m_image_clone(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int x = luaL_optinteger(L, 2, 0);
	int y = luaL_optinteger(L, 3, 0);
	int w = luaL_optinteger(L, 4, cairo_image_surface_get_width(img->cs));
	int h = luaL_optinteger(L, 5, cairo_image_surface_get_height(img->cs));
	struct limage_t * subimg = lua_newuserdata(L, sizeof(struct limage_t));
	subimg->cs = cairo_surface_create_similar(img->cs, cairo_surface_get_content(img->cs), w, h);
	cairo_t * cr = cairo_create(subimg->cs);
	cairo_set_source_surface(cr, img->cs, -x, -y);
	cairo_paint(cr);
	cairo_destroy(cr);
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
}

static int m_image_grayscale(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	cairo_surface_t * cs = img->cs;
	int width = cairo_image_surface_get_width(cs);
	int height = cairo_image_surface_get_height(cs);
	int stride = cairo_image_surface_get_stride(cs);
	cairo_format_t format = cairo_image_surface_get_format(cs);
	unsigned char * p, * q = cairo_image_surface_get_data(cs);
	unsigned char gray;
	int r, g, b;
	int x, y;
	switch(format)
	{
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				gray = (r * 19595 + g * 38469 + b * 7472) >> 16;
				p[0] = gray;
				p[1] = gray;
				p[2] = gray;
			}
		}
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A8:
	case CAIRO_FORMAT_A1:
	case CAIRO_FORMAT_RGB16_565:
	case CAIRO_FORMAT_RGB30:
	default:
		break;
	}
	lua_settop(L, 1);
	return 1;
}

static int m_image_sepia(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	cairo_surface_t * cs = img->cs;
	int width = cairo_image_surface_get_width(cs);
	int height = cairo_image_surface_get_height(cs);
	int stride = cairo_image_surface_get_stride(cs);
	cairo_format_t format = cairo_image_surface_get_format(cs);
	unsigned char * p, * q = cairo_image_surface_get_data(cs);
	int r, g, b;
	int tr, tg, tb;
	int x, y;
	switch(format)
	{
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = (r * 17826 + g * 34996 + b * 8585) >> 16;
				tg = (r * 22872 + g * 44958 + b * 11010) >> 16;
				tr = (r * 25756 + g * 50397 + b * 12386) >> 16;
				p[0] = MIN(tb, 255);
				p[1] = MIN(tg, 255);
				p[2] = MIN(tr, 255);
			}
		}
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A8:
	case CAIRO_FORMAT_A1:
	case CAIRO_FORMAT_RGB16_565:
	case CAIRO_FORMAT_RGB30:
	default:
		break;
	}
	lua_settop(L, 1);
	return 1;
}

static int m_image_invert(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	cairo_surface_t * cs = img->cs;
	int width = cairo_image_surface_get_width(cs);
	int height = cairo_image_surface_get_height(cs);
	int stride = cairo_image_surface_get_stride(cs);
	cairo_format_t format = cairo_image_surface_get_format(cs);
	unsigned char * p, * q = cairo_image_surface_get_data(cs);
	int x, y;
	switch(format)
	{
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				p[0] = 255 - p[0];
				p[1] = 255 - p[1];
				p[2] = 255 - p[2];
			}
		}
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A8:
	case CAIRO_FORMAT_A1:
	case CAIRO_FORMAT_RGB16_565:
	case CAIRO_FORMAT_RGB30:
	default:
		break;
	}
	lua_settop(L, 1);
	return 1;
}

static int m_image_saturate(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int saturate = luaL_optinteger(L, 2, 0);
	int k = CLIP3(saturate, -100, 100) / 100.0 * 128.0;
	cairo_surface_t * cs = img->cs;
	int width = cairo_image_surface_get_width(cs);
	int height = cairo_image_surface_get_height(cs);
	int stride = cairo_image_surface_get_stride(cs);
	cairo_format_t format = cairo_image_surface_get_format(cs);
	unsigned char * p, * q = cairo_image_surface_get_data(cs);
	int r, g, b, min, max;
	int alpha, delta, value, l, s;
	int x, y;
	switch(format)
	{
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				min = MIN(MIN(r, g), b);
				max = MAX(MAX(r, g), b);
				delta = max - min;
				value = max + min;
				if(delta == 0)
					continue;
				l = value >> 1;
				s = l < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
				if(k >= 0)
				{
					alpha = (k + s >= 128) ? s : 128 - k;
					alpha = 128 * 128 / alpha - 128;
				}
				else
				{
					alpha = k;
				}
				r = r + ((r - l) * alpha >> 7);
				g = g + ((g - l) * alpha >> 7);
				b = b + ((b - l) * alpha >> 7);
				p[0] = CLIP3(b, 0, 255);
				p[1] = CLIP3(g, 0, 255);
				p[2] = CLIP3(r, 0, 255);
			}
		}
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A8:
	case CAIRO_FORMAT_A1:
	case CAIRO_FORMAT_RGB16_565:
	case CAIRO_FORMAT_RGB30:
	default:
		break;
	}
	lua_settop(L, 1);
	return 1;
}

static int m_image_brightness(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int brightness = luaL_optinteger(L, 2, 0);
	int delta = CLIP3(brightness, -100, 100) / 100.0 * 255.0;
	cairo_surface_t * cs = img->cs;
	int width = cairo_image_surface_get_width(cs);
	int height = cairo_image_surface_get_height(cs);
	int stride = cairo_image_surface_get_stride(cs);
	cairo_format_t format = cairo_image_surface_get_format(cs);
	unsigned char * p, * q = cairo_image_surface_get_data(cs);
	int r, g, b;
	int tr, tg, tb;
	int x, y;
	switch(format)
	{
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = b + delta;
				tg = g + delta;
				tr = r + delta;
				p[0] = CLIP3(tb, 0, 255);
				p[1] = CLIP3(tg, 0, 255);
				p[2] = CLIP3(tr, 0, 255);
			}
		}
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A8:
	case CAIRO_FORMAT_A1:
	case CAIRO_FORMAT_RGB16_565:
	case CAIRO_FORMAT_RGB30:
	default:
		break;
	}
	lua_settop(L, 1);
	return 1;
}

static int m_image_contrast(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int contrast = luaL_optinteger(L, 2, 0);
	int k = CLIP3(contrast, -100, 100) / 100.0 * 128.0;
	cairo_surface_t * cs = img->cs;
	int width = cairo_image_surface_get_width(cs);
	int height = cairo_image_surface_get_height(cs);
	int stride = cairo_image_surface_get_stride(cs);
	cairo_format_t format = cairo_image_surface_get_format(cs);
	unsigned char * p, * q = cairo_image_surface_get_data(cs);
	int r, g, b;
	int tr, tg, tb;
	int x, y;
	switch(format)
	{
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
		for(y = 0; y < height; y++, q += stride)
		{
			for(x = 0, p = q; x < width; x++, p += 4)
			{
				b = p[0];
				g = p[1];
				r = p[2];
				tb = (b << 7) + (b - 128) * k;
				tg = (g << 7) + (g - 128) * k;
				tr = (r << 7) + (r - 128) * k;
				p[0] = CLIP3(tb, 0, 255 << 7) >> 7;
				p[1] = CLIP3(tg, 0, 255 << 7) >> 7;
				p[2] = CLIP3(tr, 0, 255 << 7) >> 7;
			}
		}
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A8:
	case CAIRO_FORMAT_A1:
	case CAIRO_FORMAT_RGB16_565:
	case CAIRO_FORMAT_RGB30:
	default:
		break;
	}
	lua_settop(L, 1);
	return 1;
}

static int m_image_get_size(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int w = cairo_image_surface_get_width(img->cs);
	int h = cairo_image_surface_get_height(img->cs);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

static inline void blurinner(unsigned char * pixel, int * zr, int * zg, int * zb, int * za, int alpha)
{
	unsigned char a;
	int r, g, b;

	r = *pixel;
	g = *(pixel + 1);
	b = *(pixel + 2);
	a = *(pixel + 3);

	*zr += (alpha * ((r << 7) - *zr)) >> 16;
	*zg += (alpha * ((g << 7) - *zg)) >> 16;
	*zb += (alpha * ((b << 7) - *zb)) >> 16;
	*za += (alpha * ((a << 7) - *za)) >> 16;

	*pixel = *zr >> 7;
	*(pixel + 1) = *zg >> 7;
	*(pixel + 2) = *zb >> 7;
	*(pixel + 3) = *za >> 7;
}

static inline void blurrow(unsigned char * pixel, int width, int height, int channel, int line, int alpha)
{
	unsigned char * scanline = &(pixel[line * width * channel]);
	int zr, zg, zb, za;
	int i;

	zr = *scanline << 7;
	zg = *(scanline + 1) << 7;
	zb = *(scanline + 2) << 7;
	za = *(scanline + 3) << 7;

	for(i = 0; i < width; i++)
		blurinner(&scanline[i * channel], &zr, &zg, &zb, &za, alpha);
	for(i = width - 2; i >= 0; i--)
		blurinner(&scanline[i * channel], &zr, &zg, &zb, &za, alpha);
}

static inline void blurcol(unsigned char * pixel, int width, int height, int channel, int x, int alpha)
{
	unsigned char * ptr = pixel;
	int zr, zg, zb, za;
	int i;

	ptr += x * channel;
	zr = *((unsigned char *)ptr) << 7;
	zg = *((unsigned char *)ptr + 1) << 7;
	zb = *((unsigned char *)ptr + 2) << 7;
	za = *((unsigned char *)ptr + 3) << 7;

	for(i = width; i < (height - 1) * width; i += width)
		blurinner((unsigned char *)&ptr[i * channel], &zr, &zg, &zb, &za, alpha);
	for(i = (height - 2) * width; i >= 0; i -= width)
		blurinner((unsigned char *)&ptr[i * channel], &zr, &zg, &zb, &za, alpha);
}

static void expblur(unsigned char * pixel, int width, int height, int channel, int radius)
{
	int row, col;
	int alpha;

	if(radius >= 1)
	{
		alpha = (int)((1 << 16) * (1.0 - expf(-2.3 / (radius + 1.0))));
		for(row = 0; row < height; row++)
			blurrow(pixel, width, height, channel, row, alpha);
		for(col = 0; col < width; col++)
			blurcol(pixel, width, height, channel, col, alpha);
	}
}

static int m_image_blur(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int radius = luaL_optinteger(L, 2, 0);
	cairo_surface_t * cs = img->cs;
	int width = cairo_image_surface_get_width(cs);
	int height = cairo_image_surface_get_height(cs);
	cairo_format_t format = cairo_image_surface_get_format(cs);
	unsigned char * pixel = cairo_image_surface_get_data(cs);

	switch(format)
	{
	case CAIRO_FORMAT_ARGB32:
		expblur(pixel, width, height, 4, radius);
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_RGB24:
		expblur(pixel, width, height, 3, radius);
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A8:
		expblur(pixel, width, height, 1, radius);
		cairo_surface_mark_dirty(cs);
		break;
	case CAIRO_FORMAT_A1:
	case CAIRO_FORMAT_RGB16_565:
	case CAIRO_FORMAT_RGB30:
	default:
		break;
	}
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_image[] = {
	{"__gc",		m_image_gc},
	{"clone",		m_image_clone},
	{"grayscale",	m_image_grayscale},
	{"sepia",		m_image_sepia},
	{"invert",		m_image_invert},
	{"saturate",	m_image_saturate},
	{"brightness",	m_image_brightness},
	{"contrast",	m_image_contrast},
	{"blur",		m_image_blur},
	{"getSize",		m_image_get_size},
	{NULL,			NULL}
};

int luaopen_image(lua_State * L)
{
	luaL_newlib(L, l_image);
	luahelper_create_metatable(L, MT_IMAGE, m_image);
	return 1;
}
