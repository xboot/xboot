/*
 * framework/graphic/l-display.c
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

#include <xboot.h>
#include <framework/graphic/l-graphic.h>

static int l_display_new(lua_State * L)
{
	struct display_t * d = ((struct vmctx_t *)luahelper_vmctx(L))->d;
	lua_pushlightuserdata(L, d);
	luaL_setmetatable(L, MT_DISPLAY);
	return 1;
}

static const luaL_Reg l_display[] = {
	{"new",	l_display_new},
	{NULL,	NULL}
};

static int m_get_size(lua_State * L)
{
	struct display_t * s = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, s->fb->width);
	lua_pushnumber(L, s->fb->height);
	return 2;
}

static int m_get_physical_size(lua_State * L)
{
	struct display_t * s = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, s->fb->pwidth);
	lua_pushnumber(L, s->fb->pheight);
	return 2;
}

static int m_get_bits_per_pixel(lua_State * L)
{
	struct display_t * s = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, s->fb->bpp);
	return 1;
}

static int m_set_backlight(lua_State * L)
{
	struct display_t * s = luaL_checkudata(L, 1, MT_DISPLAY);
	int brightness = luaL_checknumber(L, 2) * (lua_Number)(CONFIG_MAX_BRIGHTNESS);
	framebuffer_set_backlight(s->fb, brightness);
	return 0;
}

static int m_get_backlight(lua_State * L)
{
	struct display_t * s = luaL_checkudata(L, 1, MT_DISPLAY);
	int brightness = framebuffer_get_backlight(s->fb);
	lua_pushnumber(L, brightness / (lua_Number)(CONFIG_MAX_BRIGHTNESS));
	return 1;
}

static int m_display_showfps(lua_State * L)
{
	struct display_t * d = luaL_checkudata(L, 1, MT_DISPLAY);
	int flag = lua_toboolean(L, 2) ? 1 : 0;
	if(flag && !d->showfps)
		d->stamp = ktime_get();
	d->showfps = flag;
	return 0;
}

static int m_present(lua_State * L)
{
	struct display_t * d = luaL_checkudata(L, 1, MT_DISPLAY);
	cairo_t * cr = d->cr;
	if(d->showfps)
	{
		char buf[32];
		ktime_t now = ktime_get();
		s64_t delta = ktime_ms_delta(now, d->stamp);
		if(delta > 0)
			d->fps = ((double)1000.0 / (double)delta) * 0.618 + d->fps * 0.382;
		d->frame++;
		d->stamp = now;
		cairo_save(cr);
		cairo_set_font_size(cr, 24);
		cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
		cairo_move_to(cr, 0, 24);
		snprintf(buf, sizeof(buf), "%.2f %ld", d->fps, d->frame);
		cairo_show_text(cr, buf);
		cairo_restore(cr);
	}
	cairo_xboot_surface_present(d->cs, NULL, 0);
	cairo_save(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_restore(cr);
	return 0;
}

static const luaL_Reg m_display[] = {
	{"getSize",			m_get_size},
	{"getPhysicalSize",	m_get_physical_size},
	{"getBitsPerPixel",	m_get_bits_per_pixel},
	{"setBacklight",	m_set_backlight},
	{"getBacklight",	m_get_backlight},
	{"showfps",			m_display_showfps},
	{"present",			m_present},
	{NULL,				NULL}
};

int luaopen_display(lua_State * L)
{
	luaL_newlib(L, l_display);
	luahelper_create_metatable(L, MT_DISPLAY, m_display);
	return 1;
}
