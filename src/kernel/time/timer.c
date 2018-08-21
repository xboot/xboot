/*
 * kernel/time/timer.c
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

#include <clockevent/clockevent.h>
#include <clocksource/clocksource.h>
#include <time/timer.h>

static struct timer_base_t __timer_base = {
	.head = { NULL },
	.next = NULL,
	.ce = NULL,
	.lock = SPIN_LOCK_INIT(),
};

static inline struct timer_t * next_timer(struct timer_base_t * base)
{
	return base->next;
}

static inline int add_timer(struct timer_base_t * base, struct timer_t * timer)
{
	struct rb_node ** p = &base->head.rb_node;
	struct rb_node * parent = NULL;
	struct timer_t * ptr;

	if(timer->state != TIMER_STATE_INACTIVE)
		return 0;

	while(*p)
	{
		parent = *p;
		ptr = rb_entry(parent, struct timer_t, node);
		if(timer->expires.tv64 < ptr->expires.tv64)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&timer->node, parent, p);
	rb_insert_color(&timer->node, &base->head);

	if(!base->next || timer->expires.tv64 < base->next->expires.tv64)
		base->next = timer;

	timer->state = TIMER_STATE_ENQUEUED;
	return (timer == base->next);
}

static inline int del_timer(struct timer_base_t * base, struct timer_t * timer)
{
	int ret = 0;

	if(timer->state != TIMER_STATE_ENQUEUED)
		return 0;

	if(base->next == timer)
	{
		struct rb_node * rbn = rb_next(&timer->node);
		base->next = rbn ? rb_entry(rbn, struct timer_t, node) : NULL;
		ret = 1;
	}
	rb_erase(&timer->node, &base->head);
	RB_CLEAR_NODE(&timer->node);

	timer->state = TIMER_STATE_INACTIVE;
	return ret;
}

void timer_init(struct timer_t * timer, int (*function)(struct timer_t *, void *), void * data)
{
	if(timer)
	{
		memset(timer, 0, sizeof(struct timer_t));
		RB_CLEAR_NODE(&timer->node);
		timer->base = &__timer_base;
		timer->state = TIMER_STATE_INACTIVE;
		timer->data = data;
		timer->function = function;
	}
}

void timer_start(struct timer_t * timer, ktime_t now, ktime_t interval)
{
	struct timer_base_t * base = timer->base;
	irq_flags_t flags;

	if(!timer)
		return;

	spin_lock_irqsave(&base->lock, flags);
	if(del_timer(base, timer))
	{
		struct timer_t * next = next_timer(base);
		if(next)
			clockevent_set_event_next(base->ce, ktime_get(), next->expires);
	}
	ktime_t expires = ktime_add_safe(now, interval);
	memcpy(&timer->expires, &expires, sizeof(ktime_t));
	if(add_timer(base, timer))
		clockevent_set_event_next(base->ce, ktime_get(), timer->expires);
	spin_unlock_irqrestore(&base->lock, flags);
}

void timer_start_now(struct timer_t * timer, ktime_t interval)
{
	if(timer)
		timer_start(timer, ktime_get(), interval);
}

void timer_forward(struct timer_t * timer, ktime_t now, ktime_t interval)
{
	if(timer)
	{
		ktime_t expires = ktime_add_safe(now, interval);
		memcpy(&timer->expires, &expires, sizeof(ktime_t));
	}
}

void timer_forward_now(struct timer_t * timer, ktime_t interval)
{
	if(timer)
		timer_forward(timer, ktime_get(), interval);
}

void timer_cancel(struct timer_t * timer)
{
	struct timer_base_t * base = timer->base;
	irq_flags_t flags;

	if(!timer)
		return;

	spin_lock_irqsave(&base->lock, flags);
	if(del_timer(base, timer))
	{
		struct timer_t * next = next_timer(base);
		if(next)
			clockevent_set_event_next(base->ce, ktime_get(), next->expires);
	}
	spin_unlock_irqrestore(&base->lock, flags);
}

static void timer_event_handler(struct clockevent_t * ce, void * data)
{
	struct timer_base_t * base = (struct timer_base_t *)(data);
	struct timer_t * timer;
	ktime_t now = ktime_get();
	irq_flags_t flags;
	int restart;

	spin_lock_irqsave(&base->lock, flags);
	while((timer = next_timer(base)))
	{
		if(now.tv64 < timer->expires.tv64)
			break;

		del_timer(base, timer);
		timer->state = TIMER_STATE_CALLBACK;
		restart = timer->function(timer, timer->data);
		timer->state = TIMER_STATE_INACTIVE;
		if(restart)
			add_timer(base, timer);
	}
	if((timer = next_timer(base)))
		clockevent_set_event_next(ce, now, timer->expires);
	spin_unlock_irqrestore(&base->lock, flags);
}

void timer_bind_clockevent(struct clockevent_t * ce)
{
	irq_flags_t flags;

	if(ce)
	{
		spin_lock_irqsave(&__timer_base.lock, flags);
		__timer_base.head = RB_ROOT;
		__timer_base.next = NULL;
		__timer_base.ce = ce;
		clockevent_set_event_handler(__timer_base.ce, timer_event_handler, &__timer_base);
		spin_unlock_irqrestore(&__timer_base.lock, flags);
	}
}
