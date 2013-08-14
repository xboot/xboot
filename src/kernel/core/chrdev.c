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
#include <xboot/chrdev.h>

struct chrdev_t * search_chrdev(const char * name)
{
	struct device_list_t * pos, * n;
	struct chrdev_t * dev;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_CHAR)
		{
			dev = (struct chrdev_t *)(pos->device->priv);
			if(strcmp(dev->name, name) == 0)
				return dev;
		}
	}

	return NULL;
}

struct chrdev_t * search_chrdev_with_type(const char * name, enum chrdev_type_t type)
{
	struct device_list_t * pos, * n;
	struct chrdev_t * dev;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_CHAR)
		{
			dev = (struct chrdev_t *)(pos->device->priv);
			if(dev->type == type)
			{
				if(strcmp(dev->name, name) == 0)
					return dev;
			}
		}
	}

	return NULL;
}

bool_t register_chrdev(struct chrdev_t * dev)
{
	struct device_t * device;

	if(!dev)
		return FALSE;

	if(!dev->name || search_device(dev->name))
		return FALSE;

	device = malloc(sizeof(struct device_t));
	if(!device)
		return FALSE;

	device->name = dev->name;
	device->type = DEVICE_TYPE_CHAR;
	device->priv = (void *)dev;

	return register_device(device);
}

bool_t unregister_chrdev(const char * name)
{
	struct device_t * device;

	if(!name)
		return FALSE;

	device = search_device(name);
	if(!device && device->type == DEVICE_TYPE_CHAR)
		return FALSE;

	if(unregister_device(device))
	{
		free(device);
		return TRUE;
	}

	return FALSE;
}
