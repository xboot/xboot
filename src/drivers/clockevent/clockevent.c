/*
 * driver/clockevent/clockevent.c
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
#include <clockevent/clockevent.h>

struct clockevent_list_t
{
	struct clockevent_t * ce;
	struct list_head entry;
};

struct clockevent_list_t __clockevent_list = {
	.entry = {
		.next	= &(__clockevent_list.entry),
		.prev	= &(__clockevent_list.entry),
	},
};
static spinlock_t __clockevent_list_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_class_clockevent_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "clockevent");
}

static struct clockevent_t * search_clockevent(const char * name)
{
	struct clockevent_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__clockevent_list.entry), entry)
	{
		if(strcmp(pos->ce->name, name) == 0)
			return pos->ce;
	}

	return NULL;
}

bool_t register_clockevent(struct clockevent_t * ce)
{
	struct clockevent_list_t * cl;

	if(!ce || !ce->name)
		return FALSE;

	if(search_clockevent(ce->name))
		return FALSE;

	cl = malloc(sizeof(struct clockevent_list_t));
	if(!cl)
		return FALSE;

	ce->kobj = kobj_alloc_directory(ce->name);
	kobj_add(search_class_clockevent_kobj(), ce->kobj);
	cl->ce = ce;

	spin_lock_irq(&__clockevent_list_lock);
	list_add_tail(&cl->entry, &(__clockevent_list.entry));
	spin_unlock_irq(&__clockevent_list_lock);

	return TRUE;
}

bool_t unregister_clockevent(struct clockevent_t * ce)
{
	struct clockevent_list_t * pos, * n;

	if(!ce || !ce->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__clockevent_list.entry), entry)
	{
		if(pos->ce == ce)
		{
			spin_lock_irq(&__clockevent_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__clockevent_list_lock);

			kobj_remove(search_class_clockevent_kobj(), pos->ce->kobj);
			kobj_remove_self(ce->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}
