/*
 * framework/display/l-texture.c
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

struct nine_patch_t {
	struct {
		int a, b, c, d;
	} stretch;

	struct {
		int a, b, c, d;
	} content;

	cairo_surface_t * lt;
	cairo_surface_t * mt;
	cairo_surface_t * rt;
	cairo_surface_t * lm;
	cairo_surface_t * mm;
	cairo_surface_t * rm;
	cairo_surface_t * lb;
	cairo_surface_t * mb;
	cairo_surface_t * rb;
};

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

static bool_t is_match_extension(const char * filename, const char * ext)
{
	int pos, len;

	pos = strlen(filename);
	len = strlen(ext);

	if ((!pos) || (!len) || (len > pos))
		return FALSE;

	pos -= len;
	return (strcasecmp(filename + pos, ext) == 0);
}

static inline int is_black_pixel(unsigned char * p)
{
	return (((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] != 0)) ? 1 : 0);
}

static bool_t to_nine_patch(cairo_surface_t * surface, struct nine_patch_t * patch)
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

	/* Bottom content row line */
	patch->content.a = 1;
	patch->content.b = width - 2;
	for(i = 1; i < width - 1; i++)
	{
		if(is_black_pixel(&data[stride * (height - 1) + i * 4]))
		{
			patch->content.a = i;
			break;
		}
	}
	for(i = width - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * (height - 1) + i * 4]))
		{
			patch->content.b = i;
			break;
		}
	}

	/* Right content column line */
	patch->content.c = 1;
	patch->content.d = height - 2;
	for(i = 1; i < height - 1; i++)
	{
		if(is_black_pixel(&data[stride * i + (width - 1) * 4]))
		{
			patch->content.c = i;
			break;
		}
	}
	for(i = height - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * i + (width - 1) * 4]))
		{
			patch->content.d = i;
			break;
		}
	}
	cairo_surface_destroy(cs);

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

static int l_texture_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	if(is_match_extension(filename, ".9.png"))
	{
		cairo_surface_t * surface = cairo_image_surface_create_from_png_xfs(filename);
		if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
			return 0;
		struct nine_patch_t patch;
		bool_t flag = to_nine_patch(surface, &patch);
		cairo_surface_destroy(surface);
		if(flag)
		{
			struct ltexture_t * texture;
			lua_newtable(L);
			if(patch.lt)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.lt;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "lt");
			}
			if(patch.mt)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.mt;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "mt");
			}
			if(patch.rt)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.rt;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "rt");
			}
			if(patch.lm)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.lm;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "lm");
			}
			if(patch.mm)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.mm;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "mm");
			}
			if(patch.rm)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.rm;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "rm");
			}
			if(patch.lb)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.lb;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "lb");
			}
			if(patch.mb)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.mb;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "mb");
			}
			if(patch.rb)
			{
				texture = lua_newuserdata(L, sizeof(struct ltexture_t));
				texture->surface = patch.rb;
				luaL_setmetatable(L, MT_NAME_TEXTURE);
				lua_setfield(L, -2, "rb");
			}
			return 1;
		}
	}
	else if(is_match_extension(filename, ".png"))
	{
		struct ltexture_t * texture = lua_newuserdata(L, sizeof(struct ltexture_t));
		texture->surface = cairo_image_surface_create_from_png_xfs(filename);
		if(cairo_surface_status(texture->surface) != CAIRO_STATUS_SUCCESS)
			return 0;
		luaL_setmetatable(L, MT_NAME_TEXTURE);
		return 1;
	}
	return 0;
}

static const luaL_Reg l_texture[] = {
	{"new",	l_texture_new},
	{NULL,	NULL}
};

static int m_texture_gc(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	cairo_surface_destroy(texture->surface);
	return 0;
}

static int m_texture_size(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int w = cairo_image_surface_get_width(texture->surface);
	int h = cairo_image_surface_get_height(texture->surface);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

static int m_texture_region(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int x = luaL_optinteger(L, 2, 0);
	int y = luaL_optinteger(L, 3, 0);
	int w = luaL_optinteger(L, 4, cairo_image_surface_get_width(texture->surface));
	int h = luaL_optinteger(L, 5, cairo_image_surface_get_height(texture->surface));
	struct ltexture_t * tex = lua_newuserdata(L, sizeof(struct ltexture_t));
	tex->surface = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
	cairo_t * cr = cairo_create(tex->surface);
	cairo_set_source_surface(cr, texture->surface, -x, -y);
	cairo_paint(cr);
	cairo_destroy(cr);
	luaL_setmetatable(L, MT_NAME_TEXTURE);
	return 1;
}

static const luaL_Reg m_texture[] = {
	{"__gc",		m_texture_gc},
	{"size",		m_texture_size},
	{"region",		m_texture_region},
	{NULL,			NULL}
};

int luaopen_texture(lua_State * L)
{
	luaL_newlib(L, l_texture);
	luahelper_create_metatable(L, MT_NAME_TEXTURE, m_texture);
	return 1;
}
