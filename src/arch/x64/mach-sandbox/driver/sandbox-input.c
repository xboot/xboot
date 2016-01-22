/*
 * driver/sandbox-input.c
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

#include <xboot.h>
#include <sandbox-input.h>

static void input_init(struct input_t * input)
{
}

static void input_exit(struct input_t * input)
{
}

static int input_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static void input_suspend(struct input_t * input)
{
}

static void input_resume(struct input_t * input)
{
}

static void cb_key_down(void * device, unsigned int key)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_KEY_DOWN;
	event.e.key_down.key = key;
	push_event(&event);
}

static void cb_key_up(void * device, unsigned int key)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_KEY_UP;
	event.e.key_up.key = key;
	push_event(&event);
}

static void cb_mouse_down(void * device, int x, int y, unsigned int button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_DOWN;
	event.e.mouse_down.x = x;
	event.e.mouse_down.y = y;
	event.e.mouse_down.button = button;
	push_event(&event);
}

static void cb_mouse_move(void * device, int x, int y)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_MOVE;
	event.e.mouse_move.x = x;
	event.e.mouse_move.y = y;
	push_event(&event);
}

static void cb_mouse_up(void * device, int x, int y, unsigned int button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_UP;
	event.e.mouse_down.x = x;
	event.e.mouse_down.y = y;
	event.e.mouse_down.button = button;
	push_event(&event);
}

static void cb_mouse_wheel(void * device, int dx, int dy)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_WHEEL;
	event.e.mouse_wheel.dx = dx;
	event.e.mouse_wheel.dy = dy;
	push_event(&event);
}

static void cb_touch_begin(void * device, int x, int y, unsigned int id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCH_BEGIN;
	event.e.touch_begin.x = x;
	event.e.touch_begin.y = y;
	event.e.touch_begin.id = id;
	push_event(&event);
}

static void cb_touch_move(void * device, int x, int y, unsigned int id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCH_MOVE;
	event.e.touch_move.x = x;
	event.e.touch_move.y = y;
	event.e.touch_move.id = id;
	push_event(&event);
}

static void cb_touch_end(void * device, int x, int y, unsigned int id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCH_END;
	event.e.touch_end.x = x;
	event.e.touch_end.y = y;
	event.e.touch_end.id = id;
	push_event(&event);
}

static void cb_joystick_left_stick(void * device, int x, int y)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_LEFTSTICK;
	event.e.joystick_left_stick.x = x;
	event.e.joystick_left_stick.y = y;
	push_event(&event);
}

static void cb_joystick_right_stick(void * device, int x, int y)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_RIGHTSTICK;
	event.e.joystick_right_stick.x = x;
	event.e.joystick_right_stick.y = y;
	push_event(&event);
}

static void cb_joystick_left_trigger(void * device, int v)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_LEFTTRIGGER;
	event.e.joystick_left_trigger.v = v;
	push_event(&event);
}

static void cb_joystick_right_trigger(void * device, int v)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_RIGHTTRIGGER;
	event.e.joystick_left_trigger.v = v;
	push_event(&event);
}

static void cb_joystick_button_down(void * device, unsigned int button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_BUTTONDOWN;
	event.e.joystick_button_down.button = button;
	push_event(&event);
}

static void cb_joystick_button_up(void * device, unsigned int button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_BUTTONUP;
	event.e.joystick_button_down.button = button;
	push_event(&event);
}

static bool_t sandbox_register_input(struct resource_t * res)
{
	struct sandbox_input_data_t * rdat = (struct sandbox_input_data_t *)res->data;
	struct input_t * input;
	char name[64];

	input = malloc(sizeof(struct input_t));
	if(!input)
		return FALSE;

	switch(rdat->type)
	{
	case INPUT_TYPE_KEYBOARD:
		sandbox_sdl_event_set_key_callback(input,
				cb_key_down,
				cb_key_up);
		break;

	case INPUT_TYPE_MOUSE:
		sandbox_sdl_event_set_mouse_callback(input,
				cb_mouse_down,
				cb_mouse_move,
				cb_mouse_up,
				cb_mouse_wheel);
		break;

	case INPUT_TYPE_TOUCHSCREEN:
		sandbox_sdl_event_set_touch_callback(input,
				cb_touch_begin,
				cb_touch_move,
				cb_touch_end);
		break;

	case INPUT_TYPE_JOYSTICK:
		sandbox_sdl_event_set_joystick_callback(input,
				cb_joystick_left_stick,
				cb_joystick_right_stick,
				cb_joystick_left_trigger,
				cb_joystick_right_trigger,
				cb_joystick_button_down,
				cb_joystick_button_up);
		break;

	default:
		free(input);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	input->name = strdup(name);
	input->type = rdat->type;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume = input_resume,
	input->priv = res;

	if(register_input(input))
		return TRUE;

	free(input->name);
	free(input);
	return FALSE;
}

static bool_t sandbox_unregister_input(struct resource_t * res)
{
	struct input_t * input;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	input = search_input(name);
	if(!input)
		return FALSE;

	if(!unregister_input(input))
		return FALSE;

	free(input->name);
	free(input);
	return TRUE;
}

static __init void sandbox_input_init(void)
{
	sandbox_sdl_event_init();
	resource_for_each_with_name("sandbox-input", sandbox_register_input);
}

static __exit void sandbox_input_exit(void)
{
	sandbox_sdl_event_exit();
	resource_for_each_with_name("sandbox-input", sandbox_unregister_input);
}

device_initcall(sandbox_input_init);
device_exitcall(sandbox_input_exit);
