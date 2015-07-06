/*
 * kernel/command/cmd-getevent.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

#include <input/input.h>
#include <shell/ctrlc.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    getevent\r\n");
}

static int do_getevent(int argc, char ** argv)
{
	struct input_t * input;
	struct event_t e;

	while(1)
	{
		if(pump_event(runtime_get()->__event_base, &e))
		{
			input = (struct input_t *)(e.device);

			switch(e.type)
			{
			case EVENT_TYPE_KEY_DOWN:
				printf("[%s]: [KeyDown]	[0x%x]\r\n", input->name, e.e.key_down.key);
				break;

			case EVENT_TYPE_KEY_UP:
				printf("[%s]: [KeyUp] [0x%x]\r\n", input->name, e.e.key_up.key);
				break;


			case EVENT_TYPE_MOUSE_DOWN:
				printf("[%s]: [MouseDown] [%d][%d][0x%x]\r\n", input->name, e.e.mouse_down.x, e.e.mouse_down.y, e.e.mouse_down.button);
				break;

			case EVENT_TYPE_MOUSE_MOVE:
				printf("[%s]: [MouseMove] [%d][%d]\r\n", input->name, e.e.mouse_move.x, e.e.mouse_move.y);
				break;

			case EVENT_TYPE_MOUSE_UP:
				printf("[%s]: [MouseUp] [%d][%d][0x%x]\r\n", input->name, e.e.mouse_up.x, e.e.mouse_up.y, e.e.mouse_up.button);
				break;

			case EVENT_TYPE_MOUSE_WHEEL:
				printf("[%s]: [MouseWheel] [%d][%d]\r\n", input->name, e.e.mouse_wheel.dx, e.e.mouse_wheel.dy);
				break;


			case EVENT_TYPE_TOUCH_BEGIN:
				printf("[%s]: [TouchBegin] [%d][%d][%d]\r\n", input->name, e.e.touch_begin.x, e.e.touch_begin.y, e.e.touch_begin.id);
				break;

			case EVENT_TYPE_TOUCH_MOVE:
				printf("[%s]: [TouchMove] [%d][%d][%d]\r\n", input->name, e.e.touch_move.x, e.e.touch_move.y, e.e.touch_move.id);
				break;

			case EVENT_TYPE_TOUCH_END:
				printf("[%s]: [TouchEnd] [%d][%d][%d]\r\n", input->name, e.e.touch_end.x, e.e.touch_end.y, e.e.touch_end.id);
				break;


			default:
				printf("[%s]: [Unkown]\r\n", input->name);
				break;
			}
		}

		if(ctrlc())
			return -1;
	}

	return 0;
}

static struct command_t cmd_getevent = {
	.name	= "getevent",
	.desc	= "show the all of events",
	.usage	= usage,
	.exec	= do_getevent,
};

static __init void getevent_cmd_init(void)
{
	command_register(&cmd_getevent);
}

static __exit void getevent_cmd_exit(void)
{
	command_unregister(&cmd_getevent);
}

command_initcall(getevent_cmd_init);
command_exitcall(getevent_cmd_exit);
