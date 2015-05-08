/*
 * kernel/core/device.c
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

#include <xboot.h>
#include <spinlock.h>
#include <xboot/device.h>

struct device_list_t __device_list = {
	.entry = {
		.next	= &(__device_list.entry),
		.prev	= &(__device_list.entry),
	},
};
static spinlock_t __device_list_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_device_kobj(struct device_t * dev)
{
	struct kobj_t * kdevice;
	char * name;

	if(!dev || !dev->kobj)
		return NULL;

	kdevice = kobj_search_directory_with_create(kobj_get_root(), "device");
	if(!kdevice)
		return NULL;

	switch(dev->type)
	{
	case DEVICE_TYPE_CONSOLE:
		name = "console";
		break;
	case DEVICE_TYPE_RTC:
		name = "rtc";
		break;
	case DEVICE_TYPE_LED:
		name = "led";
		break;
	case DEVICE_TYPE_LEDTRIG:
		name = "ledtrig";
		break;
	case DEVICE_TYPE_BUZZER:
		name = "buzzer";
		break;
	case DEVICE_TYPE_FRAMEBUFFER:
		name = "framebuffer";
		break;
	case DEVICE_TYPE_INPUT:
		name = "input";
		break;
	case DEVICE_TYPE_WATCHDOG:
		name = "watchdog";
		break;
	case DEVICE_TYPE_BATTERY:
		name = "battery";
		break;
	case DEVICE_TYPE_BLOCK:
		name = "block";
		break;
	case DEVICE_TYPE_DISK:
		name = "disk";
		break;
	default:
		return NULL;
	}

	return kobj_search_directory_with_create(kdevice, (const char *)name);
}

struct device_t * search_device(const char * name)
{
	struct device_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(strcmp(pos->device->name, name) == 0)
			return pos->device;
	}

	return NULL;
}

struct device_t * search_device_with_type(const char * name, enum device_type_t type)
{
	struct device_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == type)
		{
			if(strcmp(pos->device->name, name) == 0)
				return pos->device;
		}
	}

	return NULL;
}

struct device_t * search_first_device_with_type(enum device_type_t type)
{
	struct device_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == type)
		{
			return pos->device;
		}
	}

	return NULL;
}

bool_t register_device(struct device_t * dev)
{
	struct device_list_t * dl;

	if(!dev || !dev->name)
		return FALSE;

	if(!dev->suspend || !dev->resume)
		return FALSE;

	if(!dev->driver)
		return FALSE;

	if(search_device(dev->name))
		return FALSE;

	dl = malloc(sizeof(struct device_list_t));
	if(!dl)
		return FALSE;

	kobj_add(search_device_kobj(dev), dev->kobj);
	dl->device = dev;

	spin_lock_irq(&__device_list_lock);
	list_add_tail(&dl->entry, &(__device_list.entry));
	spin_unlock_irq(&__device_list_lock);

	return TRUE;
}

bool_t unregister_device(struct device_t * dev)
{
	struct device_list_t * pos, * n;

	if(!dev || !dev->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device == dev)
		{
			spin_lock_irq(&__device_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__device_list_lock);

			kobj_remove(search_device_kobj(dev), pos->device->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

void suspend_device(const char * name)
{
	struct device_t * dev =	search_device(name);

	if(dev)
		dev->suspend(dev);
}

void resume_device(const char * name)
{
	struct device_t * dev =	search_device(name);

	if(dev)
		dev->resume(dev);
}

void suspend_all_device(void)
{
	struct device_list_t * pos, * n;

	list_for_each_entry_safe_reverse(pos, n, &(__device_list.entry), entry)
	{
		LOG("Suspend device '%s'", pos->device->name);
		pos->device->suspend(pos->device);
	}
}

void resume_all_device(void)
{
	struct device_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		LOG("Resume device '%s'", pos->device->name);
		pos->device->resume(pos->device);
	}
}
