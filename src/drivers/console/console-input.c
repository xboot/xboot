/*
 * drivers/console/console-input.c
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

#include <fifo.h>
#include <console/console.h>
#include <console/console-input.h>

struct console_input_data_t {
	struct input_t * input;
	struct fifo_t * fifo;
};

static ssize_t console_input_read(struct console_t * console, unsigned char * buf, size_t count)
{
	struct console_input_data_t * dat = (struct console_input_data_t *)console->priv;
	struct event_t event;
	u32_t code;
	u8_t key[16];
	size_t len;

	if(pump_event(runtime_get()->__event_base, &event) && (event.type == EVENT_TYPE_KEY_DOWN) && (event.device == dat->input))
	{
		code = event.e.key.code;
		switch(code)
		{
		case KEY_BACKSPACE:
			key[0] = 0x7f;
			len = 1;
			break;

		case KEY_TAB:
			key[0] = 0x9;
			len = 1;
			break;

		case KEY_ENTER:
			key[0] = 0xd;
			len = 1;
			break;

		case KEY_UP:
			key[0] = '\e';
			key[1] = '[';
			key[2] = 'A';
			len = 3;
			break;

		case KEY_DOWN:
			key[0] = '\e';
			key[1] = '[';
			key[2] = 'B';
			len = 3;
			break;

		case KEY_LEFT:
			key[0] = '\e';
			key[1] = '[';
			key[2] = 'D';
			len = 3;
			break;

		case KEY_RIGHT:
			key[0] = '\e';
			key[1] = '[';
			key[2] = 'C';
			len = 3;
			break;

		case KEY_PAGE_UP:
			key[0] = '\e';
			key[1] = '[';
			key[2] = '5';
			key[3] = '~';
			len = 4;
			break;

		case KEY_PAGE_DOWN:
			key[0] = '\e';
			key[1] = '[';
			key[2] = '6';
			key[3] = '~';
			len = 4;
			break;

		case KEY_HOME:
			key[0] = '\e';
			key[1] = '[';
			key[2] = '1';
			key[3] = '~';
			len = 4;
			break;

		case KEY_END:
			key[0] = '\e';
			key[1] = '[';
			key[2] = '4';
			key[3] = '~';
			len = 4;
			break;

		case KEY_VOLUME_UP:
		case KEY_VOLUME_DOWN:
		case KEY_MENU:
		case KEY_BACK:
		case KEY_POWER:
			len = 0;
			break;

		default:
			ucs4_to_utf8(&code, 1, (char *)key, sizeof(key));
			len = strlen((const char *)key);
			break;
		}
		fifo_put(dat->fifo, key, len);
	}

	return fifo_get(dat->fifo, buf, count);
}

static void console_input_suspend(struct console_t * console)
{
}

static void console_input_resume(struct console_t * console)
{
}

bool_t register_console_input(struct input_t * input)
{
	struct console_input_data_t * dat;
	struct console_t * console;

	if(!input || !input->name)
		return FALSE;

	dat = malloc(sizeof(struct console_input_data_t));
	if(!dat)
		return FALSE;

	console = malloc(sizeof(struct console_t));
	if(!console)
	{
		free(dat);
		return FALSE;
	}

	dat->input = input;
	dat->fifo = fifo_alloc(256);
	console->name = strdup(input->name);
	console->read = console_input_read,
	console->write = NULL,
	console->suspend = console_input_suspend,
	console->resume	= console_input_resume,
	console->priv = dat;

	if(register_console(console))
		return TRUE;

	free(console->priv);
	free(console->name);
	free(console);
	return FALSE;
}

bool_t unregister_console_input(struct input_t * input)
{
	struct console_t * console;
	struct console_input_data_t * dat;

	console = search_console(input->name);
	if(!console)
		return FALSE;
	dat = (struct console_input_data_t *)console->priv;

	if(!unregister_console(console))
		return FALSE;

	fifo_free(dat->fifo);
	free(dat);
	free(console->name);
	free(console);
	return TRUE;
}
