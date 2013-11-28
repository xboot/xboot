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

#include <cairo.h>
#include <cairoint.h>
#include <framework/display/l-display.h>

enum alignment_t {
	ALIGN_NONE					= 0,
	ALIGN_LEFT					= 1,
	ALIGN_TOP					= 2,
	ALIGN_RIGHT					= 3,
	ALIGN_BOTTOM				= 4,
	ALIGN_LEFT_TOP				= 5,
	ALIGN_RIGHT_TOP				= 6,
	ALIGN_LEFT_BOTTOM			= 7,
	ALIGN_RIGHT_BOTTOM			= 8,
	ALIGN_LEFT_CENTER			= 9,
	ALIGN_TOP_CENTER			= 10,
	ALIGN_RIGHT_CENTER			= 11,
	ALIGN_BOTTOM_CENTER			= 12,
	ALIGN_HORIZONTAL_CENTER		= 13,
	ALIGN_VERTICAL_CENTER		= 14,
	ALIGN_CENTER				= 15,
	ALIGN_LEFT_FILL				= 16,
	ALIGN_TOP_FILL				= 17,
	ALIGN_RIGHT_FILL			= 18,
	ALIGN_BOTTOM_FILL			= 19,
	ALIGN_HORIZONTAL_FILL		= 20,
	ALIGN_VERTICAL_FILL			= 21,
	ALIGN_CENTER_FILL			= 22,
};

struct object_t {
	double width, height;
	double x, y;
	double rotation;
	double scalex, scaley;
	double anchorx, anchory;
	double alpha;
	enum alignment_t alignment;
	int visible;
	int touchable;

	int __translate;
	int __rotate;
	int __scale;
	int __anchor;

	int __matrix_valid;
	cairo_matrix_t __matrix;
};

static void __object_translate(struct object_t * object, double dx, double dy)
{
	object->x = object->x + dx;
	object->y = object->y + dy;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__matrix_valid = 0;
}

static void __object_translate_fill(struct object_t * object, double x, double y, double w, double h)
{
	object->x = x;
	object->y = y;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;

	if(object->width != 0 && object->height != 0)
	{
		object->scalex = w / object->width;
		object->scaley = h / object->height;
		object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	}

	object->rotation = 0;
	object->__rotate = 0;
	object->anchorx = 0;
	object->anchory = 0;
	object->__anchor = 0;
	object->__matrix_valid = 0;
}

static inline cairo_matrix_t * __get_matrix(struct object_t * object)
{
	cairo_matrix_t * m = &object->__matrix;
	if(!object->__matrix_valid)
	{
		cairo_matrix_init_identity(m);
		if(object->__translate)
			cairo_matrix_translate(m, object->x, object->y);
		if(object->__rotate)
			cairo_matrix_rotate(m, object->rotation);
		if(object->__anchor)
			cairo_matrix_translate(m, -object->anchorx * object->width * object->scalex, -object->anchory * object->height * object->scaley);
		if(object->__scale)
			cairo_matrix_scale(m, object->scalex, object->scaley);
		object->__matrix_valid = 1;
	}
	return m;
}

