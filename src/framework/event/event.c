/*
 * framework/event/event.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
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

#include <spinlock.h>
#include <framework/event/event.h>

static struct event_base_t __event_base = {
	.entry = {
		.next	= &(__event_base.entry),
		.prev	= &(__event_base.entry),
	},
};
static spinlock_t __event_base_lock;

struct event_base_t * __event_base_alloc(void)
{
	struct event_base_t * eb;

	eb = malloc(sizeof(struct event_base_t));
	if(!eb)
		return NULL;

	spin_lock_irq(&__event_base_lock);

	eb->fifo = fifo_alloc(sizeof(struct event_t) * CONFIG_EVENT_FIFO_LENGTH);
	if(!eb->fifo)
	{
		free(eb);
		spin_unlock_irq(&__event_base_lock);
		return NULL;
	}
	list_add_tail(&eb->entry, &(__event_base.entry));

	spin_unlock_irq(&__event_base_lock);

	return eb;
}

void __event_base_free(struct event_base_t * eb)
{
	struct event_base_t * pos, * n;

	if(!eb)
		return;

	spin_lock_irq(&__event_base_lock);

	list_for_each_entry_safe(pos, n, &(__event_base.entry), entry)
	{
		if(pos == eb)
		{
			if(eb->fifo)
				fifo_free(eb->fifo);
			list_del(&(eb->entry));
			free(eb);
		}
	}

	spin_unlock_irq(&__event_base_lock);
}

bool_t event_push(struct event_t * event)
{
	struct event_base_t * pos, * n;

	if(!event)
		return FALSE;

	spin_lock_irq(&__event_base_lock);

	list_for_each_entry_safe(pos, n, &(__event_base.entry), entry)
	{
		fifo_put(pos->fifo, (u8_t *)event, sizeof(struct event_t));
	}

	spin_unlock_irq(&__event_base_lock);

	return TRUE;
}

bool_t event_pop(struct event_t * event)
{
	bool_t ret;

	if(!event)
		return FALSE;

	spin_lock_irq(&__event_base_lock);
	ret = fifo_get(runtime_get()->__event_base->fifo, (u8_t *)event, sizeof(struct event_t)) == sizeof(struct event_t);
	spin_unlock_irq(&__event_base_lock);

	return ret;
}

bool_t event_flush(void)
{
	spin_lock_irq(&__event_base_lock);
	fifo_reset(runtime_get()->__event_base->fifo);
	spin_unlock_irq(&__event_base_lock);

	return TRUE;
}

static __init void event_init(void)
{
	init_list_head(&(__event_base.entry));
	spin_lock_init(&__event_base_lock);
}

static __exit void event_exit(void)
{
}

pure_initcall(event_init);
pure_exitcall(event_exit);
