/*
 * kernel/time/timer.c
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
#include <types.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <time/tick.h>
#include <time/timer.h>


/*
 * event timer code
 */
#define TVN_BITS 				(6)
#define TVR_BITS 				(8)
#define TVN_SIZE 				(1 << TVN_BITS)
#define TVR_SIZE 				(1 << TVR_BITS)
#define TVN_MASK 				(TVN_SIZE - 1)
#define TVR_MASK 				(TVR_SIZE - 1)

struct timer_vec {
	u32_t index;
	struct list_head vec[TVN_SIZE];
};

struct timer_vec_root {
	u32_t index;
	struct list_head vec[TVR_SIZE];
};

static u32_t timer_jiffies = 0;

static struct timer_vec tv5;
static struct timer_vec tv4;
static struct timer_vec tv3;
static struct timer_vec tv2;
static struct timer_vec_root tv1;

static struct timer_vec * const tvecs[] = {
	(struct timer_vec *)&tv1, &tv2, &tv3, &tv4, &tv5
};

static inline void internal_add_timer(struct timer_list *timer)
{
	struct list_head * vec;
	u32_t expires = timer->expires;
	u32_t idx = expires - timer_jiffies;
	u32_t i;

	if(idx < TVR_SIZE)
	{
		i = expires & TVR_MASK;
		vec = tv1.vec + i;
	}
	else if(idx < 1 << (TVR_BITS + TVN_BITS))
	{
		i = (expires >> TVR_BITS) & TVN_MASK;
		vec = tv2.vec + i;
	}
	else if(idx < 1 << (TVR_BITS + 2 * TVN_BITS))
	{
		i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
		vec = tv3.vec + i;
	}
	else if(idx < 1 << (TVR_BITS + 3 * TVN_BITS))
	{
		i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
		vec = tv4.vec + i;
	}
	else if((s32_t) idx < 0)
	{
		/*
		 * can happen if you add a timer with expires == jiffies,
		 * or you set a timer to go off in the past
		 */
		vec = tv1.vec + tv1.index;
	}
	else if(idx <= 0xffffffffUL)
	{
		i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
		vec = tv5.vec + i;
	}
	else
	{
		/* can only get here on architectures with 64-bit jiffies */
		init_list_head(&timer->list);
		return;
	}
	/*
	 * timers are fifo
	 */
	list_add(&timer->list, vec->prev);
}

static inline bool_t detach_timer(struct timer_list *timer)
{
	if (!timer_pending(timer))
		return FALSE;

	list_del(&timer->list);
	return TRUE;
}

static inline void cascade_timers(struct timer_vec *tv)
{
	/* cascade all the timers from tv up one level */
	struct list_head *head, *curr, *next;
	struct timer_list *tmp;

	head = tv->vec + tv->index;
	curr = head->next;

	/*
	 * we are removing _all_ timers from the list, so we don't have to
	 * detach them individually, just clear the list afterwards.
	 */
	while(curr != head)
	{
		tmp = list_entry(curr, struct timer_list, list);
		next = curr->next;
		list_del(curr);
		internal_add_timer(tmp);
		curr = next;
	}
	init_list_head(head);
	tv->index = (tv->index + 1) & TVN_MASK;
}

/*
 * exec task in timer list.
 */
void exec_timer_task(void)
{
	struct list_head *head, *curr;
	struct timer_list * timer;
	void (*fn)(u32_t);
	u32_t data;
	s32_t n = 1;

	while((s32_t)(jiffies - timer_jiffies) >= 0)
	{
		if(!tv1.index)
		{
			do {
				cascade_timers(tvecs[n]);
			} while (tvecs[n]->index == 1 && ++n < (sizeof(tvecs) / sizeof(tvecs[0])));
		}
repeat:
		head = tv1.vec + tv1.index;
		curr = head->next;
		if(curr != head)
		{
			timer = list_entry(curr, struct timer_list, list);
			fn = timer->function;
			data= timer->data;

			detach_timer(timer);
			timer->list.next = timer->list.prev = NULL;
			fn(data);
			goto repeat;
		}
		++timer_jiffies;
		tv1.index = (tv1.index + 1) & TVR_MASK;
	}
}

/*
 * intial timer
 */
void init_timer(struct timer_list * timer)
{
	timer->list.next = timer->list.prev = NULL;
}

/*
 * timer pending.
 */
bool_t timer_pending(const struct timer_list * timer)
{
	return timer->list.next != NULL;
}

/*
 * add timer
 */
void add_timer(struct timer_list *timer)
{
	if(timer_pending(timer))
		return;

	internal_add_timer(timer);
}

/*
 * modfied timer
 */
bool_t mod_timer(struct timer_list *timer, u32_t expires)
{
	bool_t ret;

	timer->expires = expires;

	ret = detach_timer(timer);
	internal_add_timer(timer);

	return ret;
}

/*
 * delete timer
 */
bool_t del_timer(struct timer_list * timer)
{
	bool_t ret;

	ret = detach_timer(timer);
	timer->list.next = timer->list.prev = NULL;

	return ret;
}

/*
 * setup timer.
 */
void setup_timer(struct timer_list * timer,	void (*function)(u32_t), u32_t data)
{
	timer->function = function;
	timer->data = data;
	init_timer(timer);
}

static __init void timer_pure_init(void)
{
	s32_t i;

	timer_jiffies = 0;

	for(i = 0; i < TVN_SIZE; i++)
	{
		init_list_head(tv5.vec + i);
		init_list_head(tv4.vec + i);
		init_list_head(tv3.vec + i);
		init_list_head(tv2.vec + i);
	}

	for(i = 0; i < TVR_SIZE; i++)
	{
		init_list_head(tv1.vec + i);
	}
}
pure_initcall(timer_pure_init);
