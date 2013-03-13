/*
 * drivers/input/incon.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <fifo.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <xboot/machine.h>
#include <console/console.h>
#include <input/keyboard/keyboard.h>

static struct fifo_t * input_console_fifo;
static bool_t input_console_onoff;

static void incon_keyboard_handler(enum key_code key)
{
	u32_t c;

	switch(key)
	{
	case KEY_UP:
		c = KEY_CTRL_P;
		break;

	case KEY_DOWN:
		c = KEY_CTRL_N;
		break;

	case KEY_LEFT:
		c = KEY_CTRL_B;
		break;

	case KEY_RIGHT:
		c = KEY_CTRL_F;
		break;

	case KEY_TAB:
		c = KEY_CTRL_I;
		break;

	case KEY_BACKSPACE:
		c = KEY_CTRL_H;
		break;

	case KEY_ENTER:
		c = KEY_CTRL_J;
		break;

	case KEY_HOME:
		c = KEY_CTRL_A;
		break;

	case KEY_MENU:
		c = KEY_CTRL_E;
		break;

	case KEY_BACK:
		c = KEY_CTRL_C;
		break;

	default:
		c = key;
		break;
	}

	fifo_put(input_console_fifo, (u8_t *)&c, sizeof(u32_t));
}

static bool_t incon_getcode(struct console * console, u32_t * code)
{
	if(!input_console_onoff)
		return FALSE;

	if(fifo_get(input_console_fifo, (u8_t *)code, sizeof(u32_t)) == sizeof(u32_t))
		return TRUE;

	return FALSE;
}

bool_t incon_onoff(struct console * console, bool_t flag)
{
	input_console_onoff = flag;
	return TRUE;
}

static struct console input_console = {
	.name     		= "input",
	.getwh			= NULL,
	.getxy			= NULL,
	.gotoxy			= NULL,
	.setcursor		= NULL,
	.getcursor		= NULL,
	.setcolor		= NULL,
	.getcolor		= NULL,
	.cls			= NULL,
	.getcode		= incon_getcode,
	.putcode		= NULL,
	.onoff			= incon_onoff,
	.priv			= NULL,
};

static __init void input_console_init(void)
{
	input_console_fifo = fifo_alloc(sizeof(u32_t) * 32);
	input_console_onoff = TRUE;

	install_listener_onkeydown(incon_keyboard_handler);

	if(!register_console(&input_console))
		LOG_E("failed to register input console '%s'", input_console.name);
}

static __exit void input_console_exit(void)
{
	if(!unregister_console(&input_console))
		LOG_E("failed to unregister input console '%s'", input_console.name);

	remove_listener_onkeydown(incon_keyboard_handler);

	free(input_console_fifo);
}

device_initcall(input_console_init);
device_exitcall(input_console_exit);