static int l_object_new(lua_State * L)
{
	struct object_t * object = lua_newuserdata(L, sizeof(struct object_t));
	object->width = 0;
	object->height = 0;
	object->x = 0;
	object->y = 0;
	object->rotation = 0;
	object->scalex = 1;
	object->scaley = 1;
	object->anchorx = 0;
	object->anchory = 0;
	object->alpha = 1;
	object->alignment = ALIGN_NONE;
	object->visible = 1;
	object->touchable = 1;

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

static int m_set_alignment(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	object->alignment = (enum alignment_t)luaL_checkinteger(L, 2);
	return 0;
}

static int m_get_alignment(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushinteger(L, object->alignment);
	return 1;
}

static int m_set_visible(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	object->visible = lua_toboolean(L, 2) ? 1 : 0;
	return 0;
}

static int m_get_visible(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushboolean(L, object->visible);
	return 1;
}

static int m_set_touchable(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	object->touchable = lua_toboolean(L, 2) ? 1 : 0;
	return 0;
}

static int m_get_touchable(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	lua_pushboolean(L, object->touchable);
	return 1;
}

static int m_get_matrix(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	cairo_matrix_t * matrix = lua_newuserdata(L, sizeof(cairo_matrix_t));
	memcpy(matrix, __get_matrix(object), sizeof(cairo_matrix_t));
	luaL_setmetatable(L, MT_NAME_MATRIX);
	return 1;
}

static int m_hit_test_point(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	double ox = luaL_checknumber(L, 2);
	double oy = luaL_checknumber(L, 3);
	if(object->visible && object->touchable)
		lua_pushboolean(L, ((ox >= 0) && (oy >= 0) && (ox <= object->width) && (oy <= object->height)) ? 1 : 0);
	else
		lua_pushboolean(L, 0);
	return 1;
}

static int m_bounds(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	cairo_matrix_t * matrix = luaL_checkudata(L, 2, MT_NAME_MATRIX);
	double x1 = 0;
	double y1 = 0;
	double x2 = object->width;
	double y2 = object->height;
	_cairo_matrix_transform_bounding_box(matrix, &x1, &y1, &x2, &y2, NULL);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2 - x1);
	lua_pushnumber(L, y2 - y1);
	return 4;
}

