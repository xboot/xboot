/*
 * framework/core/l-text.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <framework/core/l-text.h>

static int l_text_new(lua_State * L)
{
	const char * utf8 = luaL_checkstring(L, 1);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	const char * family = luaL_optstring(L, 3, NULL);
	int size = luaL_optinteger(L, 4, 24);
	struct text_t * txt = text_alloc(utf8, c, ((struct vmctx_t *)luahelper_vmctx(L))->f, family, size);
	if(txt)
	{
		struct ltext_t * text = lua_newuserdata(L, sizeof(struct ltext_t));
		text->txt = txt;
		luaL_setmetatable(L, MT_TEXT);
		return 1;
	}
	return 0;
}

static const luaL_Reg l_text[] = {
	{"new",	l_text_new},
	{NULL,	NULL}
};

static int m_text_gc(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	text_free(text->txt);
	return 0;
}

static int m_text_get_origin(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	lua_pushinteger(L, text->txt->e.x);
	lua_pushinteger(L, text->txt->e.y);
	return 2;
}

static int m_text_get_size(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	lua_pushinteger(L, text->txt->e.w);
	lua_pushinteger(L, text->txt->e.h);
	return 2;
}

static int m_text_set_text(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	const char * utf8 = luaL_checkstring(L, 2);
	text_set_text(text->txt, utf8);
	lua_settop(L, 1);
	return 1;
}

static int m_text_set_color(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	text_set_color(text->txt, c);
	lua_settop(L, 1);
	return 1;
}

static int m_text_set_font_family(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	const char * family = luaL_checkstring(L, 2);
	text_set_font_family(text->txt, family);
	lua_settop(L, 1);
	return 1;
}

static int m_text_set_font_size(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	int size = luaL_optinteger(L, 2, 0);
	text_set_font_size(text->txt, size);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_text[] = {
	{"__gc",			m_text_gc},
	{"getOrigin",		m_text_get_origin},
	{"getSize",			m_text_get_size},
	{"setText",			m_text_set_text},
	{"setColor",		m_text_set_color},
	{"setFontFamily",	m_text_set_font_family},
	{"setFontSize",		m_text_set_font_size},
	{NULL, NULL}
};

int luaopen_text(lua_State * L)
{
	luaL_newlib(L, l_text);
	luahelper_create_metatable(L, MT_TEXT, m_text);
	return 1;
}
