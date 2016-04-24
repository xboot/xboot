/*
 * framework/event/l-event.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#include <framework/event/l-event.h>

#define __TYPE_KEY_DOWN					"KeyDown"
#define __TYPE_KEY_UP					"KeyUp"
#define __TYPE_MOUSE_DOWN				"MouseDown"
#define __TYPE_MOUSE_MOVE				"MouseMove"
#define __TYPE_MOUSE_UP					"MouseUp"
#define __TYPE_MOUSE_WHEEL				"MouseWheel"
#define __TYPE_TOUCH_BEGIN				"TouchBegin"
#define __TYPE_TOUCH_MOVE				"TouchMove"
#define __TYPE_TOUCH_END				"TouchEnd"
#define __TYPE_JOYSTICK_LEFTSTICK		"JoystickLeftStick"
#define __TYPE_JOYSTICK_RIGHTSTICK		"JoystickRightStick"
#define __TYPE_JOYSTICK_LEFTTRIGGER		"JoystickLeftTrigger"
#define __TYPE_JOYSTICK_RIGHTTRIGGER	"JoystickRightTrigger"
#define __TYPE_JOYSTICK_BUTTONDOWN		"JoystickButtonDown"
#define __TYPE_JOYSTICK_BUTTONUP		"JoystickButtonUp"
#define __TYPE_ENTER_FRAME				"EnterFrame"
#define __TYPE_ANIMATE_COMPLETE			"AnimateComplete"

static int l_event_new(lua_State * L)
{
	const char * type = luaL_checkstring(L, 1);
	if(!type)
		return 0;
	lua_newtable(L);
	lua_pushstring(L, type);
	lua_setfield(L, -2, "type");
	if(lua_type(L, 2) == LUA_TTABLE)
	{
		lua_pushvalue(L, 2);
		luahelper_deepcopy_table(L);
		lua_setfield(L, -2, "ext");
	}
	lua_pushboolean(L, 0);
	lua_setfield(L, -2, "stop");
	return 1;
}

static int l_event_pump(lua_State * L)
{
	struct event_t event;

	if(!pump_event(runtime_get()->__event_base, &event))
		return 0;

	switch(event.type)
	{
	/* key */
	case EVENT_TYPE_KEY_DOWN:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_KEY_DOWN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.key_down.key);
		lua_setfield(L, -2, "key");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_KEY_UP:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_KEY_UP);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.key_up.key);
		lua_setfield(L, -2, "key");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	/* mouse */
	case EVENT_TYPE_MOUSE_DOWN:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_DOWN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.mouse_down.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.mouse_down.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, event.e.mouse_down.button);
		lua_setfield(L, -2, "button");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_MOUSE_MOVE:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_MOVE);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.mouse_move.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.mouse_move.y);
		lua_setfield(L, -2, "y");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_MOUSE_UP:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_UP);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.mouse_up.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.mouse_up.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, event.e.mouse_up.button);
		lua_setfield(L, -2, "button");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_MOUSE_WHEEL:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_WHEEL);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.mouse_wheel.dx);
		lua_setfield(L, -2, "dx");
		lua_pushinteger(L, event.e.mouse_wheel.dy);
		lua_setfield(L, -2, "dy");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	/* touch */
	case EVENT_TYPE_TOUCH_BEGIN:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCH_BEGIN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.touch_begin.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.touch_begin.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, event.e.touch_begin.id);
		lua_setfield(L, -2, "id");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_TOUCH_MOVE:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCH_MOVE);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.touch_move.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.touch_move.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, event.e.touch_move.id);
		lua_setfield(L, -2, "id");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_TOUCH_END:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCH_END);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.touch_end.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.touch_end.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, event.e.touch_end.id);
		lua_setfield(L, -2, "id");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_JOYSTICK_LEFTSTICK:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_LEFTSTICK);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.joystick_left_stick.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.joystick_left_stick.y);
		lua_setfield(L, -2, "y");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_JOYSTICK_RIGHTSTICK:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_RIGHTSTICK);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.joystick_right_stick.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, event.e.joystick_right_stick.y);
		lua_setfield(L, -2, "y");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_JOYSTICK_LEFTTRIGGER:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_LEFTTRIGGER);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.joystick_left_trigger.v);
		lua_setfield(L, -2, "v");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_JOYSTICK_RIGHTTRIGGER:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_RIGHTTRIGGER);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.joystick_right_trigger.v);
		lua_setfield(L, -2, "v");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_JOYSTICK_BUTTONDOWN:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_BUTTONDOWN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.joystick_button_down.button);
		lua_setfield(L, -2, "button");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	case EVENT_TYPE_JOYSTICK_BUTTONUP:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_BUTTONUP);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(event.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, event.e.joystick_button_up.button);
		lua_setfield(L, -2, "button");
		lua_pushboolean(L, 0);
		lua_setfield(L, -2, "stop");
		return 1;

	default:
		return 0;
	}

	return 0;
}

static const luaL_Reg l_event[] = {
	{"new",		l_event_new},
	{"pump",	l_event_pump},
	{NULL,		NULL}
};

int luaopen_event(lua_State * L)
{
	luaL_newlib(L, l_event);
	luahelper_set_strfield(L, "KEY_DOWN",				__TYPE_KEY_DOWN);
	luahelper_set_strfield(L, "KEY_UP",					__TYPE_KEY_UP);
	luahelper_set_strfield(L, "MOUSE_DOWN",				__TYPE_MOUSE_DOWN);
	luahelper_set_strfield(L, "MOUSE_MOVE",				__TYPE_MOUSE_MOVE);
	luahelper_set_strfield(L, "MOUSE_UP",				__TYPE_MOUSE_UP);
	luahelper_set_strfield(L, "MOUSE_WHEEL",			__TYPE_MOUSE_WHEEL);
	luahelper_set_strfield(L, "TOUCH_BEGIN",			__TYPE_TOUCH_BEGIN);
	luahelper_set_strfield(L, "TOUCH_MOVE",				__TYPE_TOUCH_MOVE);
	luahelper_set_strfield(L, "TOUCH_END",				__TYPE_TOUCH_END);
	luahelper_set_strfield(L, "JOYSTICK_LEFTSTICK",		__TYPE_JOYSTICK_LEFTSTICK);
	luahelper_set_strfield(L, "JOYSTICK_RIGHTSTICK",	__TYPE_JOYSTICK_RIGHTSTICK);
	luahelper_set_strfield(L, "JOYSTICK_LEFTTRIGGER",	__TYPE_JOYSTICK_LEFTTRIGGER);
	luahelper_set_strfield(L, "JOYSTICK_RIGHTTRIGGER",	__TYPE_JOYSTICK_RIGHTTRIGGER);
	luahelper_set_strfield(L, "JOYSTICK_BUTTONDOWN",	__TYPE_JOYSTICK_BUTTONDOWN);
	luahelper_set_strfield(L, "JOYSTICK_BUTTONUP",		__TYPE_JOYSTICK_BUTTONUP);
	luahelper_set_strfield(L, "ENTER_FRAME",			__TYPE_ENTER_FRAME);
	luahelper_set_strfield(L, "ANIMATE_COMPLETE",		__TYPE_ANIMATE_COMPLETE);
	return 1;
}
