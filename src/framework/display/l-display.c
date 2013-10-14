/*
 * framework/display/l-display.c
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
#include <cairo-xboot.h>
#include <framework/display/l-display.h>

extern cairo_scaled_font_t * luaL_checkudata_scaled_font(lua_State * L, int ud, const char * tname);

struct display_t {
	struct fb_t * fb;
	cairo_surface_t * alone;
	cairo_surface_t * cs[2];
	cairo_t * cr[2];
	int index;
};

static int l_display_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct display_t * display = lua_newuserdata(L, sizeof(struct display_t));
	display->fb = search_framebuffer(name);
	if(!display->fb)
		display->fb = search_first_framebuffer();
	display->alone = cairo_xboot_surface_create(display->fb, display->fb->alone);
	display->cs[0] = cairo_xboot_surface_create(display->fb, NULL);
	display->cs[1] = cairo_xboot_surface_create(display->fb, NULL);
	display->cr[0] = cairo_create(display->cs[0]);
	display->cr[1] = cairo_create(display->cs[1]);
	display->index = 0;
	luaL_setmetatable(L, MT_NAME_DISPLAY);
	return 1;
}

static const luaL_Reg l_display[] = {
	{"new",	l_display_new},
	{NULL,	NULL}
};

static int m_display_gc(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	cairo_xboot_surface_present(display->alone);
	cairo_destroy(display->cr[0]);
	cairo_destroy(display->cr[1]);
	cairo_surface_destroy(display->cs[0]);
	cairo_surface_destroy(display->cs[1]);
	return 0;
}

static int m_display_draw_shape(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	cairo_t ** shape = luaL_checkudata(L, 2, MT_NAME_SHAPE);
	cairo_matrix_t * matrix = luaL_checkudata(L, 3, MT_NAME_MATRIX);
	double alpha = luaL_optnumber(L, 4, 1.0);
	cairo_t * cr = display->cr[display->index];
	cairo_save(cr);
	cairo_set_matrix(cr, matrix);
	cairo_surface_t * surface = cairo_surface_reference(cairo_get_target(*shape));
	cairo_set_source_surface(cr, surface, 0, 0);
	cairo_surface_destroy(surface);
	if(alpha != 1.0)
		cairo_paint_with_alpha(cr, alpha);
	else
		cairo_paint(cr);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_text(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	cairo_scaled_font_t * sfont = luaL_checkudata_scaled_font(L, 2, MT_NAME_FONT);
	const char * text = luaL_optstring(L, 3, NULL);
	cairo_pattern_t ** pattern = luaL_checkudata(L, 4, MT_NAME_PARTTERN);
	cairo_matrix_t * matrix = luaL_checkudata(L, 5, MT_NAME_MATRIX);
	cairo_t * cr = display->cr[display->index];
	cairo_save(cr);
	cairo_set_scaled_font(cr, sfont);
	cairo_set_font_matrix(cr, matrix);
	cairo_text_path(cr, text);
	cairo_set_source(cr, *pattern);
	cairo_fill(cr);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_texture(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	struct ltexture_t * texture = luaL_checkudata(L, 2, MT_NAME_TEXTURE);
	cairo_matrix_t * matrix = luaL_checkudata(L, 3, MT_NAME_MATRIX);
	double alpha = luaL_optnumber(L, 4, 1.0);
	cairo_t * cr = display->cr[display->index];
	if(texture->patch.valid)
	{
		/*
		 * TODO
		 */
	}
	else
	{
		cairo_save(cr);
		cairo_set_matrix(cr, matrix);
		cairo_set_source_surface(cr, texture->surface, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, alpha);
		cairo_restore(cr);
	}
	return 0;
}

static int m_display_present(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	cairo_xboot_surface_present(display->cs[display->index]);
	display->index = (display->index + 1) % 2;
	return 0;
}

static const luaL_Reg m_display[] = {
	{"__gc",			m_display_gc},
	{"drawShape",		m_display_draw_shape},
	{"drawText",		m_display_draw_text},
	{"drawTexture",		m_display_draw_texture},
	{"present",			m_display_present},
	{NULL,				NULL}
};

int luaopen_display(lua_State * L)
{
	luaL_newlib(L, l_display);
	luahelper_create_metatable(L, MT_NAME_DISPLAY, m_display);
	return 1;
}
