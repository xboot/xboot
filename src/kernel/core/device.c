/*
 * kernel/core/device.c
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
#include <spinlock.h>
#include <xboot/device.h>

struct device_list_t __device_list = {
	.entry = {
		.next	= &(__device_list.entry),
		.prev	= &(__device_list.entry),
	},
};
static spinlock_t __device_lock_lock = SPIN_LOCK_INIT();

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

	dl->device = dev;

	spin_lock_irq(&__device_lock_lock);
	list_add_tail(&dl->entry, &(__device_list.entry));
	spin_unlock_irq(&__device_lock_lock);

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
			spin_lock_irq(&__device_lock_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__device_lock_lock);

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

static s32_t device_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct device_list_t * pos, * n;
	char * p;
	int len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), "[device]");

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		len += sprintf((char *)(p + len), "\r\n    %s", pos->device->name);
	}

	len -= offset;

	if(len < 0)
		len = 0;

	if(len > count)
		len = count;

	memcpy(buf, (char *)(p + offset), len);
	free(p);

	return len;
}

static struct proc_t device_proc = {
	.name	= "device",
	.read	= device_proc_read,
};

static __init void device_pure_sync_init(void)
{
	proc_register(&device_proc);
}

static __exit void device_pure_sync_exit(void)
{
	proc_unregister(&device_proc);
}

pure_initcall_sync(device_pure_sync_init);
pure_exitcall_sync(device_pure_sync_exit);
