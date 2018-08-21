/*
 * framework/display/l-pattern.c
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
#include <framework/display/l-display.h>

static int l_pattern_create_color(lua_State * L)
{
	double red = luaL_optnumber(L, 1, 1);
	double green = luaL_optnumber(L, 2, 1);
	double blue = luaL_optnumber(L, 3, 1);
	double alpha = luaL_optnumber(L, 4, 1);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = cairo_pattern_create_rgba(red, green, blue, alpha);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int l_pattern_create_texture(lua_State * L)
{
	struct ltexture_t * texture = luaL_checkudata(L, 1, MT_TEXTURE);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = cairo_pattern_create_for_surface(texture->surface);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int l_pattern_create_linear(lua_State * L)
{
	double x0 = luaL_checknumber(L, 1);
	double y0 = luaL_checknumber(L, 2);
	double x1 = luaL_checknumber(L, 3);
	double y1 = luaL_checknumber(L, 4);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = cairo_pattern_create_linear(x0, y0, x1, y1);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int l_pattern_create_radial(lua_State * L)
{
	double cx0 = luaL_checknumber(L, 1);
	double cy0 = luaL_checknumber(L, 2);
	double radius0 = luaL_checknumber(L, 3);
	double cx1 = luaL_checknumber(L, 4);
	double cy1 = luaL_checknumber(L, 5);
	double radius1 = luaL_checknumber(L, 6);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static const luaL_Reg l_pattern[] = {
	{"color",	l_pattern_create_color},
	{"texture",	l_pattern_create_texture},
	{"linear",	l_pattern_create_linear},
	{"radial",	l_pattern_create_radial},
	{NULL,		NULL}
};

static int m_pattern_eq(lua_State * L)
{
	struct lpattern_t * pattern1 = luaL_checkudata(L, 1, MT_PATTERN);
	struct lpattern_t * pattern2 = luaL_checkudata(L, 2, MT_PATTERN);
	lua_pushboolean(L, (pattern1 == pattern2));
	return 1;
}

static int m_pattern_gc(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	cairo_pattern_destroy(pattern->pattern);
	return 0;
}

static int m_pattern_add_color_stop(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	double offset = luaL_checknumber(L, 2);
	double red = luaL_checknumber(L, 3);
	double green = luaL_checknumber(L, 4);
	double blue = luaL_checknumber(L, 5);
	double alpha = luaL_optnumber(L, 6, 1);
	cairo_pattern_add_color_stop_rgba(pattern->pattern, offset, red, green, blue, alpha);
	lua_pushlightuserdata(L, pattern);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int m_pattern_set_extend(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	cairo_extend_t extend = (cairo_extend_t)luaL_checkinteger(L, 2);
	cairo_pattern_set_extend(pattern->pattern, extend);
	lua_pushlightuserdata(L, pattern);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int m_pattern_set_filter(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	cairo_filter_t filter = (cairo_filter_t)luaL_checkinteger(L, 2);
	cairo_pattern_set_filter(pattern->pattern, filter);
	lua_pushlightuserdata(L, pattern);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int m_pattern_set_matrix(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	cairo_matrix_t * matrix = luaL_checkudata(L, 2, MT_MATRIX);
	cairo_pattern_set_matrix(pattern->pattern, matrix);
	lua_pushlightuserdata(L, pattern);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static const luaL_Reg m_pattern[] = {
	{"__eq",			m_pattern_eq},
	{"__gc",			m_pattern_gc},
	{"addColorStop",	m_pattern_add_color_stop},
	{"setExtend",		m_pattern_set_extend},
	{"setFilter",		m_pattern_set_filter},
	{"setMatrix",		m_pattern_set_matrix},
	{NULL,				NULL}
};

int luaopen_pattern(lua_State * L)
{
	luaL_newlib(L, l_pattern);
	/* cairo_extend_t */
	luahelper_set_intfield(L, "EXTEND_NONE",		CAIRO_EXTEND_NONE);
	luahelper_set_intfield(L, "EXTEND_REPEAT",		CAIRO_EXTEND_REPEAT);
	luahelper_set_intfield(L, "EXTEND_REFLECT",		CAIRO_EXTEND_REFLECT);
	luahelper_set_intfield(L, "EXTEND_PAD",			CAIRO_EXTEND_PAD);
	/* cairo_filter_t */
	luahelper_set_intfield(L, "FILTER_FAST",		CAIRO_FILTER_FAST);
	luahelper_set_intfield(L, "FILTER_GOOD",		CAIRO_FILTER_GOOD);
	luahelper_set_intfield(L, "FILTER_BEST",		CAIRO_FILTER_BEST);
	luahelper_set_intfield(L, "FILTER_NEAREST",		CAIRO_FILTER_NEAREST);
	luahelper_set_intfield(L, "FILTER_BILINEAR",	CAIRO_FILTER_BILINEAR);
	luahelper_set_intfield(L, "FILTER_GAUSSIAN",	CAIRO_FILTER_GAUSSIAN);
	luahelper_create_metatable(L, MT_PATTERN, m_pattern);
	return 1;
}
