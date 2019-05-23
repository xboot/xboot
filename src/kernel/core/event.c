/*
 * kernel/core/event.c
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

#include <xboot.h>
#include <input/input.h>
#include <xboot/window.h>
#include <xboot/event.h>

void push_event_key_down(void * device, u32_t key)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_KEY_DOWN;
	e.e.key_down.key = key;
	push_event(&e);
}

void push_event_key_up(void * device, u32_t key)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_KEY_UP;
	e.e.key_up.key = key;
	push_event(&e);
}

void push_event_rotary_turn(void * device, s32_t v)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_ROTARY_TURN;
	e.e.rotary_turn.v = v;
	push_event(&e);
}

void push_event_rotary_switch(void * device, s32_t v)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_ROTARY_SWITCH;
	e.e.rotary_switch.v = v;
	push_event(&e);
}

void push_event_mouse_button_down(void * device, s32_t x, s32_t y, u32_t button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_DOWN;
	e.e.mouse_down.x = x;
	e.e.mouse_down.y = y;
	e.e.mouse_down.button = button;
	push_event(&e);
}

void push_event_mouse_button_up(void * device, s32_t x, s32_t y, u32_t button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_UP;
	e.e.mouse_up.x = x;
	e.e.mouse_up.y = y;
	e.e.mouse_up.button = button;
	push_event(&e);
}

void push_event_mouse_move(void * device, s32_t x, s32_t y)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_MOVE;
	e.e.mouse_move.x = x;
	e.e.mouse_move.y = y;
	push_event(&e);
}

void push_event_mouse_wheel(void * device, s32_t dx, s32_t dy)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_WHEEL;
	e.e.mouse_wheel.dx = dx;
	e.e.mouse_wheel.dy = dy;
	push_event(&e);
}

void push_event_touch_begin(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_TOUCH_BEGIN;
	e.e.touch_begin.x = x;
	e.e.touch_begin.y = y;
	e.e.touch_begin.id = id;
	push_event(&e);
}

void push_event_touch_move(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_TOUCH_MOVE;
	e.e.touch_move.x = x;
	e.e.touch_move.y = y;
	e.e.touch_move.id = id;
	push_event(&e);
}

void push_event_touch_end(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_TOUCH_END;
	e.e.touch_end.x = x;
	e.e.touch_end.y = y;
	e.e.touch_end.id = id;
	push_event(&e);
}

void push_event_joystick_left_stick(void * device, s32_t x, s32_t y)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_LEFTSTICK;
	e.e.joystick_left_stick.x = x;
	e.e.joystick_left_stick.y = y;
	push_event(&e);
}

void push_event_joystick_right_stick(void * device, s32_t x, s32_t y)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_RIGHTSTICK;
	e.e.joystick_right_stick.x = x;
	e.e.joystick_right_stick.y = y;
	push_event(&e);
}

void push_event_joystick_left_trigger(void * device, s32_t v)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_LEFTTRIGGER;
	e.e.joystick_left_trigger.v = v;
	push_event(&e);
}

void push_event_joystick_right_trigger(void * device, s32_t v)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_RIGHTTRIGGER;
	e.e.joystick_left_trigger.v = v;
	push_event(&e);
}

void push_event_joystick_button_down(void * device, u32_t button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_BUTTONDOWN;
	e.e.joystick_button_down.button = button;
	push_event(&e);
}

void push_event_joystick_button_up(void * device, u32_t button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_BUTTONUP;
	e.e.joystick_button_down.button = button;
	push_event(&e);
}
