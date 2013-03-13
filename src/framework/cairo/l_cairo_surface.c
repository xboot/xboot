/*
 * framework/cairo/l_cairo_surface.c
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
#include <framework/cairo/l_cairo.h>

int l_cairo_image_surface_create(lua_State * L)
{
	cairo_format_t format = (cairo_format_t)luaL_checkint(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	cairo_surface_t ** cs = lua_newuserdata(L, sizeof(cairo_surface_t *));
	*cs = cairo_image_surface_create(format, width, height);
	luaL_setmetatable(L, MT_NAME_CAIRO_SURFACE);
	return 1;
}

int l_cairo_image_surface_create_from_png(lua_State * L)
{
	const char * filename = luaL_checkstring(L, 1);
	cairo_surface_t ** cs = lua_newuserdata(L, sizeof(cairo_surface_t *));
	*cs = cairo_image_surface_create_from_png(filename);
	luaL_setmetatable(L, MT_NAME_CAIRO_SURFACE);
	return 1;
}

int l_cairo_surface_create_similar(lua_State * L)
{
	cairo_surface_t ** other = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	cairo_content_t content = (cairo_content_t)luaL_checkinteger(L, 2);
	int width = luaL_checkinteger(L, 3);
	int height = luaL_checkinteger(L, 4);
	cairo_surface_t ** cs = lua_newuserdata(L, sizeof(cairo_surface_t *));
	*cs = cairo_surface_create_similar(*other, content, width, height);
	luaL_setmetatable(L, MT_NAME_CAIRO_SURFACE);
	return 1;
}

static int m_cairo_surface_gc(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	cairo_surface_destroy(*cs);
	return 0;
}

static int m_cairo_surface_get_width(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	int v = cairo_image_surface_get_width(*cs);
	lua_pushinteger(L, v);
	return 1;
}

static int m_cairo_surface_get_height(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	int v = cairo_image_surface_get_height(*cs);
	lua_pushinteger(L, v);
	return 1;
}

extern void show_to_framebuffer(cairo_surface_t * surface);
static int m_cairo_surface_show(lua_State * L)
{
	cairo_surface_t ** cs = luaL_checkudata(L, 1, MT_NAME_CAIRO_SURFACE);
	show_to_framebuffer(*cs);
	return 0;
}

const luaL_Reg m_cairo_surface[] = {
	{"__gc",				m_cairo_surface_gc},
	{"get_width",			m_cairo_surface_get_width},
	{"get_height",			m_cairo_surface_get_height},
	{"show",				m_cairo_surface_show},
	{NULL, 					NULL}
};
