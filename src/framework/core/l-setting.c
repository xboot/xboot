/*
 * framework/core/l-setting.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <core/l-setting.h>

static int l_setting_set(lua_State * L)
{
	const char * key = luaL_checkstring(L, 1);
	const char * value = luaL_optstring(L, 2, NULL);
	setting_set(key, value);
	return 0;
}

static int l_setting_get(lua_State * L)
{
	const char * key = luaL_checkstring(L, 1);
	const char * def = luaL_optstring(L, 2, NULL);
	lua_pushstring(L, setting_get(key, def));
	return 1;
}

static int l_setting_clear(lua_State * L)
{
	setting_clear();
	return 0;
}

static const luaL_Reg l_setting[] = {
	{ "set",	l_setting_set },
	{ "get",	l_setting_get },
	{ "clear",	l_setting_clear },
	{ NULL, NULL }
};

int luaopen_setting(lua_State * L)
{
	luaL_newlib(L, l_setting);
	return 1;
}
