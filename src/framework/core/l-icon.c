/*
 * framework/core/l-icon.c
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

#include <xboot.h>
#include <core/l-color.h>
#include <core/l-icon.h>

static int l_icon_new(lua_State * L)
{
	uint32_t code = luaL_checkinteger(L, 1);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	const char * family = luaL_optstring(L, 3, NULL);
	int size = luaL_optnumber(L, 4, 16);
	struct licon_t * icon = lua_newuserdata(L, sizeof(struct licon_t));
	icon->family = strdup(family);
	memcpy(&icon->c, c, sizeof(struct color_t));
	icon_init(&icon->ico, code, &icon->c, ((struct vmctx_t *)luahelper_vmctx(L))->f, icon->family, size);
	luaL_setmetatable(L, MT_ICON);
	return 1;
}

static const luaL_Reg l_icon[] = {
	{"new",	l_icon_new},
	{NULL,	NULL}
};

static int m_icon_gc(lua_State * L)
{
	struct licon_t * icon = luaL_checkudata(L, 1, MT_ICON);
	if(icon->family)
		free(icon->family);
	return 0;
}

static int m_icon_set_code(lua_State * L)
{
	struct licon_t * icon = luaL_checkudata(L, 1, MT_ICON);
	uint32_t code = luaL_checkinteger(L, 2);
	icon_set_code(&icon->ico, code);
	lua_settop(L, 1);
	return 1;
}

static int m_icon_set_color(lua_State * L)
{
	struct licon_t * icon = luaL_checkudata(L, 1, MT_ICON);
	struct color_t * c = luaL_checkudata(L, 2, MT_COLOR);
	memcpy(&icon->c, c, sizeof(struct color_t));
	icon_set_color(&icon->ico, &icon->c);
	lua_settop(L, 1);
	return 1;
}

static int m_icon_set_family(lua_State * L)
{
	struct licon_t * icon = luaL_checkudata(L, 1, MT_ICON);
	const char * family = luaL_checkstring(L, 2);
	if(icon->family)
		free(icon->family);
	icon->family = strdup(family);
	icon_set_family(&icon->ico, icon->family);
	lua_settop(L, 1);
	return 1;
}

static int m_icon_set_size(lua_State * L)
{
	struct licon_t * icon = luaL_checkudata(L, 1, MT_ICON);
	int size = luaL_checknumber(L, 2);
	icon_set_size(&icon->ico, size);
	lua_settop(L, 1);
	return 1;
}

static int m_icon_get_size(lua_State * L)
{
	struct licon_t * icon = luaL_checkudata(L, 1, MT_ICON);
	lua_pushnumber(L, icon->ico.size);
	return 1;
}

static const luaL_Reg m_icon[] = {
	{"__gc",		m_icon_gc},
	{"setCode",		m_icon_set_code},
	{"setColor",	m_icon_set_color},
	{"setFamily",	m_icon_set_family},
	{"setSize",		m_icon_set_size},
	{"getSize",		m_icon_get_size},
	{NULL, NULL}
};

int luaopen_icon(lua_State * L)
{
	luaL_newlib(L, l_icon);
	luahelper_create_metatable(L, MT_ICON, m_icon);
	return 1;
}
