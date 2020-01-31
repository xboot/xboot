/*
 * driver/input-sandbox.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <sandbox.h>

struct input_sandbox_pdata_t {
	void * ctx;
};

static void cb_key_down(void * device, unsigned int key)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_KEY_DOWN;
	e.e.key_down.key = key;
	push_event(&e);
}

static void cb_key_up(void * device, unsigned int key)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_KEY_UP;
	e.e.key_up.key = key;
	push_event(&e);
}

static void cb_mouse_down(void * device, int x, int y, unsigned int button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_DOWN;
	e.e.mouse_down.x = x;
	e.e.mouse_down.y = y;
	e.e.mouse_down.button = button;
	push_event(&e);
}

static void cb_mouse_move(void * device, int x, int y)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_MOVE;
	e.e.mouse_move.x = x;
	e.e.mouse_move.y = y;
	push_event(&e);
}

static void cb_mouse_up(void * device, int x, int y, unsigned int button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_UP;
	e.e.mouse_down.x = x;
	e.e.mouse_down.y = y;
	e.e.mouse_down.button = button;
	push_event(&e);
}

static void cb_mouse_wheel(void * device, int dx, int dy)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_MOUSE_WHEEL;
	e.e.mouse_wheel.dx = dx;
	e.e.mouse_wheel.dy = dy;
	push_event(&e);
}

static void cb_touch_begin(void * device, int x, int y, unsigned int id)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_TOUCH_BEGIN;
	e.e.touch_begin.x = x;
	e.e.touch_begin.y = y;
	e.e.touch_begin.id = id;
	push_event(&e);
}

static void cb_touch_move(void * device, int x, int y, unsigned int id)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_TOUCH_MOVE;
	e.e.touch_move.x = x;
	e.e.touch_move.y = y;
	e.e.touch_move.id = id;
	push_event(&e);
}

static void cb_touch_end(void * device, int x, int y, unsigned int id)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_TOUCH_END;
	e.e.touch_end.x = x;
	e.e.touch_end.y = y;
	e.e.touch_end.id = id;
	push_event(&e);
}

static void cb_joystick_left_stick(void * device, int x, int y)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_LEFTSTICK;
	e.e.joystick_left_stick.x = x;
	e.e.joystick_left_stick.y = y;
	push_event(&e);
}

static void cb_joystick_right_stick(void * device, int x, int y)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_RIGHTSTICK;
	e.e.joystick_right_stick.x = x;
	e.e.joystick_right_stick.y = y;
	push_event(&e);
}

static void cb_joystick_left_trigger(void * device, int v)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_LEFTTRIGGER;
	e.e.joystick_left_trigger.v = v;
	push_event(&e);
}

static void cb_joystick_right_trigger(void * device, int v)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_RIGHTTRIGGER;
	e.e.joystick_left_trigger.v = v;
	push_event(&e);
}

static void cb_joystick_button_down(void * device, unsigned int button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_BUTTONDOWN;
	e.e.joystick_button_down.button = button;
	push_event(&e);
}

static void cb_joystick_button_up(void * device, unsigned int button)
{
	struct event_t e;

	e.device = device;
	e.type = EVENT_TYPE_JOYSTICK_BUTTONUP;
	e.e.joystick_button_down.button = button;
	push_event(&e);
}

static int input_sandbox_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	struct input_sandbox_pdata_t * pdat = (struct input_sandbox_pdata_t *)input->priv;
	int * p = arg;

	switch(shash(cmd))
	{
	case 0x431aa221: /* "mouse-set-range" */
		if(p)
		{
			sandbox_event_mouse_set_range(pdat->ctx, p[0], p[1]);
			return 0;
		}
		break;
	case 0xcd455615: /* "mouse-get-range" */
		if(p)
		{
			sandbox_event_mouse_get_range(pdat->ctx, &p[0], &p[1]);
			return 0;
		}
		break;
	case 0xe818d6df: /* "mouse-set-sensitivity" */
		if(p)
		{
			sandbox_event_mouse_set_sensitivity(pdat->ctx, clamp(p[0], 1, 11));
			return 0;
		}
		break;
	case 0x40bfb1d3: /* "mouse-get-sensitivity" */
		if(p)
		{
			sandbox_event_mouse_get_sensitivity(pdat->ctx, &p[0]);
			return 0;
		}
		break;
	default:
		break;
	}
	return -1;
}

static struct device_t * input_sandbox_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct input_sandbox_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	void * ctx;

	ctx = sandbox_event_open();
	if(!ctx)
		return NULL;

	pdat = malloc(sizeof(struct input_sandbox_pdata_t));
	if(!pdat)
	{
		sandbox_event_close(ctx);
		return NULL;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		sandbox_event_close(ctx);
		free(pdat);
		return NULL;
	}

	pdat->ctx = ctx;
	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = input_sandbox_ioctl;
	input->priv = pdat;

	sandbox_event_mouse_set_range(pdat->ctx, dt_read_int(n, "pointer-range-x", 640), dt_read_int(n, "pointer-range-y", 480));
	sandbox_event_mouse_set_sensitivity(pdat->ctx, dt_read_int(n, "pointer-sensitivity", 6));
	sandbox_event_set_key_callback(pdat->ctx, input,
			cb_key_down,
			cb_key_up);
	sandbox_event_set_mouse_callback(pdat->ctx, input,
			cb_mouse_down,
			cb_mouse_move,
			cb_mouse_up,
			cb_mouse_wheel);
	sandbox_event_set_touch_callback(pdat->ctx, input,
			cb_touch_begin,
			cb_touch_move,
			cb_touch_end);
	sandbox_event_set_joystick_callback(pdat->ctx, input,
			cb_joystick_left_stick,
			cb_joystick_right_stick,
			cb_joystick_left_trigger,
			cb_joystick_right_trigger,
			cb_joystick_button_down,
			cb_joystick_button_up);

	if(!(dev = register_input(input, drv)))
	{
		sandbox_event_close(input->priv);
		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	return dev;
}

static void input_sandbox_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;

	if(input)
	{
		unregister_input(input);
		sandbox_event_close(input->priv);
		free_device_name(input->name);
		free(input->priv);
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
