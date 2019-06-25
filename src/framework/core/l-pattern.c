/*
 * framework/core/l-pattern.c
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
#include <framework/core/l-matrix.h>
#include <framework/core/l-image.h>
#include <framework/core/l-pattern.h>

static int l_pattern_create_image(lua_State * L)
{
	struct limage_t * img = luaL_checkudata(L, 1, MT_IMAGE);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = surface_pattern_create(img->s);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int l_pattern_create_color(lua_State * L)
{
	double r = luaL_optnumber(L, 1, 1);
	double g = luaL_optnumber(L, 2, 1);
	double b = luaL_optnumber(L, 3, 1);
	double a = luaL_optnumber(L, 4, 1);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = surface_pattern_create_color(r, g, b, a);
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
	pattern->pattern = surface_pattern_create_linear(x0, y0, x1, y1);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static int l_pattern_create_radial(lua_State * L)
{
	double x0 = luaL_checknumber(L, 1);
	double y0 = luaL_checknumber(L, 2);
	double r0 = luaL_checknumber(L, 3);
	double x1 = luaL_checknumber(L, 4);
	double y1 = luaL_checknumber(L, 5);
	double r1 = luaL_checknumber(L, 6);
	struct lpattern_t * pattern = lua_newuserdata(L, sizeof(struct lpattern_t));
	pattern->pattern = surface_pattern_create_radial(x0, y0, r0, x1, y1, r1);
	luaL_setmetatable(L, MT_PATTERN);
	return 1;
}

static const luaL_Reg l_pattern[] = {
	{"image",	l_pattern_create_image},
	{"color",	l_pattern_create_color},
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
	surface_pattern_destroy(pattern->pattern);
	return 0;
}

static int m_pattern_add_color_stop(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	double o = luaL_checknumber(L, 2);
	double r = luaL_checknumber(L, 3);
	double g = luaL_checknumber(L, 4);
	double b = luaL_checknumber(L, 5);
	double a = luaL_optnumber(L, 6, 1);
	surface_pattern_add_color_stop(pattern->pattern, o, r, g, b, a);
	lua_settop(L, 1);
	return 1;
}

static int m_pattern_set_extend(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	const char * type = luaL_optstring(L, 2, "none");
	surface_pattern_set_extend(pattern->pattern, type);
	lua_settop(L, 1);
	return 1;
}

static int m_pattern_set_filter(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	const char * type = luaL_optstring(L, 2, "fast");
	surface_pattern_set_filter(pattern->pattern, type);
	lua_settop(L, 1);
	return 1;
}

static int m_pattern_set_matrix(lua_State * L)
{
	struct lpattern_t * pattern = luaL_checkudata(L, 1, MT_PATTERN);
	struct matrix_t * m = luaL_checkudata(L, 2, MT_MATRIX);
	surface_pattern_set_matrix(pattern->pattern, m);
	lua_settop(L, 1);
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
	luahelper_create_metatable(L, MT_PATTERN, m_pattern);
	return 1;
}
