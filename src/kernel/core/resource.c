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

struct resource_list_t {
	struct resource_t * res;
	struct list_head entry;
};

static struct resource_list_t __resource_list = {
	.entry = {
		.next	= &(__resource_list.entry),
		.prev	= &(__resource_list.entry),
	},
};
static spinlock_t __resource_list_lock = SPIN_LOCK_INIT();

static int resource_alloc_free_id(const char * name)
{
	struct resource_list_t * pos, * n;
	int id = 0;

	if(!name)
		return 0;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(strcmp(pos->res->name, name) == 0)
			id++;
	}

	return id;
}

static struct resource_t * search_resource_with_id(const char * name, int id)
{
	struct resource_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(pos->res->id == id)
		{
			if(strcmp(pos->res->name, name) == 0)
				return pos->res;
		}
	}

	return NULL;
}

static struct kobj_t * search_resource_kobj(struct resource_t * res)
{
	return kobj_search_directory_with_create(kobj_get_root(), "resource");
}

static ssize_t resource_read_name(struct kobj_t * kobj, void * buf, size_t size)
{
	struct resource_t * res = (struct resource_t *)kobj->priv;
	return sprintf(buf, "%s", res->name);
}

static ssize_t resource_read_id(struct kobj_t * kobj, void * buf, size_t size)
{
	struct resource_t * res = (struct resource_t *)kobj->priv;
	return sprintf(buf, "%d", res->id);
}

bool_t register_resource(struct resource_t * res)
{
	struct resource_list_t * rl;
	char name[64];

	if(!res || !res->name)
		return FALSE;

	if(res->id < 0)
	{
		res->id = resource_alloc_free_id(res->name);
	}
	else
	{
		if(search_resource_with_id(res->name, res->id))
			return FALSE;
	}

	rl = malloc(sizeof(struct resource_list_t));
	if(!rl)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);
	res->kobj = kobj_alloc_directory(name);
	kobj_add_regular(res->kobj, "name", resource_read_name, NULL, res);
	kobj_add_regular(res->kobj, "id", resource_read_id, NULL, res);
	kobj_add(search_resource_kobj(res), res->kobj);
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

			kobj_remove(search_resource_kobj(res), pos->res->kobj);
			kobj_remove_self(res->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

void resource_for_each_with_name(const char * name, bool_t (*fn)(struct resource_t *))
{
	struct resource_list_t * pos, * n;

	if(!name || !fn)
		return;

	list_for_each_entry_safe(pos, n, &(__resource_list.entry), entry)
	{
		if(strcmp(pos->res->name, name) == 0)
		{
			if(fn(pos->res))
				LOG("Resource iterator with '%s.%d'", pos->res->name, pos->res->id);
			else
				LOG("Fail to resource iterator with '%s.%d'", pos->res->name, pos->res->id);
		}
	}
}
