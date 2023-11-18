/*
 * kernel/command/cmd-event.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <shell/ctrlc.h>
#include <input/input.h>
#include <xboot/window.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    event [framebuffer] [input]\r\n");
}

struct event_context_t {
	struct {
		int x;
		int y;
	} param[1024];
	int index;
	int pressed;
};

static void event_draw(struct window_t * w, void * o)
{
	struct event_context_t * ectx = (struct event_context_t *)o;

	if(ectx->index > 0)
	{
		surface_shape_save(w->s);
		surface_shape_set_line_width(w->s, 1);
		surface_shape_set_source_color(w->s, &(struct color_t){ 0, 0, 255, 255 });
		surface_shape_move_to(w->s, ectx->param[0].x, ectx->param[0].y);
		for(int i = 1; i < ectx->index; i++)
			surface_shape_line_to(w->s, ectx->param[i].x, ectx->param[i].y);
		surface_shape_stroke(w->s);
		surface_shape_set_line_width(w->s, 1);
		surface_shape_set_source_color(w->s, &(struct color_t){ 255, 0, 0, 255 });
		for(int i = 0; i < ectx->index; i++)
			surface_shape_rectangle(w->s, ectx->param[i].x - 1, ectx->param[i].y - 1, 2, 2);
		surface_shape_fill(w->s);
		surface_shape_restore(w->s);

		if(ectx->pressed)
		{
			surface_shape_save(w->s);
			surface_shape_set_line_width(w->s, 1);
			surface_shape_set_source_color(w->s, &(struct color_t){ 255, 255, 0, 255 });
			surface_shape_move_to(w->s, 0, ectx->param[ectx->index - 1].y);
			surface_shape_line_to(w->s, window_get_width(w), ectx->param[ectx->index - 1].y);
			surface_shape_move_to(w->s, ectx->param[ectx->index - 1].x, 0);
			surface_shape_line_to(w->s, ectx->param[ectx->index - 1].x, window_get_height(w));
			surface_shape_stroke(w->s);
			surface_shape_set_line_width(w->s, 1);
			surface_shape_set_source_color(w->s, &(struct color_t){ 0, 0, 255, 255 });
			surface_shape_arc(w->s, ectx->param[ectx->index - 1].x, ectx->param[ectx->index - 1].y, 32, 0, 2 * M_PI);
			surface_shape_stroke(w->s);
			surface_shape_restore(w->s);
		}
	}
}

static int do_event(int argc, char ** argv)
{
	const char * fb = (argc >= 2) ? argv[1] : NULL;
	const char * input = (argc >= 3) ? argv[2] : NULL;
	struct window_t * w = window_alloc(fb, input);
	struct event_context_t * ectx = malloc(sizeof(struct event_context_t));
	struct input_t * dev;
	struct event_t e;
	int running = 1;

	if(w && ectx)
	{
		memset(ectx, 0, sizeof(struct event_context_t));
		while(running)
		{
			if(window_pump_event(w, &e))
			{
				dev = (struct input_t *)(e.device);
				switch(e.type)
				{
				case EVENT_TYPE_KEY_DOWN:
					printf("[%s]: [KeyDown] [%d]\r\n", dev->name, e.e.key_down.key);
					break;
				case EVENT_TYPE_KEY_UP:
					printf("[%s]: [KeyUp] [%d]\r\n", dev->name, e.e.key_up.key);
					break;
				case EVENT_TYPE_ROTARY_TURN:
					printf("[%s]: [RotaryTurn] [%d]\r\n", dev->name, e.e.rotary_turn.v);
					break;
				case EVENT_TYPE_MOUSE_DOWN:
					if(e.e.mouse_down.button & MOUSE_BUTTON_LEFT)
					{
						ectx->index = 0;
						ectx->pressed = 1;
						ectx->param[ectx->index].x = e.e.mouse_down.x;
						ectx->param[ectx->index].y = e.e.mouse_down.y;
						ectx->index++;
					}
					printf("[%s]: [MouseDown] [%d][%d][0x%x]\r\n", dev->name, e.e.mouse_down.x, e.e.mouse_down.y, e.e.mouse_down.button);
					break;
				case EVENT_TYPE_MOUSE_MOVE:
					if(ectx->pressed)
					{
						if(ectx->index < 1024)
						{
							ectx->param[ectx->index].x = e.e.mouse_move.x;
							ectx->param[ectx->index].y = e.e.mouse_move.y;
							ectx->index++;
						}
					}
					printf("[%s]: [MouseMove] [%d][%d]\r\n", dev->name, e.e.mouse_move.x, e.e.mouse_move.y);
					break;
				case EVENT_TYPE_MOUSE_UP:
					ectx->pressed = 0;
					printf("[%s]: [MouseUp] [%d][%d][0x%x]\r\n", dev->name, e.e.mouse_up.x, e.e.mouse_up.y, e.e.mouse_up.button);
					break;
				case EVENT_TYPE_MOUSE_WHEEL:
					printf("[%s]: [MouseWheel] [%d][%d]\r\n", dev->name, e.e.mouse_wheel.dx, e.e.mouse_wheel.dy);
					break;
				case EVENT_TYPE_TOUCH_BEGIN:
					ectx->index = 0;
					ectx->pressed = 1;
					ectx->param[ectx->index].x = e.e.touch_begin.x;
					ectx->param[ectx->index].y = e.e.touch_begin.y;
					ectx->index++;
					printf("[%s]: [TouchBegin] [%d][%d][%d]\r\n", dev->name, e.e.touch_begin.x, e.e.touch_begin.y, e.e.touch_begin.id);
					break;
				case EVENT_TYPE_TOUCH_MOVE:
					if(ectx->pressed)
					{
						if(ectx->index < 1024)
						{
							ectx->param[ectx->index].x = e.e.touch_move.x;
							ectx->param[ectx->index].y = e.e.touch_move.y;
							ectx->index++;
						}
					}
					printf("[%s]: [TouchMove] [%d][%d][%d]\r\n", dev->name, e.e.touch_move.x, e.e.touch_move.y, e.e.touch_move.id);
					break;
				case EVENT_TYPE_TOUCH_END:
					ectx->pressed = 0;
					printf("[%s]: [TouchEnd] [%d][%d][%d]\r\n", dev->name, e.e.touch_end.x, e.e.touch_end.y, e.e.touch_end.id);
					break;
				case EVENT_TYPE_SYSTEM_EXIT:
					printf("[%s]: [Exit]\r\n", dev->name);
					running = 0;
					break;
				default:
					break;
				}
			}
			if(window_is_active(w))
				window_present(w, ectx, event_draw);
			if(ctrlc())
				running = 0;
		}
		free(ectx);
		window_free(w);
	}
	return 0;
}

static struct command_t cmd_event = {
	.name	= "event",
	.desc	= "show all of input events",
	.usage	= usage,
	.exec	= do_event,
};

static __init void event_cmd_init(void)
{
	register_command(&cmd_event);
}

static __exit void event_cmd_exit(void)
{
	unregister_command(&cmd_event);
}

command_initcall(event_cmd_init);
command_exitcall(event_cmd_exit);
