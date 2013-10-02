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
#include <framework/display/l-display.h>

static inline int is_black_pixel(unsigned char * p)
{
	return (((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] == 0xff)) ? 1 : 0);
}

static bool_t fill_nine_patch(struct ltexture_t * texture)
{
	cairo_surface_t * cs;
	cairo_t * cr;
	unsigned char * data;
	int width, height;
	int stride;
	int w, h;
	int i;

	if(!texture || !texture->surface)
		return FALSE;

	width = cairo_image_surface_get_width(texture->surface);
	height = cairo_image_surface_get_height(texture->surface);
	if(width < 3 || height < 3)
		return FALSE;

	/* Nine patch chunk */
	cs = cairo_surface_create_similar_image(texture->surface, CAIRO_FORMAT_ARGB32, width, height);
	cr = cairo_create(cs);
	cairo_set_source_surface(cr, texture->surface, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);

	data = cairo_image_surface_get_data(cs);
	stride = cairo_image_surface_get_stride(cs);

	/* Top stretch row line */
	texture->patch.stretch.a = 1;
	texture->patch.stretch.b = width - 2;
	for(i = 1; i < width - 1; i++)
	{
		if(is_black_pixel(&data[stride * 0 + i * 4]))
		{
			texture->patch.stretch.a = i;
			break;
		}
	}
	for(i = width - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * 0 + i * 4]))
		{
			texture->patch.stretch.b = i;
			break;
		}
	}

	/* Left stretch column line */
	texture->patch.stretch.c = 1;
	texture->patch.stretch.d = height - 2;
	for(i = 1; i < height - 1; i++)
	{
		if(is_black_pixel(&data[stride * i + 0 * 4]))
		{
			texture->patch.stretch.c = i;
			break;
		}
	}
	for(i = height - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * i + 0 * 4]))
		{
			texture->patch.stretch.d = i;
			break;
		}
	}

	/* Bottom content row line */
	texture->patch.content.a = 1;
	texture->patch.content.b = width - 2;
	for(i = 1; i < width - 1; i++)
	{
		if(is_black_pixel(&data[stride * (height - 1) + i * 4]))
		{
			texture->patch.content.a = i;
			break;
		}
	}
	for(i = width - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * (height - 1) + i * 4]))
		{
			texture->patch.content.b = i;
			break;
		}
	}

	/* Right content column line */
	texture->patch.content.c = 1;
	texture->patch.content.d = height - 2;
	for(i = 1; i < height - 1; i++)
	{
		if(is_black_pixel(&data[stride * i + (width - 1) * 4]))
		{
			texture->patch.content.c = i;
			break;
		}
	}
	for(i = height - 2; i > 0; i--)
	{
		if(is_black_pixel(&data[stride * i + (width - 1) * 4]))
		{
			texture->patch.content.d = i;
			break;
		}
	}
	cairo_surface_destroy(cs);

	/* Top left */
	w = texture->patch.stretch.a - 1;
	h = texture->patch.stretch.c - 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -1, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.tl = cs;
	}
	else
	{
		texture->patch.tl = NULL;
	}

	/* Top Middle */
	w = texture->patch.stretch.b - texture->patch.stretch.a + 1;
	h = texture->patch.stretch.c - 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -texture->patch.stretch.a, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.tm = cs;
	}
	else
	{
		texture->patch.tm = NULL;
	}

	/* Top Right */
	w = width -2 - texture->patch.stretch.b;
	h = texture->patch.stretch.c - 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -texture->patch.stretch.b - 1, -1);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.tr = cs;
	}
	else
	{
		texture->patch.tr = NULL;
	}

	/* Middle left */
	w = texture->patch.stretch.a - 1;
	h = texture->patch.stretch.d - texture->patch.stretch.c + 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -1, -texture->patch.stretch.c);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.ml = cs;
	}
	else
	{
		texture->patch.ml = NULL;
	}

	/* Middle Middle */
	w = texture->patch.stretch.b - texture->patch.stretch.a + 1;
	h = texture->patch.stretch.d - texture->patch.stretch.c + 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -texture->patch.stretch.a, -texture->patch.stretch.c);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.mm = cs;
	}
	else
	{
		texture->patch.mm = NULL;
	}

	/* Middle Right */
	w = width -2 - texture->patch.stretch.b;
	h = texture->patch.stretch.d - texture->patch.stretch.c + 1;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -texture->patch.stretch.b - 1, -texture->patch.stretch.c);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.mr = cs;
	}
	else
	{
		texture->patch.mr = NULL;
	}

	/* Bottom left */
	w = texture->patch.stretch.a - 1;
	h = height - 2 - texture->patch.stretch.d;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -1, -texture->patch.stretch.d - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.bl = cs;
	}
	else
	{
		texture->patch.bl = NULL;
	}

	/* Bottom Middle */
	w = texture->patch.stretch.b - texture->patch.stretch.a + 1;
	h = height - 2 - texture->patch.stretch.d;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -texture->patch.stretch.a, -texture->patch.stretch.d - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.bm = cs;
	}
	else
	{
		texture->patch.bm = NULL;
	}

	/* Bottom Right */
	w = width -2 - texture->patch.stretch.b;
	h = height - 2 - texture->patch.stretch.d;
	if(w > 0 && h > 0)
	{
		cs = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
		cr = cairo_create(cs);
		cairo_set_source_surface(cr, texture->surface, -texture->patch.stretch.b - 1, -texture->patch.stretch.d - 1);
		cairo_paint(cr);
		cairo_destroy(cr);
		texture->patch.br = cs;
	}
	else
	{
		texture->patch.br = NULL;
	}

	return TRUE;
}

