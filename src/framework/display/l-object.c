/*
 * framework/display/l-object.c
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

#include <math.h>
#include <cairo.h>
#include <cairoint.h>
#include <framework/display/l-display.h>

struct object_t {
	double x, y;
	double width, height;
	double rotation;
	double scalex, scaley;
	double anchorx, anchory;
	double alpha;

	int __translate;
	int __rotate;
	int __scale;
	int __anchor;

	int __matrix_valid;
	cairo_matrix_t __matrix;
};

static int l_object_new(lua_State * L)
{
	struct object_t * object = lua_newuserdata(L, sizeof(struct object_t));
	object->x = 0;
	object->y = 0;
	object->width = 0;
	object->height = 0;
	object->rotation = 0;
	object->scalex = 1;
	object->scaley = 1;
	object->anchorx = 0;
	object->anchory = 0;
	object->alpha = 1;

	object->__translate = 0;
	object->__rotate = 0;
	object->__scale = 0;
	object->__anchor = 0;

	object->__matrix_valid = 1;
	cairo_matrix_init_identity(&object->__matrix);

	luaL_setmetatable(L, MT_NAME_OBJECT);
	return 1;
}

static const luaL_Reg l_object[] = {
	{"new",	l_object_new},
	{NULL,	NULL}
};

static int m_translate(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double dx = luaL_checknumber(L, 2);
	double dy = luaL_checknumber(L, 3);
	object->x = object->x + dx;
	object->y = object->y + dy;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_rotate(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double rotation = luaL_checknumber(L, 2);
	object->rotation = object->rotation + rotation * (M_PI / 180.0);
	while(object->rotation < 0)
		object->rotation = object->rotation + (M_PI * 2);
	while(object->rotation > (M_PI * 2))
		object->rotation = object->rotation - (M_PI * 2);
	object->__rotate = (object->rotation != 0) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_scale(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double sx = luaL_checknumber(L, 2);
	double sy = luaL_checknumber(L, 3);
	object->scalex = object->scalex * sx;
	object->scaley = object->scaley * sy;
	object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_set_x(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double x = luaL_checknumber(L, 2);
	object->x = x;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_x(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->x);
	return 1;
}

static int m_set_y(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double y = luaL_checknumber(L, 2);
	object->y = y;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_y(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->y);
	return 1;
}

static int m_set_position(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	object->x = x;
	object->y = y;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_position(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->x);
	lua_pushnumber(L, object->y);
	return 2;
}

static int m_set_size(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double w = luaL_checknumber(L, 2);
	double h = luaL_checknumber(L, 3);
	object->width = w;
	object->height = h;
	return 0;
}

static int m_get_size(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->width);
	lua_pushnumber(L, object->height);
	return 2;
}

static int m_set_rotation(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double rotation = luaL_checknumber(L, 2);
	object->rotation = rotation * (M_PI / 180.0);
	while(object->rotation < 0)
		object->rotation = object->rotation + (M_PI * 2);
	while(object->rotation > (M_PI * 2))
		object->rotation = object->rotation - (M_PI * 2);
	object->__rotate = (object->rotation != 0) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_rotation(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->rotation / (M_PI / 180.0));
	return 1;
}

static int m_set_scale_x(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double x = luaL_checknumber(L, 2);
	object->scalex = x;
	object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_scale_x(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->scalex);
	return 1;
}

static int m_set_scale_y(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double y = luaL_checknumber(L, 2);
	object->scaley = y;
	object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_scale_y(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->scaley);
	return 1;
}

static int m_set_scale(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	object->scalex = x;
	object->scaley = y;
	object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_scale(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->scalex);
	lua_pushnumber(L, object->scaley);
	return 2;
}

static int m_set_archor(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	object->anchorx = x;
	object->anchory = y;
	object->__anchor = ((object->anchorx != 0) || (object->anchory != 0)) ? 1 : 0;
	object->__matrix_valid = 0;
	return 0;
}

static int m_get_archor(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->anchorx);
	lua_pushnumber(L, object->anchory);
	return 2;
}

static int m_set_alpha(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double alpha = luaL_checknumber(L, 2);
	object->alpha = alpha;
	return 0;
}

static int m_get_alpha(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushnumber(L, object->alpha);
	return 1;
}

static int m_get_matrix(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	cairo_matrix_t * matrix = lua_newuserdata(L, sizeof(cairo_matrix_t));
	cairo_matrix_t * m = &object->__matrix;
	if(!object->__matrix_valid)
	{
		cairo_matrix_init_identity(m);
		if(object->__translate)
			cairo_matrix_translate(m, object->x, object->y);
		if(object->__rotate)
			cairo_matrix_rotate(m, object->rotation);
		if(object->__anchor)
			cairo_matrix_translate(m, -object->anchorx * object->scalex, -object->anchory * object->scaley);
		if(object->__scale)
			cairo_matrix_scale(m, object->scalex, object->scaley);
		object->__matrix_valid = 1;
	}
	memcpy(matrix, m, sizeof(cairo_matrix_t));
	luaL_setmetatable(L, MT_NAME_MATRIX);
	return 1;
}

static int m_bounds(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	cairo_matrix_t * matrix = luaL_checkudata(L, 2, MT_NAME_MATRIX);
	struct rectangle_t * r = lua_newuserdata(L, sizeof(struct rectangle_t));
	double x1 = 0;
	double y1 = 0;
	double x2 = object->width;
	double y2 = object->height;
	_cairo_matrix_transform_bounding_box(matrix, &x1, &y1, &x2, &y2, NULL);
	r->x = x1;
	r->y = y1;
	r->w = x2 - x1;
	r->h = y2 - y1;
	luaL_setmetatable(L, MT_NAME_RECTANGLE);
	return 1;
}

static const luaL_Reg m_object[] = {
	{"translate",	m_translate},
	{"rotate",		m_rotate},
	{"scale",		m_scale},
	{"setX",		m_set_x},
	{"getX",		m_get_x},
	{"setY",		m_set_y},
	{"getY",		m_get_y},
	{"setPosition",	m_set_position},
	{"getPosition",	m_get_position},
	{"setSize",		m_set_size},
	{"getSize",		m_get_size},
	{"setRotation",	m_set_rotation},
	{"getRotation",	m_get_rotation},
	{"setScaleX",	m_set_scale_x},
	{"getScaleX",	m_get_scale_x},
	{"setScaleY",	m_set_scale_y},
	{"getScaleY",	m_get_scale_y},
	{"setScale",	m_set_scale},
	{"getScale",	m_get_scale},
	{"setAnchor",	m_set_archor},
	{"getAnchor",	m_get_archor},
	{"setAlpha",	m_set_alpha},
	{"getAlpha",	m_get_alpha},
	{"getMatrix",	m_get_matrix},
	{"bounds",		m_bounds},
	{NULL,			NULL}
};

int luaopen_object(lua_State * L)
{
	luaL_newlib(L, l_object);
	luahelper_create_metatable(L, MT_NAME_OBJECT, m_object);
	return 1;
}
