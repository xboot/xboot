/*
 * framework/core/l-color.c
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
#include <framework/core/l-color.h>

static int l_color_new(lua_State * L)
{
	struct color_t * c;
	if(lua_istable(L, 1) && lua_rawlen(L, 1) == 4)
	{
		struct color_t t;
		lua_rawgeti(L, 1, 1); t.r = lua_tointeger(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 2); t.g = lua_tointeger(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 3); t.b = lua_tointeger(L, -1); lua_pop(L, 1);
		lua_rawgeti(L, 1, 4); t.a = lua_tointeger(L, -1); lua_pop(L, 1);
		c = lua_newuserdata(L, sizeof(struct color_t));
		memcpy(c, &t, sizeof(struct color_t));
	}
	else if(lua_isstring(L, 1))
	{
		const char * s = luaL_optstring(L, 1, "white");
		c = lua_newuserdata(L, sizeof(struct color_t));
		color_init_string(c, s);
	}
	else if(luaL_testudata(L, 1, MT_COLOR))
	{
		struct color_t * p = lua_touserdata(L, 1);
		c = lua_newuserdata(L, sizeof(struct color_t));
		memcpy(c, p, sizeof(struct color_t));
	}
	else
	{
		c = lua_newuserdata(L, sizeof(struct color_t));
		color_init(c, 0xff, 0xff, 0xff, 0xff);
	}
	luaL_setmetatable(L, MT_COLOR);
	return 1;
}

static const luaL_Reg l_color[] = {
	{"new",	l_color_new},
	{NULL,	NULL}
};

static int m_color_tostring(lua_State * L)
{
	struct color_t * c = luaL_checkudata(L, 1, MT_COLOR);
	lua_pushfstring(L, "#%02X%02X%02X%02X", c->r, c->g, c->b, c->a);
	return 1;
}

static int m_color_color(lua_State * L)
{
	struct color_t * c = luaL_checkudata(L, 1, MT_COLOR);
	lua_pushinteger(L, c->r);
	lua_pushinteger(L, c->g);
	lua_pushinteger(L, c->b);
	lua_pushinteger(L, c->a);
	return 4;
}

static int m_color_red(lua_State * L)
{
	struct color_t * c = luaL_checkudata(L, 1, MT_COLOR);
	lua_pushinteger(L, c->r);
	return 1;
}

static int m_color_green(lua_State * L)
{
	struct color_t * c = luaL_checkudata(L, 1, MT_COLOR);
	lua_pushinteger(L, c->g);
	return 1;
}

static int m_color_blue(lua_State * L)
{
	struct color_t * c = luaL_checkudata(L, 1, MT_COLOR);
	lua_pushinteger(L, c->b);
	return 1;
}

static int m_color_alpha(lua_State * L)
{
	struct color_t * c = luaL_checkudata(L, 1, MT_COLOR);
	lua_pushinteger(L, c->a);
	return 1;
}

static const luaL_Reg m_color[] = {
	{"__tostring",	m_color_tostring},
	{"color",		m_color_color},
	{"red",			m_color_red},
	{"green",		m_color_green},
	{"blue",		m_color_blue},
	{"alpha",		m_color_alpha},
	{NULL,	NULL}
};

int luaopen_color(lua_State * L)
{
	luaL_newlib(L, l_color);
	luahelper_create_metatable(L, MT_COLOR, m_color);
	return 1;
}
