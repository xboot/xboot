/*
 * xboot/kernel/core/resource.c
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
#include <xboot/resource.h>

static struct resource_list_t __resource_list = {
	.entry = {
		.next	= &(__resource_list.entry),
		.prev	= &(__resource_list.entry),
	},
};
static spinlock_t __resource_list_lock = SPIN_LOCK_INIT();

bool_t register_resource(struct resource_t * res)
{
	struct resource_list_t * rl;

	if(!res || !res->name)
		return FALSE;

	rl = malloc(sizeof(struct resource_list_t));
	if(!rl)
		return FALSE;

	rl->res = res;

	spin_lock_irq(&__resource_list_lock);
	list_add_tail(&rl->entry, &(__resource_list.entry));
	spin_unlock_irq(&__resource_list_lock);

	return TRUE;
}

bool_t unregister_resource(struct resource_t * res)
{
	struct resource_list_t * pos, * n;

	if(!res || !res->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(pos->res == res)
		{
			spin_lock_irq(&__resource_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__resource_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

static struct resource_t * search_resource(const char * name)
{
	struct resource_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(strcmp(pos->res->name, name) == 0)
			return pos->res;
	}

	return NULL;
}

void * resource_get_data(const char * name)
{
	struct resource_t * res;

	res = search_resource(name);
	if(!res)
		return NULL;

	return res->data;
}

static s32_t resource_proc_read(u8_t * buf, s32_t offset, s32_t count)
{
	struct resource_list_t * pos, * n;
	s8_t * p;
	s32_t len = 0;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	len += sprintf((char *)(p + len), (const char *)"[resource]");
	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		len += sprintf((char *)(p + len), (const char *)"\r\n %s", pos->res->name);
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

static struct proc_t resource_proc = {
	.name	= "resource",
	.read	= resource_proc_read,
};

static __init void resource_pure_sync_init(void)
{
	proc_register(&resource_proc);
}

static __exit void resource_pure_sync_exit(void)
{
	proc_unregister(&resource_proc);
}

pure_initcall_sync(resource_pure_sync_init);
pure_exitcall_sync(resource_pure_sync_exit);
