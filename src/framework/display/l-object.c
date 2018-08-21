/*
 * framework/display/l-object.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <cairo.h>
#include <cairoint.h>
#include <framework/display/l-display.h>

static void __object_translate(struct lobject_t * object, double dx, double dy)
{
	object->x = object->x + dx;
	object->y = object->y + dy;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
}

static void __object_translate_fill(struct lobject_t * object, double x, double y, double w, double h)
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
	object->__obj_matrix_valid = 0;
}

static inline cairo_matrix_t * __get_obj_matrix(struct lobject_t * object)
{
	cairo_matrix_t * m = &object->__obj_matrix;
	if(!object->__obj_matrix_valid)
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
		object->__obj_matrix_valid = 1;
	}
	return m;
}

static int l_object_new(lua_State * L)
{
	struct lobject_t * object = lua_newuserdata(L, sizeof(struct lobject_t));
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

	object->__obj_matrix_valid = 1;
	cairo_matrix_init_identity(&object->__obj_matrix);
	cairo_matrix_init_identity(&object->__transform_matrix);

	luaL_setmetatable(L, MT_OBJECT);
	return 1;
}

static const luaL_Reg l_object[] = {
	{"new",	l_object_new},
	{NULL,	NULL}
};

static int m_set_size(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double w = luaL_checknumber(L, 2);
	double h = luaL_checknumber(L, 3);
	object->width = w;
	object->height = h;
	return 0;
}

static int m_get_size(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->width);
	lua_pushnumber(L, object->height);
	return 2;
}

static int m_set_x(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	object->x = x;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_x(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->x);
	return 1;
}

static int m_set_y(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double y = luaL_checknumber(L, 2);
	object->y = y;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_y(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->y);
	return 1;
}

static int m_set_position(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	object->x = x;
	object->y = y;
	object->__translate = ((object->x != 0) || (object->y != 0)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_position(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->x);
	lua_pushnumber(L, object->y);
	return 2;
}

static int m_set_rotation(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double rotation = luaL_checknumber(L, 2);
	object->rotation = rotation * (M_PI / 180.0);
	while(object->rotation < 0)
		object->rotation = object->rotation + (M_PI * 2);
	while(object->rotation > (M_PI * 2))
		object->rotation = object->rotation - (M_PI * 2);
	object->__rotate = (object->rotation != 0) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_rotation(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->rotation / (M_PI / 180.0));
	return 1;
}

static int m_set_scale_x(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	object->scalex = x;
	object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_scale_x(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->scalex);
	return 1;
}

static int m_set_scale_y(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double y = luaL_checknumber(L, 2);
	object->scaley = y;
	object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_scale_y(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->scaley);
	return 1;
}

static int m_set_scale(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	object->scalex = x;
	object->scaley = y;
	object->__scale = ((object->scalex != 1) || (object->scaley != 1)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_scale(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->scalex);
	lua_pushnumber(L, object->scaley);
	return 2;
}

static int m_set_archor(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	object->anchorx = x;
	object->anchory = y;
	object->__anchor = ((object->anchorx != 0) || (object->anchory != 0)) ? 1 : 0;
	object->__obj_matrix_valid = 0;
	return 0;
}

static int m_get_archor(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->anchorx);
	lua_pushnumber(L, object->anchory);
	return 2;
}

static int m_set_alpha(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double alpha = luaL_checknumber(L, 2);
	object->alpha = alpha;
	return 0;
}

static int m_get_alpha(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushnumber(L, object->alpha);
	return 1;
}

static int m_set_alignment(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	object->alignment = (enum alignment_t)luaL_checkinteger(L, 2);
	return 0;
}

static int m_get_alignment(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushinteger(L, object->alignment);
	return 1;
}

static int m_set_visible(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	object->visible = lua_toboolean(L, 2) ? 1 : 0;
	return 0;
}

static int m_get_visible(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushboolean(L, object->visible);
	return 1;
}

static int m_set_touchable(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	object->touchable = lua_toboolean(L, 2) ? 1 : 0;
	return 0;
}

static int m_get_touchable(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	lua_pushboolean(L, object->touchable);
	return 1;
}

static int m_init_transform_matrix(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	memcpy(&object->__transform_matrix, __get_obj_matrix(object), sizeof(cairo_matrix_t));
	return 0;
}

static int m_update_transform_matrix(lua_State * L)
{
	struct lobject_t * obj1 = luaL_checkudata(L, 1, MT_OBJECT);
	struct lobject_t * obj2 = luaL_checkudata(L, 2, MT_OBJECT);
	cairo_matrix_multiply(&obj1->__transform_matrix, &obj1->__transform_matrix, __get_obj_matrix(obj2));
	return 0;
}

static int m_get_transform_matrix(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	cairo_matrix_t * matrix = lua_newuserdata(L, sizeof(cairo_matrix_t));
	memcpy(matrix, &object->__transform_matrix, sizeof(cairo_matrix_t));
	luaL_setmetatable(L, MT_MATRIX);
	return 1;
}

static int m_global_to_local(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_invert(&object->__transform_matrix);
	cairo_matrix_transform_point(&object->__transform_matrix, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_local_to_global(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_transform_point(&object->__transform_matrix, &x, &y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

static int m_hit_test_point(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x = luaL_checknumber(L, 2);
	double y = luaL_checknumber(L, 3);
	cairo_matrix_invert(&object->__transform_matrix);
	cairo_matrix_transform_point(&object->__transform_matrix, &x, &y);
	lua_pushboolean(L, ((x >= 0) && (y >= 0) && (x <= object->width) && (y <= object->height)) ? 1 : 0);
	return 1;
}

static int m_bounds(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	double x1 = 0;
	double y1 = 0;
	double x2 = object->width;
	double y2 = object->height;
	_cairo_matrix_transform_bounding_box(&object->__transform_matrix, &x1, &y1, &x2, &y2, NULL);
	lua_pushnumber(L, x1);
	lua_pushnumber(L, y1);
	lua_pushnumber(L, x2 - x1);
	lua_pushnumber(L, y2 - y1);
	return 4;
}

static int m_layout(lua_State * L)
{
	struct lobject_t * object = luaL_checkudata(L, 1, MT_OBJECT);
	struct lobject_t * child = luaL_checkudata(L, 2, MT_OBJECT);
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
		_cairo_matrix_transform_bounding_box(__get_obj_matrix(child), &cx1, &cy1, &cx2, &cy2, NULL);

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
	{"setSize",					m_set_size},
	{"getSize",					m_get_size},
	{"setX",					m_set_x},
	{"getX",					m_get_x},
	{"setY",					m_set_y},
	{"getY",					m_get_y},
	{"setPosition",				m_set_position},
	{"getPosition",				m_get_position},
	{"setRotation",				m_set_rotation},
	{"getRotation",				m_get_rotation},
	{"setScaleX",				m_set_scale_x},
	{"getScaleX",				m_get_scale_x},
	{"setScaleY",				m_set_scale_y},
	{"getScaleY",				m_get_scale_y},
	{"setScale",				m_set_scale},
	{"getScale",				m_get_scale},
	{"setAnchor",				m_set_archor},
	{"getAnchor",				m_get_archor},
	{"setAlpha",				m_set_alpha},
	{"getAlpha",				m_get_alpha},
	{"setAlignment",			m_set_alignment},
	{"getAlignment",			m_get_alignment},
	{"setVisible",				m_set_visible},
	{"getVisible",				m_get_visible},
	{"setTouchable",			m_set_touchable},
	{"getTouchable",			m_get_touchable},
	{"initTransormMatrix",		m_init_transform_matrix},
	{"upateTransformMatrix",	m_update_transform_matrix},
	{"getTransformMatrix",		m_get_transform_matrix},
	{"globalToLocal",			m_global_to_local},
	{"localToGlobal",			m_local_to_global},
	{"hitTestPoint",			m_hit_test_point},
	{"bounds",					m_bounds},
	{"layout",					m_layout},
	{NULL,						NULL}
};

int luaopen_object(lua_State * L)
{
	luaL_newlib(L, l_object);
	/* enum alignment_t */
	luahelper_set_intfield(L, "ALIGN_NONE", 				ALIGN_NONE);
	luahelper_set_intfield(L, "ALIGN_LEFT", 				ALIGN_LEFT);
	luahelper_set_intfield(L, "ALIGN_TOP", 					ALIGN_TOP);
	luahelper_set_intfield(L, "ALIGN_RIGHT", 				ALIGN_RIGHT);
	luahelper_set_intfield(L, "ALIGN_BOTTOM", 				ALIGN_BOTTOM);
	luahelper_set_intfield(L, "ALIGN_LEFT_TOP", 			ALIGN_LEFT_TOP);
	luahelper_set_intfield(L, "ALIGN_RIGHT_TOP", 			ALIGN_RIGHT_TOP);
	luahelper_set_intfield(L, "ALIGN_LEFT_BOTTOM", 			ALIGN_LEFT_BOTTOM);
	luahelper_set_intfield(L, "ALIGN_RIGHT_BOTTOM", 		ALIGN_RIGHT_BOTTOM);
	luahelper_set_intfield(L, "ALIGN_LEFT_CENTER", 			ALIGN_LEFT_CENTER);
	luahelper_set_intfield(L, "ALIGN_TOP_CENTER", 			ALIGN_TOP_CENTER);
	luahelper_set_intfield(L, "ALIGN_RIGHT_CENTER", 		ALIGN_RIGHT_CENTER);
	luahelper_set_intfield(L, "ALIGN_BOTTOM_CENTER",		ALIGN_BOTTOM_CENTER);
	luahelper_set_intfield(L, "ALIGN_HORIZONTAL_CENTER",	ALIGN_HORIZONTAL_CENTER);
	luahelper_set_intfield(L, "ALIGN_VERTICAL_CENTER", 		ALIGN_VERTICAL_CENTER);
	luahelper_set_intfield(L, "ALIGN_CENTER", 				ALIGN_CENTER);
	luahelper_set_intfield(L, "ALIGN_LEFT_FILL", 			ALIGN_LEFT_FILL);
	luahelper_set_intfield(L, "ALIGN_TOP_FILL", 			ALIGN_TOP_FILL);
	luahelper_set_intfield(L, "ALIGN_RIGHT_FILL", 			ALIGN_RIGHT_FILL);
	luahelper_set_intfield(L, "ALIGN_BOTTOM_FILL", 			ALIGN_BOTTOM_FILL);
	luahelper_set_intfield(L, "ALIGN_HORIZONTAL_FILL", 		ALIGN_HORIZONTAL_FILL);
	luahelper_set_intfield(L, "ALIGN_VERTICAL_FILL", 		ALIGN_VERTICAL_FILL);
	luahelper_set_intfield(L, "ALIGN_CENTER_FILL", 			ALIGN_CENTER_FILL);
	luahelper_create_metatable(L, MT_OBJECT, m_object);
	return 1;
}
