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
	double r, g, b, a;
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
	display->r = 1;
	display->g = 1;
	display->b = 1;
	display->a = 1;
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
	cairo_surface_destroy(display->alone);
	cairo_destroy(display->cr[0]);
	cairo_destroy(display->cr[1]);
	cairo_surface_destroy(display->cs[0]);
	cairo_surface_destroy(display->cs[1]);
	return 0;
}

static int m_display_info(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	struct screen_info_t info;
	if(display->fb->ioctl)
		display->fb->ioctl(display->fb, IOCTL_FB_GET_SCREEN_INFORMATION, &info);
	lua_newtable(L);
	lua_pushnumber(L, info.width);
	lua_setfield(L, -2, "width");
	lua_pushnumber(L, info.height);
	lua_setfield(L, -2, "height");
	lua_pushnumber(L, info.xdpi);
	lua_setfield(L, -2, "xdpi");
	lua_pushnumber(L, info.ydpi);
	lua_setfield(L, -2, "ydpi");
	lua_pushnumber(L, info.bpp);
	lua_setfield(L, -2, "bpp");
	return 1;
}

static int m_display_get_backlight(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	int brightness = 0;
	if(display->fb->ioctl)
		display->fb->ioctl(display->fb, IOCTL_FB_GET_BACKLIGHT_BRIGHTNESS, &brightness);
	lua_pushnumber(L, brightness / 255.0);
	return 1;
}

static int m_display_set_backlight(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	int brightness = luaL_checknumber(L, 2) * 255.0;
	if(display->fb->ioctl)
		display->fb->ioctl(display->fb, IOCTL_FB_SET_BACKLIGHT_BRIGHTNESS, &brightness);
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
	cairo_paint_with_alpha(cr, alpha);
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
	cairo_save(cr);
	cairo_set_matrix(cr, matrix);
	cairo_set_source_surface(cr, texture->surface, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
	cairo_paint_with_alpha(cr, alpha);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_texture_mask(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	struct ltexture_t * texture = luaL_checkudata(L, 2, MT_NAME_TEXTURE);
	cairo_pattern_t ** pattern = luaL_checkudata(L, 3, MT_NAME_PARTTERN);
	cairo_matrix_t * matrix = luaL_checkudata(L, 4, MT_NAME_MATRIX);
	cairo_t * cr = display->cr[display->index];
	cairo_save(cr);
	cairo_set_matrix(cr, matrix);
	cairo_set_source_surface(cr, texture->surface, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
	cairo_mask(cr, *pattern);
	cairo_fill(cr);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_ninepatch(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	struct ninepatch_t * ninepatch = luaL_checkudata(L, 2, MT_NAME_NINEPATCH);
	cairo_matrix_t * matrix = luaL_checkudata(L, 3, MT_NAME_MATRIX);
	double alpha = luaL_optnumber(L, 4, 1.0);
	cairo_t * cr = display->cr[display->index];

	double w = ninepatch->width - ninepatch->left - ninepatch->right;
	double h = ninepatch->height - ninepatch->top - ninepatch->bottom;

	cairo_save(cr);
	cairo_set_matrix(cr, matrix);

	cairo_set_source_surface(cr, ninepatch->lt, 0, 0);
//	cairo_set_source_surface(cr, ninepatch->mt, ninepatch->left, 0);
	cairo_set_source_surface(cr, ninepatch->rt, ninepatch->left + w, 0);
/*
	cairo_set_source_surface(cr, ninepatch->lm, 0, ninepatch->top);
	cairo_set_source_surface(cr, ninepatch->mm, ninepatch->left, ninepatch->top);
	cairo_set_source_surface(cr, ninepatch->rm, ninepatch->left + w, ninepatch->top);

	cairo_set_source_surface(cr, ninepatch->lb, 0, ninepatch->top + h);
	cairo_set_source_surface(cr, ninepatch->mb, ninepatch->left, ninepatch->top + h);
	cairo_set_source_surface(cr, ninepatch->rb, ninepatch->left + w, ninepatch->top + h);
*/
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
	cairo_paint_with_alpha(cr, alpha);
	cairo_restore(cr);
//	LOG("w=%f,h=%f", ninepatch->width, ninepatch->height);
	return 0;
}

static int m_display_set_background_color(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	display->r = luaL_checknumber(L, 2);
	display->g = luaL_checknumber(L, 3);
	display->b = luaL_checknumber(L, 4);
	display->a = luaL_optnumber(L, 5, 1);
	return 0;
}

static int m_display_get_background_color(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	lua_pushnumber(L, display->r);
	lua_pushnumber(L, display->g);
	lua_pushnumber(L, display->b);
	return 3;
}

static int m_display_present(lua_State * L)
{
	struct display_t * display = luaL_checkudata(L, 1, MT_NAME_DISPLAY);
	cairo_xboot_surface_present(display->cs[display->index]);
	display->index = (display->index + 1) % 2;
	if(display->a != 0)
	{
		cairo_t * cr = display->cr[display->index];
		cairo_save(cr);
		cairo_set_source_rgb(cr, display->r, display->g, display->b);
		cairo_paint(cr);
		cairo_restore(cr);
	}
	return 0;
}

static const luaL_Reg m_display[] = {
	{"__gc",				m_display_gc},
	{"info",				m_display_info},
	{"getBacklight",		m_display_get_backlight},
	{"setBacklight",		m_display_set_backlight},
	{"drawShape",			m_display_draw_shape},
	{"drawText",			m_display_draw_text},
	{"drawTexture",			m_display_draw_texture},
	{"drawTextureMask",		m_display_draw_texture_mask},
	{"drawNinepatch",		m_display_draw_ninepatch},
	{"setBackgroundColor",	m_display_set_background_color},
	{"getBackgroundColor",	m_display_get_background_color},
	{"present",				m_display_present},
	{NULL,					NULL}
};

int luaopen_display(lua_State * L)
{
	luaL_newlib(L, l_display);
	luahelper_create_metatable(L, MT_NAME_DISPLAY, m_display);
	return 1;
}
