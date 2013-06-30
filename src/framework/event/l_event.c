/*
 * framework/event/l_event.c
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

#include <framework/event/l_event.h>

#define __TYPE_KEY_DOWN				"keydown"
#define __TYPE_KEY_UP				"keyup"
#define __TYPE_MOUSE_DOWN			"mousedown"
#define __TYPE_MOUSE_MOVE			"mousemove"
#define __TYPE_MOUSE_UP				"mouseup"
#define __TYPE_MOUSE_WHEEL			"mousewheel"
#define __TYPE_TOUCHES_BEGAN		"touchesbegan"
#define __TYPE_TOUCHES_MOVE			"touchesmove"
#define __TYPE_TOUCHES_END			"touchesend"
#define __TYPE_TOUCHES_CANCEL		"touchescancel"

static int l_event_peek(lua_State * L)
{
	struct event_t event;
	u32_t button;

	if(!peek_event(runtime_get()->__event_base, &event))
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
		lua_pushnumber(L, event.e.key.code);
		lua_setfield(L, -2, "code");
		return 1;

	case EVENT_TYPE_KEY_UP:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_KEY_UP);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.key.code);
		lua_setfield(L, -2, "code");
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
		lua_pushnumber(L, event.e.mouse_wheel.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.mouse_wheel.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.mouse_wheel.delta);
		lua_setfield(L, -2, "delta");
		return 1;

	/* touches */
	case EVENT_TYPE_TOUCHES_BEGAN:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCHES_BEGAN);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.touches.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.touches.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.touches.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_TOUCHES_MOVE:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCHES_MOVE);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.touches.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.touches.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.touches.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_TOUCHES_END:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCHES_END);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.touches.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.touches.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.touches.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_TOUCHES_CANCEL:
		lua_newtable(L);
		lua_pushstring(L, __TYPE_TOUCHES_CANCEL);
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, event.timestamp);
		lua_setfield(L, -2, "time");
		lua_pushnumber(L, event.e.touches.x);
		lua_setfield(L, -2, "x");
		lua_pushnumber(L, event.e.touches.y);
		lua_setfield(L, -2, "y");
		lua_pushnumber(L, event.e.touches.id);
		lua_setfield(L, -2, "id");
		return 1;

	default:
		return 0;
	}

	return 0;
}

static const luaL_Reg l_event[] = {
	{"peek",		l_event_peek},
	{NULL, NULL}
};

int luaopen_event(lua_State * L)
{
	luaL_newlib(L, l_event);
	return 1;
}
