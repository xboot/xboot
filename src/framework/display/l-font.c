/*
 * framework/display/l-font.c
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
#include <cairo-ft.h>
#include <framework/display/l-display.h>

struct font_t {
	FT_Library library;
	FT_Face fface;
	cairo_font_face_t * face;
	cairo_scaled_font_t * sfont;
};

cairo_scaled_font_t * luaL_checkudata_scaled_font(lua_State * L, int ud, const char * tname)
{
	struct font_t * font = luaL_checkudata(L, ud, tname);
	return font->sfont;
}

static int l_font_new(lua_State * L)
{
	const char * family = luaL_checkstring(L, 1);
	struct font_t * font = lua_newuserdata(L, sizeof(struct font_t));
	if(FT_Init_FreeType(&font->library))
		return 0;
	if(FT_New_Face(font->library, family, 0, &font->fface))
	{
		FT_Done_FreeType(font->library);
		return 0;
	}
	font->face = cairo_ft_font_face_create_for_ft_face(font->fface, 0);
	if(font->face->status != CAIRO_STATUS_SUCCESS)
	{
		FT_Done_Face(font->fface);
		FT_Done_FreeType(font->library);
		cairo_font_face_destroy(font->face);
		return 0;
	}
	cairo_font_options_t * options = cairo_font_options_create();
	cairo_matrix_t identity;
	cairo_matrix_init_identity(&identity);
	font->sfont = cairo_scaled_font_create(font->face, &identity, &identity, options);
    cairo_font_options_destroy(options);
    if(cairo_scaled_font_status(font->sfont) != CAIRO_STATUS_SUCCESS)
    {
		FT_Done_Face(font->fface);
		FT_Done_FreeType(font->library);
		cairo_font_face_destroy(font->face);
		cairo_scaled_font_destroy(font->sfont);
        return 0;
    }
	luaL_setmetatable(L, MT_NAME_FONT);
	return 1;
}

static const luaL_Reg l_font[] = {
	{"new",	l_font_new},
	{NULL,	NULL}
};

static int m_font_gc(lua_State * L)
{
	struct font_t * font = luaL_checkudata(L, 1, MT_NAME_FONT);
	FT_Done_Face(font->fface);
	FT_Done_FreeType(font->library);
	cairo_font_face_destroy(font->face);
	cairo_scaled_font_destroy(font->sfont);
	return 0;
}

static int m_font_bounds(lua_State * L)
{
	struct font_t * font = luaL_checkudata(L, 1, MT_NAME_FONT);
	const char * text = luaL_optstring(L, 2, NULL);
	cairo_text_extents_t extents;
	cairo_scaled_font_text_extents(font->sfont, text, &extents);
	lua_newtable(L);
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "x");
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "y");
	lua_pushnumber(L, extents.width);
	lua_setfield(L, -2, "w");
	lua_pushnumber(L, extents.height);
	lua_setfield(L, -2, "h");
	return 1;
}

static const luaL_Reg m_font[] = {
	{"__gc",		m_font_gc},
	{"bounds",		m_font_bounds},
	{NULL,			NULL}
};

int luaopen_font(lua_State * L)
{
	luaL_newlib(L, l_font);
	luahelper_create_metatable(L, MT_NAME_FONT, m_font);
	return 1;
}
