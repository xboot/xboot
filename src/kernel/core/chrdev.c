/*
 * kernel/core/chrdev.c
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
#include <stddef.h>
#include <malloc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/device.h>
#include <xboot/chrdev.h>

extern struct device_list * device_list;

/*
 * search char device by name
 */
struct chrdev * search_chrdev(const char * name)
{
	struct chrdev * dev;
	struct device_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(list->device->type == CHAR_DEVICE)
		{
			dev = (struct chrdev *)(list->device->priv);
			if(strcmp((const char *)dev->name, (const char *)name) == 0)
				return dev;
		}
	}

	return NULL;
}

/*
 * search char device by name and char device type
 */
struct chrdev * search_chrdev_with_type(const char * name, enum chrdev_type type)
{
	struct chrdev * dev;
	struct device_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(list->device->type == CHAR_DEVICE)
		{
			dev = (struct chrdev *)(list->device->priv);
			if(dev->type == type)
			{
				if(strcmp((const char *)dev->name, (const char *)name) == 0)
					return dev;
			}
		}
	}

	return NULL;
}

/*
 * register a char device into device_list
 */
bool_t register_chrdev(struct chrdev * dev)
{
	struct device * device;

	device = malloc(sizeof(struct device));
	if(!device || !dev)
	{
		free(device);
		return FALSE;
	}

	if(!dev->name || search_device(dev->name))
	{
		free(device);
		return FALSE;
	}

	device->name = dev->name;
	device->type = CHAR_DEVICE;
	device->priv = (void *)dev;

	return register_device(device);
}

/*
 * unregister char device from chrdev_list
 */
bool_t unregister_chrdev(const char * name)
{
	struct device * device;

	if(!name)
		return FALSE;

	device = search_device(name);
	if(!device && device->type == CHAR_DEVICE)
		return FALSE;

	if(unregister_device(device))
	{
		free(device);
		return TRUE;
	}

	return FALSE;
}
