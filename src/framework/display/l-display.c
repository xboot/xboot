/*
 * framework/display/l-display.c
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
#include <cairo-xboot.h>
#include <framework/display/l-display.h>

extern cairo_scaled_font_t * luaL_checkudata_scaled_font(lua_State * L, int ud, const char * tname);

struct ldisplay_t {
	struct framebuffer_t * fb;
	cairo_surface_t * alone;
	cairo_surface_t * cs;
	cairo_t * cr;

	int showfps;
	double fps;
	u64_t frame;
	ktime_t stamp;
};

static int l_display_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct ldisplay_t * display;
	struct framebuffer_t * fb = name ? search_framebuffer(name) : search_first_framebuffer();
	if(!fb)
		return 0;
	display = lua_newuserdata(L, sizeof(struct ldisplay_t));
	display->fb = fb;
	display->alone = cairo_xboot_surface_create(display->fb, display->fb->alone);
	display->cs = cairo_xboot_surface_create(display->fb, NULL);
	display->cr = cairo_create(display->cs);
	display->showfps = 0;
	display->fps = 60;
	display->frame = 0;
	display->stamp = ktime_get();
	luaL_setmetatable(L, MT_DISPLAY);
	return 1;
}

static const luaL_Reg l_display[] = {
	{"new",	l_display_new},
	{NULL,	NULL}
};

static int m_display_gc(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	cairo_xboot_surface_present(display->alone);
	cairo_surface_destroy(display->alone);
	cairo_destroy(display->cr);
	cairo_surface_destroy(display->cs);
	return 0;
}

static int m_display_get_size(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, display->fb->width);
	lua_pushnumber(L, display->fb->height);
	return 2;
}

static int m_display_get_physical_size(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, display->fb->pwidth);
	lua_pushnumber(L, display->fb->pheight);
	return 2;
}

static int m_display_get_bpp(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, display->fb->bpp);
	return 1;
}

static int m_display_get_backlight(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	int brightness = framebuffer_get_backlight(display->fb);
	lua_pushnumber(L, brightness / (lua_Number)(CONFIG_MAX_BRIGHTNESS));
	return 1;
}

static int m_display_set_backlight(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	int brightness = luaL_checknumber(L, 2) * (lua_Number)(CONFIG_MAX_BRIGHTNESS);
	framebuffer_set_backlight(display->fb, brightness);
	return 0;
}

static int m_display_draw_shape(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	struct lobject_t * object = luaL_checkudata(L, 2, MT_OBJECT);
	cairo_t ** shape = luaL_checkudata(L, 3, MT_SHAPE);
	cairo_t * cr = display->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, &object->__transform_matrix);
	cairo_surface_t * surface = cairo_surface_reference(cairo_get_target(*shape));
	cairo_set_source_surface(cr, surface, 0, 0);
	cairo_surface_destroy(surface);
	cairo_paint_with_alpha(cr, object->alpha);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_text(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	cairo_scaled_font_t * sfont = luaL_checkudata_scaled_font(L, 2, MT_FONT);
	const char * text = luaL_optstring(L, 3, NULL);
	struct lpattern_t * pattern = luaL_checkudata(L, 4, MT_PATTERN);
	cairo_matrix_t * matrix = luaL_checkudata(L, 5, MT_MATRIX);
	cairo_t * cr = display->cr;
	cairo_save(cr);
	cairo_set_scaled_font(cr, sfont);
	cairo_set_font_matrix(cr, matrix);
	cairo_text_path(cr, text);
	cairo_set_source(cr, pattern->pattern);
	cairo_fill(cr);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_texture(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	struct lobject_t * object = luaL_checkudata(L, 2, MT_OBJECT);
	struct ltexture_t * texture = luaL_checkudata(L, 3, MT_TEXTURE);
	cairo_t * cr = display->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, &object->__transform_matrix);
	cairo_set_source_surface(cr, texture->surface, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
	cairo_paint_with_alpha(cr, object->alpha);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_texture_mask(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	struct lobject_t * object = luaL_checkudata(L, 2, MT_OBJECT);
	struct ltexture_t * texture = luaL_checkudata(L, 3, MT_TEXTURE);
	struct lpattern_t * pattern = luaL_checkudata(L, 4, MT_PATTERN);
	cairo_t * cr = display->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, &object->__transform_matrix);
	cairo_set_source_surface(cr, texture->surface, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
	cairo_mask(cr, pattern->pattern);
	cairo_restore(cr);
	return 0;
}

static int m_display_draw_ninepatch(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	struct lobject_t * object = luaL_checkudata(L, 2, MT_OBJECT);
	struct lninepatch_t * ninepatch = luaL_checkudata(L, 3, MT_NINEPATCH);
	cairo_t * cr = display->cr;
	cairo_save(cr);
	cairo_set_matrix(cr, &object->__transform_matrix);
	if(ninepatch->lt)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, 0);
		cairo_set_source_surface(cr, ninepatch->lt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->mt)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, 0);
		cairo_scale(cr, ninepatch->__sx, 1);
		cairo_set_source_surface(cr, ninepatch->mt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rt)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, 0);
		cairo_set_source_surface(cr, ninepatch->rt, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->lm)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, ninepatch->top);
		cairo_scale(cr, 1, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->lm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->mm)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, ninepatch->top);
		cairo_scale(cr, ninepatch->__sx, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->mm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rm)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, ninepatch->top);
		cairo_scale(cr, 1, ninepatch->__sy);
		cairo_set_source_surface(cr, ninepatch->rm, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->lb)
	{
		cairo_save(cr);
		cairo_translate(cr, 0, ninepatch->__h - ninepatch->bottom);
		cairo_set_source_surface(cr, ninepatch->lb, 0, 0);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_restore(cr);
	}
	if(ninepatch->mb)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->left, ninepatch->__h - ninepatch->bottom);
		cairo_scale(cr, ninepatch->__sx, 1);
		cairo_set_source_surface(cr, ninepatch->mb, 0, 0);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_restore(cr);
	}
	if(ninepatch->rb)
	{
		cairo_save(cr);
		cairo_translate(cr, ninepatch->__w - ninepatch->right, ninepatch->__h - ninepatch->bottom);
		cairo_set_source_surface(cr, ninepatch->rb, 0, 0);
		cairo_paint_with_alpha(cr, object->alpha);
		cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
		cairo_restore(cr);
	}
	cairo_restore(cr);
	return 0;
}

static int m_display_showfps(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	int flag = lua_toboolean(L, 2) ? 1 : 0;
	if(flag && !display->showfps)
		display->stamp = ktime_get();
	display->showfps = flag;
	return 0;
}

static int m_display_present(lua_State * L)
{
	struct ldisplay_t * display = luaL_checkudata(L, 1, MT_DISPLAY);
	cairo_t * cr = display->cr;
	if(display->showfps)
	{
		char buf[32];
		ktime_t now = ktime_get();
		s64_t delta = ktime_ms_delta(now, display->stamp);
		if(delta > 0)
			display->fps = ((double)1000.0 / (double)delta) * 0.618 + display->fps * 0.382;
		display->frame++;
		display->stamp = now;
		cairo_save(cr);
		cairo_set_font_size(cr, 24);
		cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
		cairo_move_to(cr, 0, 24);
		snprintf(buf, sizeof(buf), "%.2f %d", display->fps, display->frame);
		cairo_show_text(cr, buf);
		cairo_restore(cr);
	}
	cairo_xboot_surface_present(display->cs);
	cairo_save(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_restore(cr);
	return 0;
}

static const luaL_Reg m_display[] = {
	{"__gc",				m_display_gc},
	{"getSize",				m_display_get_size},
	{"getPhysicalSize",		m_display_get_physical_size},
	{"getBpp",				m_display_get_bpp},
	{"getBacklight",		m_display_get_backlight},
	{"setBacklight",		m_display_set_backlight},
	{"drawShape",			m_display_draw_shape},
	{"drawText",			m_display_draw_text},
	{"drawTexture",			m_display_draw_texture},
	{"drawTextureMask",		m_display_draw_texture_mask},
	{"drawNinepatch",		m_display_draw_ninepatch},
	{"showfps",				m_display_showfps},
	{"present",				m_display_present},
	{NULL,					NULL}
};

int luaopen_display(lua_State * L)
{
	luaL_newlib(L, l_display);
	luahelper_create_metatable(L, MT_DISPLAY, m_display);
	return 1;
}
