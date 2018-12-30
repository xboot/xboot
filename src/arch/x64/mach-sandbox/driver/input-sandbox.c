/*
 * driver/input-sandbox.c
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
#include <sandbox.h>

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

static int input_sandbox_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * input_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct input_t * input;
	struct device_t * dev;
	enum input_type_t type;
	char * t = dt_read_string(n, "type", NULL);

	if(strcmp(t, "keyboard") == 0)
		type = INPUT_TYPE_KEYBOARD;
	else if(strcmp(t, "mouse") == 0)
		type = INPUT_TYPE_MOUSE;
	else if(strcmp(t, "touchscreen") == 0)
		type = INPUT_TYPE_TOUCHSCREEN;
	else if(strcmp(t, "joystick") == 0)
		type = INPUT_TYPE_JOYSTICK;
	else
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
		return NULL;

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = type;
	input->ioctl = input_sandbox_ioctl;
	input->priv = NULL;

	switch(input->type)
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
		break;
	}

	if(!register_input(&dev, input))
	{
		switch(input->type)
		{
		case INPUT_TYPE_KEYBOARD:
			sandbox_sdl_event_set_key_callback(0, 0, 0);
			break;
		case INPUT_TYPE_MOUSE:
			sandbox_sdl_event_set_mouse_callback(0, 0, 0, 0, 0);
			break;
		case INPUT_TYPE_TOUCHSCREEN:
			sandbox_sdl_event_set_touch_callback(0, 0, 0, 0);
			break;
		case INPUT_TYPE_JOYSTICK:
			sandbox_sdl_event_set_joystick_callback(0, 0, 0, 0, 0, 0, 0);
			break;
		default:
			break;
		}

		free_device_name(input->name);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void input_sandbox_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;

	if(input && unregister_input(input))
	{
		switch(input->type)
		{
		case INPUT_TYPE_KEYBOARD:
			sandbox_sdl_event_set_key_callback(0, 0, 0);
			break;
		case INPUT_TYPE_MOUSE:
			sandbox_sdl_event_set_mouse_callback(0, 0, 0, 0, 0);
			break;
		case INPUT_TYPE_TOUCHSCREEN:
			sandbox_sdl_event_set_touch_callback(0, 0, 0, 0);
			break;
		case INPUT_TYPE_JOYSTICK:
			sandbox_sdl_event_set_joystick_callback(0, 0, 0, 0, 0, 0, 0);
			break;
		default:
			break;
		}

		free_device_name(input->name);
		free(input);
	}
}

static void input_sandbox_suspend(struct device_t * dev)
{
}

static void input_sandbox_resume(struct device_t * dev)
{
}

static struct driver_t input_sandbox = {
	.name		= "input-sandbox",
	.probe		= input_sandbox_probe,
	.remove		= input_sandbox_remove,
	.suspend	= input_sandbox_suspend,
	.resume		= input_sandbox_resume,
};

static __init void input_sandbox_driver_init(void)
{
	register_driver(&input_sandbox);
}

static __exit void input_sandbox_driver_exit(void)
{
	unregister_driver(&input_sandbox);
}

driver_initcall(input_sandbox_driver_init);
driver_exitcall(input_sandbox_driver_exit);
