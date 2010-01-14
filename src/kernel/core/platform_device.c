/*
 * kernel/core/platform_device.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <macros.h>
#include <malloc.h>
#include <hash.h>
#include <vsprintf.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <xboot/printk.h>
#include <xboot/proc.h>
#include <xboot/platform_device.h>


/*
 * the hash list of platform device
 */
static struct hlist_head platform_device_hash[CONFIG_PLATFORM_DEVICE_HASH_SIZE];

/*
 * search platform device by name. a static function.
 */
static struct platform_device * platform_device_search(const char * name)
{
	struct platform_device_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!name)
		return NULL;

	hash = string_hash(name) % CONFIG_PLATFORM_DEVICE_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(platform_device_hash[hash]), node)
	{
		if(strcmp((x_s8*)list->pdev->name, (const x_s8 *)name) == 0)
			return list->pdev;
	}

	return NULL;
}

/*
 * register a platform device into platform_device_list
 * return true is successed, otherwise is not.
 */
x_bool platform_device_register(struct platform_device * pdev)
{
	struct platform_device_list * list;
	x_u32 hash;

	list = malloc(sizeof(struct platform_device_list));
	if(!list || !pdev)
	{
		free(list);
		return FALSE;
	}

	if(!pdev->name || platform_device_search(pdev->name))
	{
		free(list);
		return FALSE;
	}

	list->pdev = pdev;

	hash = string_hash(pdev->name) % CONFIG_PLATFORM_DEVICE_HASH_SIZE;
	hlist_add_head(&(list->node), &(platform_device_hash[hash]));

	return TRUE;
}

/*
 * unregister platform device from platform_device_list
 */
x_bool platform_device_unregister(struct platform_device * pdev)
{
	struct platform_device_list * list;
	struct hlist_node * pos;
	x_u32 hash;

	if(!pdev || !pdev->name)
		return FALSE;

	hash = string_hash(pdev->name) % CONFIG_PLATFORM_DEVICE_HASH_SIZE;

	hlist_for_each_entry(list,  pos, &(platform_device_hash[hash]), node)
	{
		if(list->pdev == pdev)
		{
			hlist_del(&(list->node));
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * get platform device data.
 */
void * platform_device_get_data(const char *name)
{
	struct platform_device * pdev;

	pdev = platform_device_search(name);

	if(!pdev)
		return 0;

	return pdev->data;
}

/*
 * device proc interface
 */
static x_s32 device_proc_read(x_u8 * buf, x_s32 offset, x_s32 count)
{
	struct platform_device_list * list;
	struct hlist_node * pos;
	x_s8 * p;
	x_s32 i, len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((x_s8 *)(p + len), (const x_s8 *)"[platform device]");
	for(i = 0; i < CONFIG_PLATFORM_DEVICE_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(platform_device_hash[i]), node)
		{
			len += sprintf((x_s8 *)(p + len), (const x_s8 *)"\r\n %s", list->pdev->name);
		}
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

static struct proc device_proc = {
	.name	= "platform_device",
	.read	= device_proc_read,
};

/*
 * platform device pure sync init
 */
static __init void device_pure_sync_init(void)
{
	x_s32 i;

	/* initialize platform device hash list */
	for(i = 0; i < CONFIG_PLATFORM_DEVICE_HASH_SIZE; i++)
		init_hlist_head(&platform_device_hash[i]);

	/* register device proc interface */
	proc_register(&device_proc);
}

static __exit void device_pure_sync_exit(void)
{
	/* unregister device proc interface */
	proc_unregister(&device_proc);
}

module_init(device_pure_sync_init, LEVEL_PURE_SYNC);
module_exit(device_pure_sync_exit, LEVEL_PURE_SYNC);
