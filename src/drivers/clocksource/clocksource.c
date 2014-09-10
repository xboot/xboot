/*
 * driver/clocksource/clocksource.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <clocksource/clocksource.h>

struct clocksource_list_t
{
	struct clocksource_t * cs;
	struct list_head entry;
};

struct clocksource_list_t __clocksource_list = {
	.entry = {
		.next	= &(__clocksource_list.entry),
		.prev	= &(__clocksource_list.entry),
	},
};
static spinlock_t __clocksource_list_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_class_clocksource_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "clocksource");
}

struct clocksource_t * search_clocksource(const char * name)
{
	struct clocksource_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		if(strcmp(pos->cs->name, name) == 0)
			return pos->cs;
	}

	return NULL;
}

bool_t register_clocksource(struct clocksource_t * cs)
{
	struct clocksource_list_t * cl;

	if(!cs || !cs->name)
		return FALSE;

	if(search_clocksource(cs->name))
		return FALSE;

	cl = malloc(sizeof(struct clocksource_list_t));
	if(!cl)
		return FALSE;

	cs->kobj = kobj_alloc_directory(cs->name);
	kobj_add(search_class_clocksource_kobj(), cs->kobj);
	cl->cs = cs;

	spin_lock_irq(&__clocksource_list_lock);
	list_add_tail(&cl->entry, &(__clocksource_list.entry));
	spin_unlock_irq(&__clocksource_list_lock);

	return TRUE;
}

bool_t unregister_clocksource(struct clocksource_t * cs)
{
	struct clocksource_list_t * pos, * n;

	if(!cs || !cs->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		if(pos->cs == cs)
		{
			spin_lock_irq(&__clocksource_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__clocksource_list_lock);

			kobj_remove(search_class_clocksource_kobj(), pos->cs->kobj);
			kobj_remove_self(cs->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}
