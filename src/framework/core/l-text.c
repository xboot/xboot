/*
 * framework/core/l-text.c
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
#include <framework/core/l-font.h>
#include <framework/core/l-color.h>
#include <framework/core/l-text.h>

static int l_text_new(lua_State * L)
{
	struct lfont_t * lfont = luaL_checkudata(L, 1, MT_FONT);
	const char * utf8 = luaL_checkstring(L, 2);
	struct ltext_t * text = lua_newuserdata(L, sizeof(struct ltext_t));
	text->s = ((struct vmctx_t *)luahelper_vmctx(L))->w->s;
	text->utf8 = strdup(utf8);
	text->sfont = lfont->sfont;
	text->size = luaL_optnumber(L, 3, 24);
	color_init(&text->c, luaL_optnumber(L, 4, 1), luaL_optnumber(L, 5, 1), luaL_optnumber(L, 6, 1), luaL_optnumber(L, 7, 1));
	surface_extent(text->s, text->utf8, text->sfont, text->size, &text->x, &text->y, &text->w, &text->h);
	luaL_setmetatable(L, MT_TEXT);
	return 1;
}

static const luaL_Reg l_text[] = {
	{"new",	l_text_new},
	{NULL,	NULL}
};

static int m_text_gc(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	if(text->utf8)
		free(text->utf8);
	return 0;
}

static int m_text_get_origin(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	lua_pushnumber(L, text->x);
	lua_pushnumber(L, text->y);
	return 2;
}

static int m_text_get_size(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	lua_pushnumber(L, text->w);
	lua_pushnumber(L, text->h);
	return 2;
}

static int m_text_set_text(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	const char * utf8 = luaL_checkstring(L, 2);
	if(text->utf8)
		free(text->utf8);
	text->utf8 = strdup(utf8);
	surface_extent(text->s, text->utf8, text->sfont, text->size, &text->x, &text->y, &text->w, &text->h);
	lua_settop(L, 1);
	return 1;
}

static int m_text_set_font(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	struct lfont_t * lfont = luaL_checkudata(L, 2, MT_FONT);
	text->sfont = lfont->sfont;
	surface_extent(text->s, text->utf8, text->sfont, text->size, &text->x, &text->y, &text->w, &text->h);
	lua_settop(L, 1);
	return 1;
}

static int m_text_set_size(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	text->size = luaL_optnumber(L, 2, 24);
	surface_extent(text->s, text->utf8, text->sfont, text->size, &text->x, &text->y, &text->w, &text->h);
	lua_settop(L, 1);
	return 1;
}

static int m_text_set_color(lua_State * L)
{
	struct ltext_t * text = luaL_checkudata(L, 1, MT_TEXT);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	memcpy(&text->c, c, sizeof(struct color_t));
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_text[] = {
	{"__gc",		m_text_gc},
	{"getOrigin",	m_text_get_origin},
	{"getSize",		m_text_get_size},
	{"setText",		m_text_set_text},
	{"setFont",		m_text_set_font},
	{"setSize",		m_text_set_size},
	{"setColor",	m_text_set_color},
	{NULL, NULL}
};

int luaopen_text(lua_State * L)
{
	luaL_newlib(L, l_text);
	luahelper_create_metatable(L, MT_TEXT, m_text);
	return 1;
}
