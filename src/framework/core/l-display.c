/*
 * framework/core/l-display.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <framework/core/l-image.h>
#include <framework/core/l-display.h>

static int l_display_new(lua_State * L)
{
	struct display_t * disp = ((struct vmctx_t *)luahelper_vmctx(L))->disp;
	lua_pushlightuserdata(L, disp);
	luaL_setmetatable(L, MT_DISPLAY);
	return 1;
}

static const luaL_Reg l_display[] = {
	{"new",	l_display_new},
	{NULL,	NULL}
};

static int m_display_get_size(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, display_get_width(disp));
	lua_pushnumber(L, display_get_height(disp));
	return 2;
}

static int m_display_get_physical_size(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, display_get_pwidth(disp));
	lua_pushnumber(L, display_get_pheight(disp));
	return 2;
}

static int m_display_get_bits_per_pixel(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	lua_pushnumber(L, display_get_bpp(disp));
	return 1;
}

static int m_display_set_backlight(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	int brightness = luaL_checknumber(L, 2) * (lua_Number)(CONFIG_MAX_BRIGHTNESS);
	display_set_backlight(disp, brightness);
	return 0;
}

static int m_display_get_backlight(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	int brightness = display_get_backlight(disp);
	lua_pushnumber(L, brightness / (lua_Number)(CONFIG_MAX_BRIGHTNESS));
	return 1;
}

static int m_display_showobj(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	display_set_showobj(disp, lua_toboolean(L, 2));
	return 0;
}

static int m_display_showfps(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	display_set_showfps(disp, lua_toboolean(L, 2));
	return 0;
}

static int m_display_snapshot(lua_State * L)
{
	struct display_t * disp = luaL_checkudata(L, 1, MT_DISPLAY);
	struct limage_t * img = lua_newuserdata(L, sizeof(struct limage_t));
	int w = cairo_image_surface_get_width(disp->cs);
	int h = cairo_image_surface_get_height(disp->cs);
	img->cs = cairo_surface_create_similar(disp->cs, cairo_surface_get_content(disp->cs), w, h);
	cairo_t * cr = cairo_create(img->cs);
	cairo_set_source_surface(cr, disp->cs, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
}

static const luaL_Reg m_display[] = {
	{"getSize",			m_display_get_size},
	{"getPhysicalSize",	m_display_get_physical_size},
	{"getBitsPerPixel",	m_display_get_bits_per_pixel},
	{"setBacklight",	m_display_set_backlight},
	{"getBacklight",	m_display_get_backlight},
	{"showobj",			m_display_showobj},
	{"showfps",			m_display_showfps},
	{"snapshot",		m_display_snapshot},
	{NULL,				NULL}
};

int luaopen_display(lua_State * L)
{
	luaL_newlib(L, l_display);
	luahelper_create_metatable(L, MT_DISPLAY, m_display);
	return 1;
}
