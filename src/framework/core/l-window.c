/*
 * framework/core/l-window.c
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
#include <framework/core/l-window.h>

static int l_window_new(lua_State * L)
{
	struct window_t * w = ((struct vmctx_t *)luahelper_vmctx(L))->w;
	lua_pushlightuserdata(L, w);
	luaL_setmetatable(L, MT_WINDOW);
	return 1;
}

static const luaL_Reg l_window[] = {
	{"new",	l_window_new},
	{NULL,	NULL}
};

static int m_window_get_size(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	lua_pushnumber(L, window_get_width(w));
	lua_pushnumber(L, window_get_height(w));
	return 2;
}

static int m_window_get_physical_size(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	lua_pushnumber(L, window_get_pwidth(w));
	lua_pushnumber(L, window_get_pheight(w));
	return 2;
}

static int m_window_get_bytes_per_pixel(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	lua_pushnumber(L, window_get_bytes(w));
	return 1;
}

static int m_window_set_backlight(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	int brightness = luaL_checknumber(L, 2) * (lua_Number)(CONFIG_MAX_BRIGHTNESS);
	window_set_backlight(w, brightness);
	return 0;
}

static int m_window_get_backlight(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	int brightness = window_get_backlight(w);
	lua_pushnumber(L, brightness / (lua_Number)(CONFIG_MAX_BRIGHTNESS));
	return 1;
}

static int m_window_to_front(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	window_to_front(w);
	return 0;
}

static int m_window_to_back(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	window_to_back(w);
	return 0;
}

static int m_window_snapshot(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	struct limage_t * img = lua_newuserdata(L, sizeof(struct limage_t));
	int width = cairo_image_surface_get_width(w->cs);
	int height = cairo_image_surface_get_height(w->cs);
	img->cs = cairo_surface_create_similar(w->cs, cairo_surface_get_content(w->cs), width, height);
	cairo_t * cr = cairo_create(img->cs);
	cairo_set_source_surface(cr, w->cs, 0, 0);
	cairo_paint(cr);
	cairo_destroy(cr);
	luaL_setmetatable(L, MT_IMAGE);
	return 1;
}

static int m_window_showobj(lua_State * L)
{
	struct window_t * w = luaL_checkudata(L, 1, MT_WINDOW);
	window_set_showobj(w, lua_toboolean(L, 2));
	return 0;
}

static const luaL_Reg m_window[] = {
	{"getSize",				m_window_get_size},
	{"getPhysicalSize",		m_window_get_physical_size},
	{"getBytesPerPixel",	m_window_get_bytes_per_pixel},
	{"setBacklight",		m_window_set_backlight},
	{"getBacklight",		m_window_get_backlight},
	{"toFront",				m_window_to_front},
	{"toBack",				m_window_to_back},
	{"snapshot",			m_window_snapshot},
	{"showobj",				m_window_showobj},
	{NULL, NULL}
};

int luaopen_window(lua_State * L)
{
	luaL_newlib(L, l_window);
	luahelper_create_metatable(L, MT_WINDOW, m_window);
	return 1;
}
