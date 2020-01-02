/*
 * framework/core/l-font.c
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

#include <graphic/font.h>
#include <framework/core/l-font.h>

static int l_font_new(lua_State * L)
{
	struct font_context_t * f = ((struct vmctx_t *)luahelper_vmctx(L))->f;
	lua_pushlightuserdata(L, f);
	luaL_setmetatable(L, MT_FONT);
	return 1;
}

static const luaL_Reg l_font[] = {
	{"new",	l_font_new},
	{NULL,	NULL}
};

static int m_font_install(lua_State * L)
{
	struct font_context_t * f = luaL_checkudata(L, 1, MT_FONT);
	const char * family = luaL_checkstring(L, 2);
	const char * path = luaL_checkstring(L, 3);
	font_install(f, family, path);
	lua_settop(L, 1);
	return 1;
}

static int m_font_uninstall(lua_State * L)
{
	struct font_context_t * f = luaL_checkudata(L, 1, MT_FONT);
	const char * family = luaL_checkstring(L, 2);
	font_uninstall(f, family);
	lua_settop(L, 1);
	return 1;
}

static const luaL_Reg m_font[] = {
	{"install",		m_font_install},
	{"uninstall",	m_font_uninstall},
	{NULL,			NULL}
};

int luaopen_font(lua_State * L)
{
	luaL_newlib(L, l_font);
	luahelper_create_metatable(L, MT_FONT, m_font);
	return 1;
}
