/*
 * drivers/input/input.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <fifo.h>
#include <vsprintf.h>
#include <xboot/chrdev.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <xboot/resource.h>
#include <terminal/terminal.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/input.h>

extern void keyboard_input_handler(struct input_event * event);


struct input_list
{
	struct input * input;
	struct list_head entry;
};

static struct input_list __input_list = {
	.entry = {
		.next	= &(__input_list.entry),
		.prev	= &(__input_list.entry),
	},
};
static struct input_list * input_list = &__input_list;

static struct fifo * input_keyboard_fifo;
static struct fifo * input_mouse_fifo;
static struct fifo * input_touchscreen_fifo;
static struct fifo * input_joystick_fifo;
static struct fifo * input_accelerometer_fifo;
static struct fifo * input_gyroscope_fifo;
static struct fifo * input_light_fifo;
static struct fifo * input_magnetic_fifo;
static struct fifo * input_orientation_fifo;
static struct fifo * input_pressure_fifo;
static struct fifo * input_proximity_fifo;
static struct fifo * input_temperature_fifo;


struct input * search_input(const char * name)
{
	struct input_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&input_list->entry)->next; pos != (&input_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct input_list, entry);
		if(strcmp((x_s8*)list->input->name, (const x_s8 *)name) == 0)
			return list->input;
	}

	return NULL;
}

x_bool register_input(struct input * input)
{
	struct input_list * list;

	if(!input || !input->name)
		return FALSE;

	if(search_input(input->name))
		return FALSE;

	if(!input->probe || !input->remove)
		return FALSE;

	if(search_input(input->name))
		return FALSE;

	list = malloc(sizeof(struct input_list));
	if(!list)
		return FALSE;

	if(input->probe)
		(input->probe)();

	list->input = input;
	list_add_tail(&list->entry, &input_list->entry);

	return TRUE;
}

x_bool unregister_input(struct input * input)
{
	struct input_list * list;
	struct list_head * pos;

	if(!input || !input->name)
		return FALSE;

	for(pos = (&input_list->entry)->next; pos != (&input_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct input_list, entry);
		if(list->input == input)
		{
			if(input->remove)
				(input->remove)();

			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

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

	case INPUT_ACCELEROMETER:
		fifo_put(input_accelerometer_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_GYROSCOPE:
		fifo_put(input_gyroscope_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_LIGHT:
		fifo_put(input_light_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_MAGNETIC:
		fifo_put(input_magnetic_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_ORIENTATION:
		fifo_put(input_orientation_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_PRESSURE:
		fifo_put(input_pressure_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_PROXIMITY:
		fifo_put(input_proximity_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	case INPUT_TEMPERATURE:
		fifo_put(input_temperature_fifo, (x_u8 *)&event, sizeof(struct input_event));
		break;

	default:
		break;
	}
}

void input_sync(enum input_type type)
{
	struct input_event event;

	switch(type)
	{
	case INPUT_KEYBOARD:
		if(fifo_get(input_keyboard_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{
			keyboard_input_handler(&event);
		}
		break;

	case INPUT_MOUSE:
		if(fifo_get(input_mouse_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_TOUCHSCREEN:
		if(fifo_get(input_touchscreen_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_JOYSTICK:
		if(fifo_get(input_joystick_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_ACCELEROMETER:
		if(fifo_get(input_accelerometer_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_GYROSCOPE:
		if(fifo_get(input_gyroscope_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_LIGHT:
		if(fifo_get(input_light_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_MAGNETIC:
		if(fifo_get(input_magnetic_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_ORIENTATION:
		if(fifo_get(input_orientation_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_PRESSURE:
		if(fifo_get(input_pressure_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_PROXIMITY:
		if(fifo_get(input_proximity_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_TEMPERATURE:
		if(fifo_get(input_temperature_fifo, (x_u8 *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	default:
		break;
	}
}

static x_s32 input_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct input_list * list;
	struct list_head * pos;
	x_s8 * p;
	x_s32 len = 0;
	x_s8 buff[32];

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[input]");

	for(pos = (&input_list->entry)->next; pos != (&input_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct input_list, entry);

		switch(list->input->type)
		{
		case INPUT_KEYBOARD:
			strcpy(buff, (const x_s8 *)"keyboard");
			break;

		case INPUT_MOUSE:
			strcpy(buff, (const x_s8 *)"mouse");
			break;

		case INPUT_TOUCHSCREEN:
			strcpy(buff, (const x_s8 *)"touchscreen");
			break;

		case INPUT_JOYSTICK:
			strcpy(buff, (const x_s8 *)"joystick");
			break;

		case INPUT_ACCELEROMETER:
			strcpy(buff, (const x_s8 *)"accelerometer");
			break;

		case INPUT_GYROSCOPE:
			strcpy(buff, (const x_s8 *)"gyroscope");
			break;

		case INPUT_LIGHT:
			strcpy(buff, (const x_s8 *)"light");
			break;

		case INPUT_MAGNETIC:
			strcpy(buff, (const x_s8 *)"magnetic");
			break;

		case INPUT_ORIENTATION:
			strcpy(buff, (const x_s8 *)"orientation");
			break;

		case INPUT_PRESSURE:
			strcpy(buff, (const x_s8 *)"pressure");
			break;

		case INPUT_PROXIMITY:
			strcpy(buff, (const x_s8 *)"proximity");
			break;

		case INPUT_TEMPERATURE:
			strcpy(buff, (const x_s8 *)"temperature");
			break;

		default:
			strcpy(buff, (const x_s8 *)"unknown");
			break;
		}
		len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s - %s", list->input->name, buff);
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (x_u8 *)(p + offset), len);
	free(p);

	return len;
}

static struct proc input_proc = {
	.name	= "input",
	.read	= input_proc_read,
};

static __init void input_pure_sync_init(void)
{
	input_keyboard_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_mouse_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_touchscreen_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_joystick_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_accelerometer_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_gyroscope_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_light_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_magnetic_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_orientation_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_pressure_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_proximity_fifo = fifo_alloc(sizeof(struct input_event) * 256);
	input_temperature_fifo = fifo_alloc(sizeof(struct input_event) * 256);

	proc_register(&input_proc);
}

static __init void input_pure_sync_exit(void)
{
	fifo_free(input_keyboard_fifo);
	fifo_free(input_mouse_fifo);
	fifo_free(input_touchscreen_fifo);
	fifo_free(input_joystick_fifo);
	fifo_free(input_accelerometer_fifo);
	fifo_free(input_gyroscope_fifo);
	fifo_free(input_light_fifo);
	fifo_free(input_magnetic_fifo);
	fifo_free(input_orientation_fifo);
	fifo_free(input_pressure_fifo);
	fifo_free(input_proximity_fifo);
	fifo_free(input_temperature_fifo);

	proc_unregister(&input_proc);
}

module_init(input_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(input_pure_sync_exit, LEVEL_PURE_SYNC);
