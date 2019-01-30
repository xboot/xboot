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

static int m_image_get_size(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	int w = cairo_image_surface_get_width(img->cs);
	int h = cairo_image_surface_get_height(img->cs);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

static int m_image_region(lua_State * L)
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

static const luaL_Reg m_image[] = {
	{"__gc",		m_image_gc},
	{"getSize",		m_image_get_size},
	{"region",		m_image_region},
	{NULL,			NULL}
};

int luaopen_image(lua_State * L)
{
	luaL_newlib(L, l_image);
	luahelper_create_metatable(L, MT_IMAGE, m_image);
	return 1;
}
