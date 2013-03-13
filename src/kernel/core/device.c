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
#include <stddef.h>
#include <malloc.h>
#include <stdio.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/proc.h>
#include <xboot/device.h>

/* the list of device */
static struct device_list __device_list = {
	.entry = {
		.next	= &(__device_list.entry),
		.prev	= &(__device_list.entry),
	},
};
struct device_list * device_list = &__device_list;

/*
 * search device by name
 */
struct device * search_device(const char * name)
{
	struct device_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(strcmp(list->device->name, name) == 0)
			return list->device;
	}

	return NULL;
}

/*
 * register a device into device_list
 */
bool_t register_device(struct device * dev)
{
	struct device_list * list;

	list = malloc(sizeof(struct device_list));
	if(!list || !dev)
	{
		free(list);
		return FALSE;
	}

	if(!dev->name || search_device(dev->name))
	{
		free(list);
		return FALSE;
	}

	if((dev->type != CHAR_DEVICE) && (dev->type != BLOCK_DEVICE) && (dev->type != NET_DEVICE))
	{
		free(list);
		return FALSE;
	}

	if(!dev->priv)
	{
		free(list);
		return FALSE;
	}

	list->device = dev;
	list_add(&list->entry, &device_list->entry);

	return TRUE;
}

/*
 * unregister device from device_list
 */
bool_t unregister_device(struct device * dev)
{
	struct device_list * list;
	struct list_head * pos;

	if(!dev || !dev->name)
		return FALSE;

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(list->device == dev)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * device proc interface
 */
static s32_t device_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct device_list * list;
	struct list_head * pos;
	s8_t * p;
	s32_t len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)"[device]");

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(list->device->type == CHAR_DEVICE)
			len += sprintf((char *)(p + len), (const char *)"\r\n CHR    %s", list->device->name);
		else if(list->device->type == BLOCK_DEVICE)
			len += sprintf((char *)(p + len), (const char *)"\r\n BLK    %s", list->device->name);
		else if(list->device->type == NET_DEVICE)
			len += sprintf((char *)(p + len), (const char *)"\r\n NET    %s", list->device->name);
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

static struct proc device_proc = {
	.name	= "device",
	.read	= device_proc_read,
};

/*
 * device pure sync init
 */
static __init void device_pure_sync_init(void)
{
	/* register device proc interface */
	proc_register(&device_proc);
}

static __exit void device_pure_sync_exit(void)
{
	/* unregister device proc interface */
	proc_unregister(&device_proc);
}

pure_initcall_sync(device_pure_sync_init);
pure_exitcall_sync(device_pure_sync_exit);
