/*
 * framework/event/l-event.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

static int l_event_pump(lua_State * L)
{
	struct event_t event;
	u32_t button;

	if(!pump_event(runtime_get()->__event_base, &event))
		return 0;

	switch(event.type)
	{
	/* unknown */
	case EVENT_TYPE_UNKNOWN:
		return 0;

	/* key */
	case EVENT_TYPE_KEY_DOWN:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_KEY_DOWN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.key_down.key);
		lua_setfield(L, -2, "key");
		return 1;

	case EVENT_TYPE_KEY_UP:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_KEY_UP);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.key_up.key);
		lua_setfield(L, -2, "key");
		return 1;

	/* mouse */
	case EVENT_TYPE_MOUSE_DOWN:
		button = event.e.mouse_down.button;
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_DOWN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.mouse_down.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.mouse_down.y);
		lua_setfield(L, -2, "y");
		lua_newtable(L);
		lua_pushboolean(L, (button & MOUSE_BUTTON_LEFT) != 0);
		lua_setfield(L, -2, "left");
		lua_pushboolean(L, (button & MOUSE_BUTTON_RIGHT) != 0);
		lua_setfield(L, -2, "right");
		lua_pushboolean(L, (button & MOUSE_BUTTON_MIDDLE) != 0);
		lua_setfield(L, -2, "middle");
		lua_setfield(L, -2, "button");
		return 1;

	case EVENT_TYPE_MOUSE_MOVE:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_MOVE);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.mouse_move.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.mouse_move.y);
		lua_setfield(L, -2, "y");
		return 1;

	case EVENT_TYPE_MOUSE_UP:
		button = event.e.mouse_up.button;
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_UP);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.mouse_up.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.mouse_up.y);
		lua_setfield(L, -2, "y");
		lua_newtable(L);
		lua_pushboolean(L, (button & MOUSE_BUTTON_LEFT) != 0);
		lua_setfield(L, -2, "left");
		lua_pushboolean(L, (button & MOUSE_BUTTON_RIGHT) != 0);
		lua_setfield(L, -2, "right");
		lua_pushboolean(L, (button & MOUSE_BUTTON_MIDDLE) != 0);
		lua_setfield(L, -2, "middle");
		lua_setfield(L, -2, "button");
		return 1;

	case EVENT_TYPE_MOUSE_WHEEL:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_MOUSE_WHEEL);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.mouse_wheel.dx);
		lua_setfield(L, -2, "dx");
		lua_pushnumber(L, event.e.mouse_wheel.dy);
		lua_setfield(L, -2, "dy");
		return 1;

	/* touch */
	case EVENT_TYPE_TOUCH_BEGIN:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCH_BEGIN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.touch_begin.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.touch_begin.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.touch_begin.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_TOUCH_MOVE:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCH_MOVE);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.touch_move.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.touch_move.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.touch_move.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_TOUCH_END:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCH_END);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.touch_end.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.touch_end.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.touch_end.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_JOYSTICK_LEFTSTICK:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_LEFTSTICK);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.joystick_left_stick.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.joystick_left_stick.y);
		lua_setfield(L, -2, "y");
		return 1;

	case EVENT_TYPE_JOYSTICK_RIGHTSTICK:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_RIGHTSTICK);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.joystick_right_stick.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.joystick_right_stick.y);
		lua_setfield(L, -2, "y");
		return 1;

	case EVENT_TYPE_JOYSTICK_LEFTTRIGGER:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_LEFTTRIGGER);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.joystick_left_trigger.value);
		lua_setfield(L, -2, "value");
		return 1;

	case EVENT_TYPE_JOYSTICK_RIGHTTRIGGER:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_RIGHTTRIGGER);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.joystick_right_trigger.value);
		lua_setfield(L, -2, "value");
		return 1;

	case EVENT_TYPE_JOYSTICK_BUTTONDOWN:
		button = event.e.joystick_button_down.button;
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_BUTTONDOWN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.joystick_button_down.button);
		lua_setfield(L, -2, "button");
		return 1;

	case EVENT_TYPE_JOYSTICK_BUTTONUP:
		button = event.e.joystick_button_up.button;
		lua_newtable(L);
		lua_pushstring(L, __TYPE_JOYSTICK_BUTTONUP);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.joystick_button_up.button);
		lua_setfield(L, -2, "button");
		return 1;

	default:
		return 0;
	}

	return 0;
}

static const luaL_Reg l_event[] = {
	{"pump",	l_event_pump},
	{NULL,		NULL}
};

int luaopen_event(lua_State * L)
{
	luaL_newlib(L, l_event);
	return 1;
}