static bool_t match_extension(const char * filename, const char * ext)
{
	int pos, len;

	pos = strlen(filename);
	len = strlen(ext);

	if ((!pos) || (!len) || (len > pos))
		return FALSE;

	pos -= len;
	return (strcasecmp(filename + pos, ext) == 0);
}

static int l_texture_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	if(match_extension(filename, ".png"))
	{
		struct ltexture_t * texture = lua_newuserdata(L, sizeof(struct ltexture_t));
		texture->surface = cairo_image_surface_create_from_png(filename);
		if(cairo_surface_status(texture->surface) != CAIRO_STATUS_SUCCESS)
			return 0;
		if(match_extension(filename, ".9.png") && fill_nine_patch(texture))
			texture->patch.valid = 1;
		else
			texture->patch.valid = 0;
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
	if(texture->patch.valid)
	{
		if(texture->patch.tl)
			cairo_surface_destroy(texture->patch.tl);
		if(texture->patch.tm)
			cairo_surface_destroy(texture->patch.tm);
		if(texture->patch.tr)
			cairo_surface_destroy(texture->patch.tr);
		if(texture->patch.ml)
			cairo_surface_destroy(texture->patch.ml);
		if(texture->patch.mm)
			cairo_surface_destroy(texture->patch.mm);
		if(texture->patch.mr)
			cairo_surface_destroy(texture->patch.mr);
		if(texture->patch.bl)
			cairo_surface_destroy(texture->patch.bl);
		if(texture->patch.bm)
			cairo_surface_destroy(texture->patch.bm);
		if(texture->patch.br)
			cairo_surface_destroy(texture->patch.br);
		texture->patch.valid = 0;
	}
	return 0;
}

static int m_texture_get_height(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int h = cairo_image_surface_get_height(texture->surface);
	lua_pushinteger(L, h);
	return 1;
}

static int m_texture_get_width(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int w = cairo_image_surface_get_width(texture->surface);
	lua_pushinteger(L, w);
	return 1;
}

static int m_texture_region(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int x = luaL_optinteger(L, 2, 0);
	int y = luaL_optinteger(L, 3, 0);
	int w = luaL_optinteger(L, 4, cairo_image_surface_get_width(texture->surface));
	int h = luaL_optinteger(L, 5, cairo_image_surface_get_height(texture->surface));
	if(texture->patch.valid)
		return 0;
	struct ltexture_t * tex = lua_newuserdata(L, sizeof(struct ltexture_t));
	tex->surface = cairo_surface_create_similar(texture->surface, cairo_surface_get_content(texture->surface), w, h);
	tex->patch.valid = 0;
	cairo_t * cr = cairo_create(tex->surface);
	cairo_set_source_surface(cr, texture->surface, -x, -y);
	cairo_paint(cr);
	cairo_destroy(cr);
	luaL_setmetatable(L, MT_NAME_TEXTURE);
	return 1;
}

static int m_texture_to_pattern(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	if(texture->patch.valid)
		return 0;
	cairo_pattern_t ** pattern = lua_newuserdata(L, sizeof(cairo_pattern_t *));
	*pattern = cairo_pattern_create_for_surface(texture->surface);
	luaL_setmetatable(L, MT_NAME_PARTTERN);
	return 1;
}

static const luaL_Reg m_texture[] = {
	{"__gc",		m_texture_gc},
	{"getHeight",	m_texture_get_height},
	{"getWidth",	m_texture_get_width},
	{"region",		m_texture_region},
	{"toPattern",	m_texture_to_pattern},
	{NULL,			NULL}
};

int luaopen_texture(lua_State * L)
{
	luaL_newlib(L, l_texture);
	luahelper_create_metatable(L, MT_NAME_TEXTURE, m_texture);
	return 1;
}
