/*
 * drivers/input/input.c
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
#include <hash.h>
#include <fifo.h>
#include <stdio.h>
#include <xboot/chrdev.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/machine.h>
#include <xboot/resource.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/input.h>

extern void keyboard_input_handler(struct input_event * event);
extern void mouse_input_handler(struct input_event * event);

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

static struct fifo_t * input_keyboard_fifo;
static struct fifo_t * input_mouse_fifo;
static struct fifo_t * input_touchscreen_fifo;
static struct fifo_t * input_joystick_fifo;
static struct fifo_t * input_accelerometer_fifo;
static struct fifo_t * input_gyroscope_fifo;
static struct fifo_t * input_light_fifo;
static struct fifo_t * input_magnetic_fifo;
static struct fifo_t * input_orientation_fifo;
static struct fifo_t * input_pressure_fifo;
static struct fifo_t * input_proximity_fifo;
static struct fifo_t * input_temperature_fifo;


struct input * search_input(const char * name)
{
	struct input_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&input_list->entry)->next; pos != (&input_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct input_list, entry);
		if(strcmp(list->input->name, name) == 0)
			return list->input;
	}

	return NULL;
}

bool_t register_input(struct input * input)
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

	if(! (input->probe)(input))
	{
		free(list);
		return FALSE;
	}

	list->input = input;
	list_add_tail(&list->entry, &input_list->entry);

	return TRUE;
}

bool_t unregister_input(struct input * input)
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
				(input->remove)(input);

			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

void input_report(enum input_type type, s32_t code, s32_t value)
{
	struct input_event event;

	event.time = jiffies;
	event.type = type;
	event.code = code;
	event.value = value;

	switch(type)
	{
	case INPUT_KEYBOARD:
		fifo_put(input_keyboard_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_MOUSE:
		fifo_put(input_mouse_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_TOUCHSCREEN:
		fifo_put(input_touchscreen_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_JOYSTICK:
		fifo_put(input_joystick_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_ACCELEROMETER:
		fifo_put(input_accelerometer_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_GYROSCOPE:
		fifo_put(input_gyroscope_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_LIGHT:
		fifo_put(input_light_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_MAGNETIC:
		fifo_put(input_magnetic_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_ORIENTATION:
		fifo_put(input_orientation_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_PRESSURE:
		fifo_put(input_pressure_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_PROXIMITY:
		fifo_put(input_proximity_fifo, (u8_t *)&event, sizeof(struct input_event));
		break;

	case INPUT_TEMPERATURE:
		fifo_put(input_temperature_fifo, (u8_t *)&event, sizeof(struct input_event));
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
		while(fifo_get(input_keyboard_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{
			keyboard_input_handler(&event);
		}
		break;

	case INPUT_MOUSE:
		while(fifo_get(input_mouse_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{
			mouse_input_handler(&event);
		}
		break;

	case INPUT_TOUCHSCREEN:
		while(fifo_get(input_touchscreen_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_JOYSTICK:
		while(fifo_get(input_joystick_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_ACCELEROMETER:
		while(fifo_get(input_accelerometer_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_GYROSCOPE:
		while(fifo_get(input_gyroscope_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_LIGHT:
		while(fifo_get(input_light_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_MAGNETIC:
		while(fifo_get(input_magnetic_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_ORIENTATION:
		while(fifo_get(input_orientation_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_PRESSURE:
		while(fifo_get(input_pressure_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_PROXIMITY:
		while(fifo_get(input_proximity_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	case INPUT_TEMPERATURE:
		while(fifo_get(input_temperature_fifo, (u8_t *)&event, sizeof(struct input_event)) == sizeof(struct input_event))
		{

		}
		break;

	default:
		break;
	}
}

static s32_t input_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct input_list * list;
	struct list_head * pos;
	s8_t * p;
	s32_t len = 0;
	char buff[32];

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)"[input]");

	for(pos = (&input_list->entry)->next; pos != (&input_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct input_list, entry);

		switch(list->input->type)
		{
		case INPUT_KEYBOARD:
			strcpy(buff, (const char *)"keyboard");
			break;

		case INPUT_MOUSE:
			strcpy(buff, (const char *)"mouse");
			break;

		case INPUT_TOUCHSCREEN:
			strcpy(buff, (const char *)"touchscreen");
			break;

		case INPUT_JOYSTICK:
			strcpy(buff, (const char *)"joystick");
			break;

		case INPUT_ACCELEROMETER:
			strcpy(buff, (const char *)"accelerometer");
			break;

		case INPUT_GYROSCOPE:
			strcpy(buff, (const char *)"gyroscope");
			break;

		case INPUT_LIGHT:
			strcpy(buff, (const char *)"light");
			break;

		case INPUT_MAGNETIC:
			strcpy(buff, (const char *)"magnetic");
			break;

		case INPUT_ORIENTATION:
			strcpy(buff, (const char *)"orientation");
			break;

		case INPUT_PRESSURE:
			strcpy(buff, (const char *)"pressure");
			break;

		case INPUT_PROXIMITY:
			strcpy(buff, (const char *)"proximity");
			break;

		case INPUT_TEMPERATURE:
			strcpy(buff, (const char *)"temperature");
			break;

		default:
			strcpy(buff, (const char *)"unknown");
			break;
		}
		len += sprintf((char *)(p + len), (const char *)"\r\n %s %*s%s", list->input->name, (int)(16 - strlen(list->input->name)), "", buff);
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (u8_t *)(p + offset), len);
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

pure_initcall_sync(input_pure_sync_init);
pure_exitcall_sync(input_pure_sync_exit);
