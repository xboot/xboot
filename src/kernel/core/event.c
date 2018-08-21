/*
 * kernel/core/event.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
	irq_flags_t flags;

	eb = malloc(sizeof(struct event_base_t));
	if(!eb)
		return NULL;

	eb->fifo = fifo_alloc(sizeof(struct event_t) * CONFIG_EVENT_FIFO_LENGTH);
	if(!eb->fifo)
	{
		free(eb);
		return NULL;
	}

	spin_lock_irqsave(&__event_base_lock, flags);
	list_add_tail(&eb->entry, &(__event_base.entry));
	spin_unlock_irqrestore(&__event_base_lock, flags);

	return eb;
}

void __event_base_free(struct event_base_t * eb)
{
	struct event_base_t * ebpos, * ebn;
	irq_flags_t flags;

	if(!eb)
		return;

	list_for_each_entry_safe(ebpos, ebn, &(__event_base.entry), entry)
	{
		if(ebpos == eb)
		{
			spin_lock_irqsave(&__event_base_lock, flags);
			list_del(&(ebpos->entry));
			spin_unlock_irqrestore(&__event_base_lock, flags);

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

	event->timestamp = ktime_get();

	list_for_each_entry_safe(pos, n, &(__event_base.entry), entry)
	{
		fifo_put(pos->fifo, (u8_t *)event, sizeof(struct event_t));
	}
}

void push_event_key_down(void * device, u32_t key)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_KEY_DOWN;
	event.e.key_down.key = key;
	push_event(&event);
}

void push_event_key_up(void * device, u32_t key)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_KEY_UP;
	event.e.key_up.key = key;
	push_event(&event);
}

void push_event_rotary_turn(void * device, s32_t v)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_ROTARY_TURN;
	event.e.rotary_turn.v = v;
	push_event(&event);
}

void push_event_rotary_switch(void * device, s32_t v)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_ROTARY_SWITCH;
	event.e.rotary_switch.v = v;
	push_event(&event);
}

void push_event_mouse_button_down(void * device, s32_t x, s32_t y, u32_t button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_DOWN;
	event.e.mouse_down.x = x;
	event.e.mouse_down.y = y;
	event.e.mouse_down.button = button;
	push_event(&event);
}

void push_event_mouse_button_up(void * device, s32_t x, s32_t y, u32_t button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_MOUSE_UP;
	event.e.mouse_up.x = x;
	event.e.mouse_up.y = y;
	event.e.mouse_up.button = button;
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

void push_event_touch_begin(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCH_BEGIN;
	event.e.touch_begin.x = x;
	event.e.touch_begin.y = y;
	event.e.touch_begin.id = id;
	push_event(&event);
}

void push_event_touch_move(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCH_MOVE;
	event.e.touch_move.x = x;
	event.e.touch_move.y = y;
	event.e.touch_move.id = id;
	push_event(&event);
}

void push_event_touch_end(void * device, s32_t x, s32_t y, u32_t id)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_TOUCH_END;
	event.e.touch_end.x = x;
	event.e.touch_end.y = y;
	event.e.touch_end.id = id;
	push_event(&event);
}

void push_event_joystick_left_stick(void * device, s32_t x, s32_t y)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_LEFTSTICK;
	event.e.joystick_left_stick.x = x;
	event.e.joystick_left_stick.y = y;
	push_event(&event);
}

void push_event_joystick_right_stick(void * device, s32_t x, s32_t y)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_RIGHTSTICK;
	event.e.joystick_right_stick.x = x;
	event.e.joystick_right_stick.y = y;
	push_event(&event);
}

void push_event_joystick_left_trigger(void * device, s32_t v)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_LEFTTRIGGER;
	event.e.joystick_left_trigger.v = v;
	push_event(&event);
}

void push_event_joystick_right_trigger(void * device, s32_t v)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_RIGHTTRIGGER;
	event.e.joystick_left_trigger.v = v;
	push_event(&event);
}

void push_event_joystick_button_down(void * device, u32_t button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_BUTTONDOWN;
	event.e.joystick_button_down.button = button;
	push_event(&event);
}

void push_event_joystick_button_up(void * device, u32_t button)
{
	struct event_t event;

	event.device = device;
	event.type = EVENT_TYPE_JOYSTICK_BUTTONUP;
	event.e.joystick_button_down.button = button;
	push_event(&event);
}

bool_t pump_event(struct event_base_t * eb, struct event_t * event)
{
	irq_flags_t flags;
	bool_t ret;

	if(!eb || !event)
		return FALSE;

	spin_lock_irqsave(&__event_base_lock, flags);
	ret = (fifo_get(eb->fifo, (u8_t *)event, sizeof(struct event_t)) == sizeof(struct event_t));
	spin_unlock_irqrestore(&__event_base_lock, flags);

	return ret;
}
