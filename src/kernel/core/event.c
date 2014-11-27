/*
 * kernel/core/event.c
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
	struct event_base_t * eb;

	eb = malloc(sizeof(struct event_base_t));
	if(!eb)
		return NULL;

	eb->fifo = fifo_alloc(sizeof(struct event_t) * CONFIG_EVENT_FIFO_LENGTH);
	if(!eb->fifo)
	{
		free(eb);
		return NULL;
	}

	spin_lock_irq(&__event_base_lock);
	list_add_tail(&eb->entry, &(__event_base.entry));
	spin_unlock_irq(&__event_base_lock);

	return eb;
}

void __event_base_free(struct event_base_t * eb)
{
	struct event_base_t * ebpos, * ebn;

	if(!eb)
		return;

	list_for_each_entry_safe(ebpos, ebn, &(__event_base.entry), entry)
	{
		if(ebpos == eb)
		{
			spin_lock_irq(&__event_base_lock);
			list_del(&(ebpos->entry));
			spin_unlock_irq(&__event_base_lock);

			if(ebpos->fifo)
				fifo_free(ebpos->fifo);
			free(ebpos);
		}
	}
}

void push_event(struct event_t * event)
{
	struct event_base_t * pos, * n;

	if(!event)
		return;

	event->timestamp = jiffies;

	list_for_each_entry_safe(pos, n, &(__event_base.entry), entry)
	{
		fifo_put(pos->fifo, (u8_t *)event, sizeof(struct event_t));
	}
}

void push_event_mouse_button_down(void * device, s32_t x, s32_t y, u32_t btn)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_DOWN;
	event.e.mouse_down.x = x;
	event.e.mouse_down.y = y;
	event.e.mouse_down.button = btn;
	push_event(&event);
}

void push_event_mouse_button_up(void * device, s32_t x, s32_t y, u32_t btn)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_UP;
	event.e.mouse_up.x = x;
	event.e.mouse_up.y = y;
	event.e.mouse_up.button = btn;
	push_event(&event);
}

void push_event_mouse_move(void * device, s32_t x, s32_t y)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_MOVE;
	event.e.mouse_move.x = x;
	event.e.mouse_move.y = y;
	push_event(&event);
}

void push_event_mouse_wheel(void * device, s32_t dx, s32_t dy)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_WHEEL;
	event.e.mouse_wheel.dx = dx;
	event.e.mouse_wheel.dy = dy;
	push_event(&event);
}

void push_event_touches_begin(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCHES_BEGIN;
	event.e.touches.x = x;
	event.e.touches.y = y;
	event.e.touches.id = id;
	push_event(&event);
}

void push_event_touches_move(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCHES_MOVE;
	event.e.touches.x = x;
	event.e.touches.y = y;
	event.e.touches.id = id;
	push_event(&event);
}

void push_event_touches_end(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCHES_END;
	event.e.touches.x = x;
	event.e.touches.y = y;
	event.e.touches.id = id;
	push_event(&event);
}

void push_event_touches_cancel(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCHES_CANCEL;
	event.e.touches.x = x;
	event.e.touches.y = y;
	event.e.touches.id = id;
	push_event(&event);
}

bool_t pump_event(struct event_base_t * eb, struct event_t * event)
{
	bool_t ret;

	if(!eb || !event)
		return FALSE;

	spin_lock_irq(&__event_base_lock);
	ret = (fifo_get(eb->fifo, (u8_t *)event, sizeof(struct event_t)) == sizeof(struct event_t));
	spin_unlock_irq(&__event_base_lock);

	return ret;
}
