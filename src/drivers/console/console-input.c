/*
 * drivers/console/console-input.c
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

#include <xboot.h>
#include <console/console.h>
#include <console/console-input.h>

struct input_console_info_t
{
	char * name;
	struct input_t * input;
	bool_t onoff;
};

static bool_t input_console_getcode(struct console_t * console, u32_t * code)
{
	struct input_console_info_t * info = console->priv;
	struct event_t event;

	if(!info->onoff)
		return FALSE;

	if(!pump_event(runtime_get()->__event_base, &event))
		return FALSE;

	if(event.type != EVENT_TYPE_KEY_DOWN)
		return FALSE;

	switch(event.e.key.code)
	{
	case KEY_UP:
		*code = 0x10;	/* ctrl-p */
		break;
	case KEY_DOWN:
		*code = 0xe;	/* ctrl-n */;
		break;
	case KEY_LEFT:
		*code = 0x2;	/* ctrl-b */
		break;
	case KEY_RIGHT:
		*code = 0x6;	/* ctrl-f */
		break;
	case KEY_TAB:
		*code = 0x9;	/* ctrl-i */
		break;
	case KEY_BACKSPACE:
		*code = 0x8;	/* ctrl-h */
		break;
	case KEY_ENTER:
		*code = 0xa;	/* ctrl-j */
		break;
	case KEY_HOME:
		*code = 0x1;	/* ctrl-a */
		break;
	case KEY_MENU:
		*code = 0x5;	/* ctrl-e */
		break;
	case KEY_BACK:
		*code = 0x3;	/* ctrl-c */
		break;
	default:
		*code = event.e.key.code;
		break;
	}
	return TRUE;
}

static bool_t input_console_onoff(struct console_t * console, bool_t flag)
{
	struct input_console_info_t * info = console->priv;

	info->onoff = flag;
	return TRUE;
}

bool_t register_console_input(struct input_t * input)
{
	struct console_t * console;
	struct input_console_info_t * info;

	if(!input || !input->name)
		return FALSE;

	console = malloc(sizeof(struct console_t));
	info = malloc(sizeof(struct input_console_info_t));
	if(!console || !info)
	{
		free(console);
		free(info);
		return FALSE;
	}

	info->name = (char *)input->name;
	info->input = input;
	info->onoff = TRUE;

	console->name = strdup(info->name);
	console->getwh = NULL;
	console->getxy = NULL;
	console->gotoxy = NULL;
	console->setcursor = NULL;
	console->getcursor = NULL;
	console->setcolor = NULL;
	console->getcolor = NULL;
	console->cls = NULL;
	console->getcode = input_console_getcode;
	console->putcode = NULL;
	console->onoff = input_console_onoff;
	console->priv = info;

	if(!register_console(console))
	{
		free(console->name);
		free(console);
		free(info);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_console_input(struct input_t * input)
{
	struct console_t * console;
	struct input_console_info_t * info;

	if(!input || !input->name)
		return FALSE;

	console = search_console(input->name);
	if(console)
		info = (struct input_console_info_t *)console->priv;
	else
		return FALSE;

	if(!unregister_console(console))
		return FALSE;

	free(console->name);
	free(console);
	free(info);

	return TRUE;
}
