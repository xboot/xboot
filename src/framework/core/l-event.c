/*
 * framework/core/l-event.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

#include <input/input.h>
#include <framework/core/l-event.h>

static int l_event_new(lua_State * L)
{
	const char * type = luaL_checkstring(L, 1);
	if(!type)
		return 0;
	if(lua_istable(L, 2))
	{
		lua_pushvalue(L, 2);
		luahelper_deepcopy_table(L);
	}
	else
	{
		lua_newtable(L);
	}
	lua_pushstring(L, "virtual");
	lua_setfield(L, -2, "device");
	lua_pushstring(L, type);
	lua_setfield(L, -2, "type");
	lua_pushnumber(L, ktime_to_ns(ktime_get()));
	lua_setfield(L, -2, "time");
	return 1;
}

static int l_event_pump(lua_State * L)
{
	struct window_t * w = ((struct vmctx_t *)luahelper_vmctx(L))->w;
	struct event_t e;

	if(!window_is_active(w) || !window_pump_event(w, &e))
		return 0;

	switch(e.type)
	{
	case EVENT_TYPE_KEY_DOWN:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "key-down");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.key_down.key);
		lua_setfield(L, -2, "key");
		return 1;

	case EVENT_TYPE_KEY_UP:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "key-up");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.key_up.key);
		lua_setfield(L, -2, "key");
		return 1;

	case EVENT_TYPE_ROTARY_TURN:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "rotary-turn");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.rotary_turn.v);
		lua_setfield(L, -2, "v");
		return 1;

	case EVENT_TYPE_ROTARY_SWITCH:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "rotary-switch");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.rotary_switch.v);
		lua_setfield(L, -2, "v");
		return 1;

	case EVENT_TYPE_MOUSE_DOWN:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "mouse-down");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.mouse_down.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.mouse_down.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, e.e.mouse_down.button);
		lua_setfield(L, -2, "button");
		return 1;

	case EVENT_TYPE_MOUSE_MOVE:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "mouse-move");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.mouse_move.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.mouse_move.y);
		lua_setfield(L, -2, "y");
		return 1;

	case EVENT_TYPE_MOUSE_UP:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "mouse-up");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.mouse_up.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.mouse_up.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, e.e.mouse_up.button);
		lua_setfield(L, -2, "button");
		return 1;

	case EVENT_TYPE_MOUSE_WHEEL:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "mouse-wheel");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.mouse_wheel.dx);
		lua_setfield(L, -2, "dx");
		lua_pushinteger(L, e.e.mouse_wheel.dy);
		lua_setfield(L, -2, "dy");
		return 1;

	case EVENT_TYPE_TOUCH_BEGIN:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "touch-begin");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.touch_begin.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.touch_begin.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, e.e.touch_begin.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_TOUCH_MOVE:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "touch-move");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.touch_move.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.touch_move.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, e.e.touch_move.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_TOUCH_END:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "touch-end");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.touch_end.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.touch_end.y);
		lua_setfield(L, -2, "y");
		lua_pushinteger(L, e.e.touch_end.id);
		lua_setfield(L, -2, "id");
		return 1;

	case EVENT_TYPE_JOYSTICK_LEFTSTICK:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "joystick-left-stick");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.joystick_left_stick.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.joystick_left_stick.y);
		lua_setfield(L, -2, "y");
		return 1;

	case EVENT_TYPE_JOYSTICK_RIGHTSTICK:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "joystick-right-stick");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.joystick_right_stick.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, e.e.joystick_right_stick.y);
		lua_setfield(L, -2, "y");
		return 1;

	case EVENT_TYPE_JOYSTICK_LEFTTRIGGER:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "joystick-left-trigger");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.joystick_left_trigger.v);
		lua_setfield(L, -2, "v");
		return 1;

	case EVENT_TYPE_JOYSTICK_RIGHTTRIGGER:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "joystick-right-trigger");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.joystick_right_trigger.v);
		lua_setfield(L, -2, "v");
		return 1;

	case EVENT_TYPE_JOYSTICK_BUTTONDOWN:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "joystick-button-down");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.joystick_button_down.button);
		lua_setfield(L, -2, "button");
		return 1;

	case EVENT_TYPE_JOYSTICK_BUTTONUP:
		lua_newtable(L);
		lua_pushstring(L, ((struct input_t *)e.device)->name);
		lua_setfield(L, -2, "device");
		lua_pushstring(L, "joystick-button-up");
		lua_setfield(L, -2, "type");
		lua_pushnumber(L, ktime_to_ns(e.timestamp));
		lua_setfield(L, -2, "time");
		lua_pushinteger(L, e.e.joystick_button_up.button);
		lua_setfield(L, -2, "button");
		return 1;

	default:
		break;
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
	return 1;
}
