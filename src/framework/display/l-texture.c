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
#include <framework/display/l-texture.h>

int l_texture_new(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	cairo_surface_t ** cs = lua_newuserdata(L, sizeof(cairo_surface_t *));
	*cs = cairo_image_surface_create_from_png(filename);
	luaL_setmetatable(L, MT_NAME_TEXTURE);
	return 1;
}

static const luaL_Reg l_texture[] = {
	{"new",	l_texture_new},
	{NULL,	NULL}
};

static int m_texture_gc(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	cairo_surface_destroy(*cs);
	return 0;
}

static int m_texture_get_width(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int w = cairo_image_surface_get_width(*cs);
	lua_pushinteger(L, w);
	return 1;
}

static int m_texture_get_height(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int h = cairo_image_surface_get_height(*cs);
	lua_pushinteger(L, h);
	return 1;
}

static int m_texture_region(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_TEXTURE);
	int x = luaL_optinteger(L, 2, 0);
	int y = luaL_optinteger(L, 3, 0);
	int w = luaL_optinteger(L, 4, cairo_image_surface_get_width(*cs));
	int h = luaL_optinteger(L, 5, cairo_image_surface_get_height(*cs));
	cairo_surface_t ** tcs = lua_newuserdata(L, sizeof(cairo_surface_t *));
	*tcs = cairo_surface_create_similar(*cs, CAIRO_CONTENT_COLOR_ALPHA, w, h);
	cairo_t * cr = cairo_create(*tcs);
	cairo_set_source_surface(cr, *cs, -x, -y);
	cairo_paint(cr);
	cairo_destroy(cr);
	luaL_setmetatable(L, MT_NAME_TEXTURE);
	return 1;
}

static const luaL_Reg m_texture[] = {
	{"__gc",		m_texture_gc},
	{"getWidth",	m_texture_get_width},
	{"getHeight",	m_texture_get_height},
	{"region",		m_texture_region},
	{NULL,			NULL}
};

int luaopen_texture(lua_State * L)
{
	luaL_newlib(L, l_texture);
	luahelper_create_metatable(L, MT_NAME_TEXTURE, m_texture);
	return 1;
}