static int m_layout(lua_State * L)
{
	struct object_t * object = luaL_checkudata(L, 1, MT_NAME_OBJECT);
	struct object_t * child = luaL_checkudata(L, 2, MT_NAME_OBJECT);
	double rx1 = luaL_optnumber(L, 3, 0);
	double ry1 = luaL_optnumber(L, 4, 0);
	double rx2 = luaL_optnumber(L, 5, object->width);
	double ry2 = luaL_optnumber(L, 6, object->height);

	if(child->alignment <= ALIGN_NONE)
	{
	}
	else if((child->alignment <= ALIGN_CENTER))
	{
		double ox1 = rx1;
		double oy1 = ry1;
		double ox2 = rx2;
		double oy2 = ry2;
		double cx1 = 0;
		double cy1 = 0;
		double cx2 = child->width;
		double cy2 = child->height;
		_cairo_matrix_transform_bounding_box(__get_matrix(child), &cx1, &cy1, &cx2, &cy2, NULL);

		switch(child->alignment)
		{
		case ALIGN_LEFT:
			__object_translate(child, ox1 - cx1, 0);
			rx1 += cx2 - cx1;
			break;
		case ALIGN_TOP:
			__object_translate(child, 0, oy1 - cy1);
			ry1 += cy2 - cy1;
			break;
		case ALIGN_RIGHT:
			__object_translate(child, ox2 - cx2, 0);
			rx2 -= cx2 - cx1;
			break;
		case ALIGN_BOTTOM:
			__object_translate(child, 0, oy2 - cy2);
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_LEFT_TOP:
			__object_translate(child, ox1 - cx1, oy1 - cy1);
			rx1 += cx2 - cx1;
			ry1 += cy2 - cy1;
			break;
		case ALIGN_RIGHT_TOP:
			__object_translate(child, ox2 - cx2, oy1 - cy1);
			rx2 -= cx2 - cx1;
			ry1 += cy2 - cy1;
			break;
		case ALIGN_LEFT_BOTTOM:
			__object_translate(child, ox1 - cx1, oy2 - cy2);
			rx1 += cx2 - cx1;
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_RIGHT_BOTTOM:
			__object_translate(child, ox2 - cx2, oy2 - cy2);
			rx2 -= cx2 - cx1;
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_LEFT_CENTER:
			__object_translate(child, ox1 - cx1, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			rx1 += cx2 - cx1;
			break;
		case ALIGN_TOP_CENTER:
			__object_translate(child, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, oy1 - cy1);
			ry1 += cy2 - cy1;
			break;
		case ALIGN_RIGHT_CENTER:
			__object_translate(child, ox2 - cx2, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			rx2 -= cx2 - cx1;
			break;
		case ALIGN_BOTTOM_CENTER:
			__object_translate(child, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, oy2 - cy2);
			ry2 -= cy2 - cy1;
			break;
		case ALIGN_HORIZONTAL_CENTER:
			__object_translate(child, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, 0);
			break;
		case ALIGN_VERTICAL_CENTER:
			__object_translate(child, 0, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			break;
		case ALIGN_CENTER:
			__object_translate(child, ox1 - cx1 + ((ox2 - ox1) - (cx2 - cx1)) / 2, oy1 - cy1 + ((oy2 - oy1) - (cy2 - cy1)) / 2);
			break;
		default:
			break;
		}
	}
	else if((child->alignment <= ALIGN_CENTER_FILL))
	{
		double w;
		double h;

		switch(child->alignment)
		{
		case ALIGN_LEFT_FILL:
			w = child->width * child->scalex;
			h = ry2 - ry1;
			__object_translate_fill(child, rx1, ry1, w, h);
			rx1 += w;
			break;
		case ALIGN_TOP_FILL:
			w = rx2 - rx1;
			h = child->height * child->scaley;
			__object_translate_fill(child, rx1, ry1, w, h);
			ry1 += h;
			break;
		case ALIGN_RIGHT_FILL:
			w = child->width * child->scalex;
			h = ry2 - ry1;
			__object_translate_fill(child, rx2 - w, ry1, w, h);
			rx2 -= w;
			break;
		case ALIGN_BOTTOM_FILL:
			w = rx2 - rx1;
			h = child->height * child->scaley;
			__object_translate_fill(child, rx1, ry2 - h, w, h);
			ry2 -= h;
			break;
		case ALIGN_HORIZONTAL_FILL:
			w = rx2 - rx1;
			h = child->height * child->scaley;
			__object_translate_fill(child, rx1, child->y, w, h);
			break;
		case ALIGN_VERTICAL_FILL:
			w = child->width * child->scalex;
			h = ry2 - ry1;
			__object_translate_fill(child, child->x, ry1, w, h);
			break;
		case ALIGN_CENTER_FILL:
			w = rx2 - rx1;
			h = ry2 - ry1;
			__object_translate_fill(child, rx1, ry1, w, h);
			rx1 += w;
			ry1 += h;
			break;
		default:
			break;
		}
	}

	lua_pushnumber(L, rx1);
	lua_pushnumber(L, ry1);
	lua_pushnumber(L, rx2);
	lua_pushnumber(L, ry2);
	return 4;
}

static const luaL_Reg m_object[] = {
	{"setSize",			m_set_size},
	{"getSize",			m_get_size},
	{"setX",			m_set_x},
	{"getX",			m_get_x},
	{"setY",			m_set_y},
	{"getY",			m_get_y},
	{"setPosition",		m_set_position},
	{"getPosition",		m_get_position},
	{"setRotation",		m_set_rotation},
	{"getRotation",		m_get_rotation},
	{"setScaleX",		m_set_scale_x},
	{"getScaleX",		m_get_scale_x},
	{"setScaleY",		m_set_scale_y},
	{"getScaleY",		m_get_scale_y},
	{"setScale",		m_set_scale},
	{"getScale",		m_get_scale},
	{"setAnchor",		m_set_archor},
	{"getAnchor",		m_get_archor},
	{"setAlpha",		m_set_alpha},
	{"getAlpha",		m_get_alpha},
	{"setAlignment",	m_set_alignment},
	{"getAlignment",	m_get_alignment},
	{"setVisible",		m_set_visible},
	{"getVisible",		m_get_visible},
	{"setTouchable",	m_set_touchable},
	{"getTouchable",	m_get_touchable},
	{"getMatrix",		m_get_matrix},
	{"hitTestPoint",	m_hit_test_point},
	{"bounds",			m_bounds},
	{"layout",			m_layout},
	{NULL,				NULL}
};

int luaopen_object(lua_State * L)
{
	luaL_newlib(L, l_object);
	luahelper_create_metatable(L, MT_NAME_OBJECT, m_object);
	return 1;
}
