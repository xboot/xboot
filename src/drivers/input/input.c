/*
 * drivers/input/input.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <fifo.h>
#include <xboot/chrdev.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <xboot/resource.h>
#include <terminal/terminal.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/input.h>

static struct fifo * input_keyboard_fifo;
static struct fifo * input_mouse_fifo;
static struct fifo * input_touchscreen_fifo;
static struct fifo * input_joystick_fifo;
static struct fifo * input_sensor_fifo;

void input_report(enum input_type type, x_u32 code, x_u32 value)
{
	struct input_event event;

	event.time = jiffies;
	event.type = type;
	event.code = code;
	event.value = value;

	switch(type)
	{
	case INPUT_KEYBOARD:
		fifo_put(input_keyboard_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_MOUSE:
		fifo_put(input_mouse_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_TOUCHSCREEN:
		fifo_put(input_touchscreen_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_JOYSTICK:
		fifo_put(input_joystick_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_SENSOR:
		fifo_put(input_sensor_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	default:
		break;
	}
}

void input_sync(enum input_type type)
{
	printk("%ld,\r\n", fifo_len(input_keyboard_fifo));
}

static __init void input_pure_sync_init(void)
{
	input_keyboard_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_mouse_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_touchscreen_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_joystick_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_sensor_fifo = fifo_alloc(sizeof(struct input_event) * 256);
}

static __init void input_pure_sync_exit(void)
{
	fifo_free(input_keyboard_fifo);
	fifo_free(input_mouse_fifo);
	fifo_free(input_touchscreen_fifo);
	fifo_free(input_joystick_fifo);
	fifo_free(input_sensor_fifo);
}

module_init(input_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(input_pure_sync_exit, LEVEL_PURE_SYNC);
