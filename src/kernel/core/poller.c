/*
 * kernel/core/poller.c
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
#include <xboot/poller.h>

struct poller_list_t
{
	struct poller_t * poller;
	struct list_head entry;
};

struct poller_list_t __poller_list = {
	.entry = {
		.next	= &(__poller_list.entry),
		.prev	= &(__poller_list.entry),
	},
};
static struct list_head * __poller_head = &(__poller_list.entry);
static spinlock_t __poller_list_lock = SPIN_LOCK_INIT();

struct poller_t * poller_create(u64_t interval, void (*func)(struct poller_t *, void *), void * data)
{
	struct poller_t * poller;
	struct poller_list_t * pl;

	if(!func)
		return NULL;

	poller = malloc(sizeof(struct poller_t));
	if(!poller)
		return NULL;

	pl = malloc(sizeof(struct poller_list_t));
	if(!pl)
	{
		free(poller);
		return NULL;
	}

	poller->func = func;
	poller->data = data;
	poller->interval = interval;
	poller->timeout = 0;
	pl->poller = poller;

	spin_lock_irq(&__poller_list_lock);
	list_add_tail(&pl->entry, &(__poller_list.entry));
	spin_unlock_irq(&__poller_list_lock);

	return poller;
}

bool_t poller_destroy(struct poller_t * poller)
{
	struct poller_list_t * pos, * n;

	if(!poller)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__poller_list.entry), entry)
	{
		if(pos->poller == poller)
		{
			spin_lock_irq(&__poller_list_lock);
			if(__poller_head == &(pos->entry))
				__poller_head = (&(pos->entry))->prev;
			list_del(&(pos->entry));
			spin_unlock_irq(&__poller_list_lock);

			free(pos->poller);
			free(pos);
			return TRUE;
		}
	}
	return FALSE;
}

void schedule_poller_yield(void)
{
	struct poller_list_t * pl;
	struct poller_t * poller;
	u64_t timeus;

	if(list_empty(&(__poller_list.entry)))
		return;

	do {
		__poller_head = __poller_head->next;
	} while(__poller_head == &(__poller_list.entry));

	pl = list_entry(__poller_head, struct poller_list_t, entry);
	poller = pl->poller;
	timeus = clocksource_gettime();
	if(timeus >= poller->timeout)
	{
		poller->timeout = timeus + poller->interval;
		poller->func(poller, poller->data);
	}
}
