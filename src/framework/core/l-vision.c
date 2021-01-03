/*
 * framework/core/l-vision.c
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
#include <vision/vision.h>
#include <core/l-image.h>
#include <core/l-vision.h>

static int l_vision_new(lua_State * L)
{
	struct vision_t * v = NULL;
	if(luaL_testudata(L, 1, MT_IMAGE))
	{
		struct limage_t * img = lua_touserdata(L, 1);
		v = vision_alloc(VISION_TYPE_RGB, surface_get_width(img->s), surface_get_height(img->s));
		if(v)
			vision_copy_from_surface(v, img->s);
	}
	else
	{
		if(lua_gettop(L) >= 2)
		{
			int width = luaL_checkinteger(L, 1);
			int height = luaL_checkinteger(L, 2);
			enum vision_type_t type = VISION_TYPE_GRAY;
			switch(shash(luaL_optstring(L, 3, NULL)))
			{
			case 0x7c977c78: /* "gray" */
				type = VISION_TYPE_GRAY;
				break;
			case 0x0b88a580: /* "rgb" */
				type = VISION_TYPE_RGB;
				break;
			case 0x0b887c96: /* "hsv" */
				type = VISION_TYPE_HSV;
				break;
			default:
				break;
			}
			v = vision_alloc(type, width, height);
		}
	}
	if(v)
	{
		struct lvision_t * vision = lua_newuserdata(L, sizeof(struct lvision_t));
		vision->v = v;
		luaL_setmetatable(L, MT_VISION);
		return 1;
	}
	return 0;
}

static const luaL_Reg l_vision[] = {
	{"new",	l_vision_new},
	{NULL,	NULL}
};

static int m_vision_gc(lua_State * L)
{
	struct lvision_t * vison = luaL_checkudata(L, 1, MT_VISION);
	vision_free(vison->v);
	return 0;
}

static int m_vision_tostring(lua_State * L)
{
	struct lvision_t * vison = luaL_checkudata(L, 1, MT_VISION);
	struct vision_t * v = vison->v;
	enum vision_type_t type = vision_get_type(v);
	int width = vision_get_width(v);
	int height = vision_get_height(v);
	void * datas = vision_get_datas(v);
	lua_pushfstring(L, "vision(0x%04x,%d,%d,%p)", type, width, height, datas);
	return 1;
}

static int m_vision_get_type(lua_State * L)
{
	struct lvision_t * vision = luaL_checkudata(L, 1, MT_VISION);
	switch(vision_get_type(vision->v))
	{
	case VISION_TYPE_GRAY:
		lua_pushstring(L, "gray");
		break;
	case VISION_TYPE_RGB:
		lua_pushstring(L, "rgb");
		break;
	case VISION_TYPE_HSV:
		lua_pushstring(L, "hsv");
		break;
	default:
		lua_pushnil(L);
		break;
	}
	return 1;
}

static int m_vision_get_type_bytes(lua_State * L)
{
	struct lvision_t * vison = luaL_checkudata(L, 1, MT_VISION);
	lua_pushnumber(L, vision_type_get_bytes(vison->v->type));
	return 1;
}

static int m_vision_get_type_channels(lua_State * L)
{
	struct lvision_t * vison = luaL_checkudata(L, 1, MT_VISION);
	lua_pushnumber(L, vision_type_get_channels(vison->v->type));
	return 1;
}

static int m_vision_get_width(lua_State * L)
{
	struct lvision_t * vison = luaL_checkudata(L, 1, MT_VISION);
	lua_pushnumber(L, vision_get_width(vison->v));
	return 1;
}

static int m_vision_get_height(lua_State * L)
{
	struct lvision_t * vison = luaL_checkudata(L, 1, MT_VISION);
	lua_pushnumber(L, vision_get_height(vison->v));
	return 1;
}

static int m_vision_get_size(lua_State * L)
{
	struct lvision_t * vison = luaL_checkudata(L, 1, MT_VISION);
	lua_pushnumber(L, vision_get_width(vison->v));
	lua_pushnumber(L, vision_get_height(vison->v));
	return 2;
}

static const luaL_Reg m_vision[] = {
	{"__gc",			m_vision_gc},
	{"__tostring",		m_vision_tostring},
	{"getType",			m_vision_get_type},
	{"getTypeBytes",	m_vision_get_type_bytes},
	{"getTypeChannels",	m_vision_get_type_channels},
	{"getWidth",		m_vision_get_width},
	{"getHeight",		m_vision_get_height},
	{"getSize",			m_vision_get_size},

	{NULL, NULL}
};

int luaopen_vision(lua_State * L)
{
	luaL_newlib(L, l_vision);
	luahelper_create_metatable(L, MT_VISION, m_vision);
	return 1;
}
