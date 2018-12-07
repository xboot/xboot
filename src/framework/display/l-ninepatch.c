/*
 * framework/display/l-ninepatch.c
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
#include <cairoint.h>
#include <xfs/xfs.h>
#include <framework/display/l-display.h>

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
	struct xfs_context_t * ctx = luahelper_task(L)->__xfs_ctx;
	struct xfs_file_t * file;
	cairo_surface_t * surface;

	file = xfs_open_read(ctx, filename);
	if(!file)
		return _cairo_surface_create_in_error(_cairo_error(CAIRO_STATUS_FILE_NOT_FOUND));
	surface = cairo_image_surface_create_from_png_stream(xfs_read_func, file);
	xfs_close(file);
    return surface;
}

static inline int detect_black_pixel(unsigned char * p)
{
	return (((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] != 0)) ? 1 : 0);
}

static inline void ninepatch_stretch(struct lninepatch_t * ninepatch, double width, double height)
{
	int lr = ninepatch->left + ninepatch->right;
	int tb = ninepatch->top + ninepatch->bottom;

	if(width < ninepatch->width)
		width = ninepatch->width;
	if(height < ninepatch->height)
		height = ninepatch->height;
	ninepatch->__w = width;
	ninepatch->__h = height;
	ninepatch->__sx = (ninepatch->__w - lr) / (ninepatch->width - lr);
	ninepatch->__sy = (ninepatch->__h - tb) / (ninepatch->height - tb);
}

static bool_t to_ninepatch(cairo_surface_t * surface, struct lninepatch_t * patch)
{
	cairo_surface_t * cs;
	cairo_t * cr;
	unsigned char * data;
	int width, height;
	int stride;
	int w, h;
	int i;

	if(!surface || !patch)
		return FALSE;

	width = cairo_image_surface_get_width(surface);
	height = cairo_image_surface_get_height(surface);
	if(width < 3 || height < 3)
		return FALSE;

	/* Nine patch chunk */
	cs = cairo_surface_create_similar_image(surface, CAIRO_FORMAT_ARGB32, width, height);
	cr = cairo_create(cs);
	cairo_set_source_surface(cr, surface, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	data = cairo_image_surface_get_data(cs);
	stride = cairo_image_surface_get_stride(cs);

	/* Nine patch default size */
	width = width - 2;
	height = height - 2;
	patch->width = width;
	patch->height = height;

	/* Stretch information */
	patch->left = 0;
	patch->right = 0;
	patch->top = 0;
	patch->right = 0;

	for(i = 0; i < width; i++)
	{
		if(detect_black_pixel(&data[(i + 1) * 4]))
		{
			patch->left = i;
			break;
		}
	}
	for(i = width - 1; i >= 0; i--)
	{
		if(detect_black_pixel(&data[(i + 1) * 4]))
		{
			patch->right = width - 1 - i;
			break;
		}
	}
	for(i = 0; i < height; i++)
	{
		if(detect_black_pixel(&data[stride * (i + 1)]))
		{
			patch->top = i;
			break;
		}
	}
	for(i = height - 1; i >= 0; i--)
	{
		if(detect_black_pixel(&data[stride * (i + 1)]))
		{
			patch->bottom = height - 1 - i;
			break;
		}
	}
	cairo_surface_destroy(cs);

	/* Left top */
	w = patch->left;
	h = patch->top;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), patch->left, patch->top);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -1, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->lt = cs;
	}
	else
	{
		patch->lt = NULL;
	}

	/* Middle top */
	w = width - patch->left - patch->right;
	h = patch->top;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->left - 1, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->mt = cs;
	}
	else
	{
		patch->mt = NULL;
	}

	/* Right top */
	w = patch->right;
	h = patch->top;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -(width - patch->right) - 1, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->rt = cs;
	}
	else
	{
		patch->rt = NULL;
	}

	/* Left Middle */
	w = patch->left;
	h = height - patch->top - patch->bottom;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -1, -patch->top - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->lm = cs;
	}
	else
	{
		patch->lm = NULL;
	}

	/* Middle Middle */
	w = width - patch->left - patch->right;
	h = height - patch->top - patch->bottom;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->left - 1, -patch->top - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->mm = cs;
	}
	else
	{
		patch->mm = NULL;
	}

	/* Right middle */
	w = patch->right;
	h = height - patch->top - patch->bottom;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -(width - patch->right) - 1, -patch->top - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->rm = cs;
	}
	else
	{
		patch->rm = NULL;
	}

	/* Left bottom */
	w = patch->left;
	h = patch->bottom;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -1, -(height - patch->bottom) - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->lb = cs;
	}
	else
	{
		patch->lb = NULL;
	}

	/* Middle bottom */
	w = width - patch->left - patch->right;
	h = patch->bottom;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->left - 1, -(height - patch->bottom) - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->mb = cs;
	}
	else
	{
		patch->mb = NULL;
	}

	/* Right bottom */
	w = patch->right;
	h = patch->bottom;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -(width - patch->right) - 1, -(height - patch->bottom) - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->rb = cs;
	}
	else
	{
		patch->rb = NULL;
	}

	ninepatch_stretch(patch, width, height);
	return TRUE;
}

static int l_ninepatch_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	struct lninepatch_t * ninepatch = lua_newuserdata(L, sizeof(struct lninepatch_t));
	cairo_surface_t * surface = cairo_image_surface_create_from_png_xfs(L, filename);
	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
		return 0;
	bool_t result = to_ninepatch(surface, ninepatch);
	cairo_surface_destroy(surface);
	if(!result)
		return 0;
	luaL_setmetatable(L, MT_NINEPATCH);
	return 1;
}

static const luaL_Reg l_ninepatch[] = {
	{"new",	l_ninepatch_new},
	{NULL,	NULL}
};

static int m_ninepatch_gc(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	if(ninepatch->lt)
		cairo_surface_destroy(ninepatch->lt);
	if(ninepatch->mt)
		cairo_surface_destroy(ninepatch->mt);
	if(ninepatch->rt)
		cairo_surface_destroy(ninepatch->rt);
	if(ninepatch->lm)
		cairo_surface_destroy(ninepatch->lm);
	if(ninepatch->mm)
		cairo_surface_destroy(ninepatch->mm);
	if(ninepatch->rm)
		cairo_surface_destroy(ninepatch->rm);
	if(ninepatch->lb)
		cairo_surface_destroy(ninepatch->lb);
	if(ninepatch->mb)
		cairo_surface_destroy(ninepatch->mb);
	if(ninepatch->rb)
		cairo_surface_destroy(ninepatch->rb);
	return 0;
}

static int m_ninepatch_set_size(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	double w = luaL_checknumber(L, 2);
	double h = luaL_checknumber(L, 3);
	ninepatch_stretch(ninepatch, w, h);
	return 0;
}

static int m_ninepatch_get_size(lua_State * L)
{
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NINEPATCH);
	lua_pushnumber(L, ninepatch->__w);
	lua_pushnumber(L, ninepatch->__h);
	return 2;
}

static const luaL_Reg m_ninepatch[] = {
	{"__gc",		m_ninepatch_gc},
	{"setSize",		m_ninepatch_set_size},
	{"getSize",		m_ninepatch_get_size},
	{NULL,			NULL}
};

int luaopen_ninepatch(lua_State * L)
{
	luaL_newlib(L, l_ninepatch);
	luahelper_create_metatable(L, MT_NINEPATCH, m_ninepatch);
	return 1;
}
