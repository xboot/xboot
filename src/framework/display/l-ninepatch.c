/*
 * framework/display/l-ninepatch.c
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
#include <cairoint.h>
#include <xfs/xfs.h>
#include <framework/display/l-display.h>

static cairo_status_t xfs_read_func(void * closure, unsigned char * data, unsigned int size)
{
	struct xfs_file_t * file = closure;
	size_t ret;

    while(size)
    {
    	ret = xfs_read(file, data, 1, size);
    	size -= ret;
    	data += ret;
    	if(size && xfs_eof(file))
    		return _cairo_error(CAIRO_STATUS_READ_ERROR);
    }
    return CAIRO_STATUS_SUCCESS;
}

static cairo_surface_t * cairo_image_surface_create_from_png_xfs(const char * filename)
{
	struct xfs_file_t * file;
	cairo_surface_t * surface;

	file = xfs_open_read(filename);
	if(!file)
		return _cairo_surface_create_in_error(_cairo_error(CAIRO_STATUS_FILE_NOT_FOUND));
	surface = cairo_image_surface_create_from_png_stream(xfs_read_func, file);
	xfs_close(file);
    return surface;
}

static inline int is_black_pixel(unsigned char * p)
{
	return (((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] != 0)) ? 1 : 0);
}

static bool_t to_ninepatch(cairo_surface_t * surface, struct ninepatch_t * patch)
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

	/* Nine patch size */
	patch->width = width - 2;
	patch->height = height - 2;

	/* Nine patch chunk */
	cs = cairo_surface_create_similar_image(surface, CAIRO_FORMAT_ARGB32, width, height);
	cr = cairo_create(cs);
	cairo_set_source_surface(cr, surface, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);

	data = cairo_image_surface_get_data(cs);
	stride = cairo_image_surface_get_stride(cs);

	/* Top stretch row line */
	patch->stretch.a = 1;
	patch->stretch.b = width - 2;
	for(i = 1; i < width - 1; i++)
	{
		if(is_black_pixel(&data[stride * 0 + i * 4]))
		{
			patch->stretch.a = i;
			break;
		}
	}
	for(i = width - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * 0 + i * 4]))
		{
			patch->stretch.b = i;
			break;
		}
	}

	/* Left stretch column line */
	patch->stretch.c = 1;
	patch->stretch.d = height - 2;
	for(i = 1; i < height - 1; i++)
	{
		if(is_black_pixel(&data[stride * i + 0 * 4]))
		{
			patch->stretch.c = i;
			break;
		}
	}
	for(i = height - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * i + 0 * 4]))
		{
			patch->stretch.d = i;
			break;
		}
	}
	cairo_surface_destroy(cs);

	patch->left = patch->stretch.a - 1;
	patch->right = width - 2 - patch->stretch.b;
	patch->top = patch->stretch.c - 1;
	patch->bottom = height - 2 - patch->stretch.d;

	/* Left top */
	w = patch->stretch.a - 1;
	h = patch->stretch.c - 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
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
	w = patch->stretch.b - patch->stretch.a + 1;
	h = patch->stretch.c - 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->stretch.a, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->mt = cs;
	}
	else
	{
		patch->mt = NULL;
	}

	/* Right top */
	w = width -2 - patch->stretch.b;
	h = patch->stretch.c - 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->stretch.b - 1, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->rt = cs;
	}
	else
	{
		patch->rt = NULL;
	}

	/* Left Middle */
	w = patch->stretch.a - 1;
	h = patch->stretch.d - patch->stretch.c + 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -1, -patch->stretch.c);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->lm = cs;
	}
	else
	{
		patch->lm = NULL;
	}

	/* Middle Middle */
	w = patch->stretch.b - patch->stretch.a + 1;
	h = patch->stretch.d - patch->stretch.c + 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->stretch.a, -patch->stretch.c);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->mm = cs;
	}
	else
	{
		patch->mm = NULL;
	}

	/* Right middle */
	w = width -2 - patch->stretch.b;
	h = patch->stretch.d - patch->stretch.c + 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->stretch.b - 1, -patch->stretch.c);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->rm = cs;
	}
	else
	{
		patch->rm = NULL;
	}

	/* Left bottom */
	w = patch->stretch.a - 1;
	h = height - 2 - patch->stretch.d;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -1, -patch->stretch.d - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->lb = cs;
	}
	else
	{
		patch->lb = NULL;
	}

	/* Middle bottom */
	w = patch->stretch.b - patch->stretch.a + 1;
	h = height - 2 - patch->stretch.d;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->stretch.a, -patch->stretch.d - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->mb = cs;
	}
	else
	{
		patch->mb = NULL;
	}

	/* Right bottom */
	w = width -2 - patch->stretch.b;
	h = height - 2 - patch->stretch.d;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(surface, cairo_surface_get_content(surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, surface, -patch->stretch.b - 1, -patch->stretch.d - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		patch->rb = cs;
	}
	else
	{
		patch->rb = NULL;
	}

	return TRUE;
}

static int l_ninepatch_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	struct ninepatch_t * ninepatch = lua_newuserdata(L, sizeof(struct ninepatch_t));
	cairo_surface_t * surface = cairo_image_surface_create_from_png_xfs(filename);
	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
		return 0;
	bool_t result = to_ninepatch(surface, ninepatch);
	cairo_surface_destroy(surface);
	if(!result)
		return 0;
	luaL_setmetatable(L, MT_NAME_NINEPATCH);
	return 1;
}

static const luaL_Reg l_ninepatch[] = {
	{"new",	l_ninepatch_new},
	{NULL,	NULL}
};

static int m_ninepatch_gc(lua_State * L)
{
	struct ninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NAME_NINEPATCH);
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
	struct ninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NAME_NINEPATCH);
	double w = luaL_checknumber(L, 2);
	double h = luaL_checknumber(L, 3);
	ninepatch->width = w;
	ninepatch->height = h;
	return 0;
}

static int m_ninepatch_get_size(lua_State * L)
{
	struct ninepatch_t * ninepatch = luaL_checkudata(L, 1, MT_NAME_NINEPATCH);
	lua_pushnumber(L, ninepatch->width);
	lua_pushnumber(L, ninepatch->height);
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
	luahelper_create_metatable(L, MT_NAME_NINEPATCH, m_ninepatch);
	return 1;
}
