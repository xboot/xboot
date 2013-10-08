/*
 * framework/display/l-rectangle.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <framework/display/l-display.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

static int l_rectangle_new(lua_State * L)
{
	struct rectangle_t * r = lua_newuserdata(L, sizeof(struct rectangle_t));
	r->x = luaL_optnumber(L, 2, 0);
	r->y = luaL_optnumber(L, 3, 0);
	r->w = luaL_optnumber(L, 4, 0);
	r->h = luaL_optnumber(L, 5, 0);
	luaL_setmetatable(L, MT_NAME_RECTANGLE);
	return 1;
}

static const luaL_Reg l_rectangle[] = {
	{"new",	l_rectangle_new},
	{NULL,	NULL}
};

static int m_rectangle_set(lua_State * L)
{
	struct rectangle_t * r = luaL_checkudata(L, 1, MT_NAME_RECTANGLE);
	r->x = luaL_optnumber(L, 2, r->x);
	r->y = luaL_optnumber(L, 3, r->y);
	r->w = luaL_optnumber(L, 4, r->w);
	r->h = luaL_optnumber(L, 5, r->h);
	return 0;
}

static int m_rectangle_get(lua_State * L)
{
	struct rectangle_t * r = luaL_checkudata(L, 1, MT_NAME_RECTANGLE);
	lua_pushnumber(L, r->x);
	lua_pushnumber(L, r->y);
	lua_pushnumber(L, r->w);
	lua_pushnumber(L, r->h);
	return 4;
}

static int m_rectangle_is_empty(lua_State * L)
{
	struct rectangle_t * r = luaL_checkudata(L, 1, MT_NAME_RECTANGLE);
	lua_pushboolean(L, ((r->w > 0) && (r->h > 0)) ? 1 : 0);
	return 1;
}

static int m_rectangle_hit_test(lua_State * L)
{
	struct rectangle_t * a = luaL_checkudata(L, 1, MT_NAME_RECTANGLE);
	struct rectangle_t * b = luaL_checkudata(L, 2, MT_NAME_RECTANGLE);
	lua_pushboolean(L, ((a->x <= b->x + b->w) && (b->x <= a->x + a->w)
		&& (a->y <= b->y + b->h) && (b->y <= a->y + a->h)) ? 1 : 0);
	return 1;
}

static int m_rectangle_hit_test_point(lua_State * L)
{
	struct rectangle_t * r = luaL_checkudata(L, 1, MT_NAME_RECTANGLE);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	lua_pushboolean(L, ((x >= r->x) && (y >= r->y) && (x <= r->x + r->w) && (y <= r->y + r->h)) ? 1 : 0);
	return 1;
}

static int m_rectangle_intersection(lua_State * L)
{
	struct rectangle_t * r = luaL_checkudata(L, 1, MT_NAME_RECTANGLE);
	struct rectangle_t * a = luaL_checkudata(L, 2, MT_NAME_RECTANGLE);
	struct rectangle_t * b = luaL_checkudata(L, 3, MT_NAME_RECTANGLE);
	double x0 = MAX(a->x, b->x);
	double x1 = MIN(a->x + a->w, b->x + b->w);
	if(x0 <= x1)
	{
		double y0 = MAX(a->y, b->y);
		double y1 = MIN(a->y + a->h, b->y + b->h);
		if(y0 <= y1)
		{
			r->x = x0;
			r->y = y0;
			r->w = x1 - x0;
			r->h = y1 - y0;
			return 0;
		}
	}
	r->x = 0;
	r->y = 0;
	r->w = 0;
	r->h = 0;
	return 0;
}

static int m_rectangle_union(lua_State * L)
{
	struct rectangle_t * r = luaL_checkudata(L, 1, MT_NAME_RECTANGLE);
	struct rectangle_t * a = luaL_checkudata(L, 2, MT_NAME_RECTANGLE);
	struct rectangle_t * b = luaL_checkudata(L, 3, MT_NAME_RECTANGLE);
	struct rectangle_t t;
	t.x = MIN(a->x, b->x);
	t.y = MIN(a->y, b->y);
	t.w = MAX(a->x + a->w, b->x + b->w) - t.x;
	t.h = MAX(a->y + a->h, b->y + b->h) - t.y;
	memcpy(r, &t, sizeof(struct rectangle_t));
	return 0;
}

static const luaL_Reg m_rectangle[] = {
	{"set",				m_rectangle_set},
	{"get",				m_rectangle_get},
	{"isEmpty",			m_rectangle_is_empty},
	{"hitTest",			m_rectangle_hit_test},
	{"hitTestPoint",	m_rectangle_hit_test_point},
	{"intersection",	m_rectangle_intersection},
	{"union",			m_rectangle_union},
	{NULL,				NULL}
};

int luaopen_rectangle(lua_State * L)
{
	luaL_newlib(L, l_rectangle);
	luahelper_create_metatable(L, MT_NAME_RECTANGLE, m_rectangle);
	return 1;
}
