/*
 * kernel/core/event.c
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

#include <fifo.h>
#include <spinlock.h>
#include <xboot/event.h>

static struct event_base_t __event_base = {
	.entry = {
		.next	= &(__event_base.entry),
		.prev	= &(__event_base.entry),
	},
};
static spinlock_t __event_base_lock = SPIN_LOCK_INIT();

struct event_base_t * __event_base_alloc(void)
{
	return NULL;
#if 0
	struct event_base_t * eb;

	eb = malloc(sizeof(struct event_base_t));
	if(!eb)
		return NULL;

	eb->fifo = fifo_alloc(sizeof(struct event_t) * CONFIG_EVENT_FIFO_LENGTH);
	eb->listener = malloc(sizeof(struct event_listener_t));
	if(!eb->fifo || !eb->listener)
	{
		if(eb->fifo)
			fifo_free(eb->fifo);
		if(eb->listener)
			free(eb->listener);
		free(eb);
		return NULL;
	}

	spin_lock_irq(&__event_base_lock);
	init_list_head(&(eb->listener->entry));
	list_add_tail(&eb->entry, &(__event_base.entry));
	spin_unlock_irq(&__event_base_lock);

	return eb;
#endif
}

void __event_base_free(struct event_base_t * eb)
{
#if 0
	struct event_base_t * ebpos, * ebn;
	struct event_listener_t * elpos, * eln;

	if(!eb)
		return;

	spin_lock_irq(&__event_base_lock);
	list_for_each_entry_safe(ebpos, ebn, &(__event_base.entry), entry)
	{
		if(ebpos == eb)
		{
			if(ebpos->fifo)
				fifo_free(ebpos->fifo);

			if(ebpos->listener)
			{
				list_for_each_entry_safe(elpos, eln, &(ebpos->listener->entry), entry)
				{
					list_del(&(elpos->entry));
				}
				free(eb->listener);
			}

			list_del(&(ebpos->entry));
			free(ebpos);
		}
	}
	spin_unlock_irq(&__event_base_lock);
#endif
}

struct event_listener_t * event_listener_alloc(enum event_type_t type, 	event_listener_callback_t callback, void * data)
{
	return NULL;
#if 0
	struct event_listener_t * el;

	el = malloc(sizeof(struct event_listener_t));
	if(!el)
		return NULL;

	el->type = type;
	el->callback = callback;
	el->data = data;

	return el;
#endif
}

void event_listener_free(struct event_listener_t * el)
{
#if 0
	if(el)
		free(el);
#endif
}

bool_t event_base_add_event_listener(struct event_base_t * eb, struct event_listener_t * el)
{
	return FALSE;
#if 0
	struct event_listener_t * elpos, * eln;

	if(!el || !eb || !eb->listener)
		return FALSE;

	spin_lock_irq(&__event_base_lock);
	list_for_each_entry_safe(elpos, eln, &(eb->listener->entry), entry)
	{
		if(elpos == el)
		{
			spin_unlock_irq(&__event_base_lock);
			return FALSE;
		}
	}

	list_add_tail(&el->entry, &(eb->listener->entry));
	spin_unlock_irq(&__event_base_lock);

	return TRUE;
#endif
}

bool_t event_base_del_event_listener(struct event_base_t * eb, struct event_listener_t * el)
{
	return FALSE;
#if 0
	struct event_listener_t * elpos, * eln;

	if(!el || !eb || !eb->listener)
		return FALSE;

	spin_lock_irq(&__event_base_lock);
	list_for_each_entry_safe(elpos, eln, &(eb->listener->entry), entry)
	{
		if(elpos == el)
		{
			list_del(&(elpos->entry));
		}
	}
	spin_unlock_irq(&__event_base_lock);

	return TRUE;
#endif
}

bool_t event_base_dispatcher(struct event_base_t * eb)
{
	return FALSE;
#if 0
	struct event_listener_t * elpos, * eln, * el;
	struct event_t event;
	bool_t ret;

	if(!eb || !eb->listener)
		return FALSE;

	spin_lock_irq(&__event_base_lock);
	ret = (fifo_get(eb->fifo, (u8_t *)&event, sizeof(struct event_t)) == sizeof(struct event_t));
	if(ret)
	{
		el = eb->listener;
		list_for_each_entry_safe(elpos, eln, &(el->entry), entry)
		{
			if(elpos->type == event.type)
			{
				if(elpos->callback)
				{
					elpos->callback(&event, elpos->data);
				}
			}
		}
	}
	spin_unlock_irq(&__event_base_lock);

	return ret;
#endif
}

bool_t event_send(struct event_t * event)
{
	return FALSE;
#if 0
	struct event_base_t * pos, * n;

	if(!event)
		return FALSE;

	event->timestamp = jiffies;

	//spin_lock_irq(&__event_base_lock);
	list_for_each_entry_safe(pos, n, &(__event_base.entry), entry)
	{
		fifo_put(pos->fifo, (u8_t *)event, sizeof(struct event_t));
	}
	//spin_unlock_irq(&__event_base_lock);

	return TRUE;
#endif
}
