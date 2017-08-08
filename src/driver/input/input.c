/*
 * driver/input/input.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <input/input.h>

static ssize_t input_read_type(struct kobj_t * kobj, void * buf, size_t size)
{
	struct input_t * input = (struct input_t *)kobj->priv;
	char * name;

	switch(input->type)
	{
	case INPUT_TYPE_KEYBOARD:
		name = "keyboard";
		break;
	case INPUT_TYPE_ROTARY:
		name = "rotary";
		break;
	case INPUT_TYPE_MOUSE:
		name = "mouse";
		break;
	case INPUT_TYPE_TOUCHSCREEN:
		name = "touchscreen";
		break;
	case INPUT_TYPE_JOYSTICK:
		name = "joystick";
		break;
	default:
		name = "unknown";
		break;
	}

	return sprintf(buf, "%s", name);
}

struct input_t * search_input(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_INPUT);
	if(!dev)
		return NULL;

	return (struct input_t *)dev->priv;
}

bool_t register_input(struct device_t ** device, struct input_t * input)
{
	struct device_t * dev;

	if(!input || !input->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(input->name);
	dev->type = DEVICE_TYPE_INPUT;
	dev->priv = input;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "type", input_read_type, NULL, input);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_input(struct input_t * input)
{
	struct device_t * dev;

	if(!input || !input->name)
		return FALSE;

	dev = search_device(input->name, DEVICE_TYPE_INPUT);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int input_ioctl(struct input_t * input, int cmd, void * arg)
{
	if(input && input->ioctl)
		return input->ioctl(input, cmd, arg);
	return -1;
}
