/*
 * driver/clockevent/clockevent.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

/*
 * Dummy clockevent, 1us - 1MHZ
 */
static void __ce_dummy_handler(struct clockevent_t * ce, void * data)
{
}

static bool_t __ce_dummy_init(struct clockevent_t * ce)
{
	return TRUE;
}

static bool_t __ce_dummy_next(struct clockevent_t * ce, u64_t evt)
{
	return TRUE;
}

static struct clockevent_t __ce_dummy = {
	.name			= "dummy",
	.mult			= 4294967,
	.shift			= 32,
	.min_delta_ns	= 1000,
	.max_delta_ns	= 4294967591000,
	.data			= NULL,
	.handler		= __ce_dummy_handler,
	.init			= __ce_dummy_init,
	.next			= __ce_dummy_next,
};

static struct kobj_t * search_class_clockevent_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "clockevent");
}

static ssize_t clockevent_read_mult(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clockevent_t * ce = (struct clockevent_t *)kobj->priv;
	return sprintf(buf, "%u", ce->mult);
}

static ssize_t clockevent_read_shift(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clockevent_t * ce = (struct clockevent_t *)kobj->priv;
	return sprintf(buf, "%u", ce->shift);
}

static ssize_t clockevent_read_frequency(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clockevent_t * ce = (struct clockevent_t *)kobj->priv;
	u64_t freq = ((u64_t)1000000000ULL * ce->mult) >> ce->shift;
	return sprintf(buf, "%llu", freq);
}

static ssize_t clockevent_read_min_delta(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clockevent_t * ce = (struct clockevent_t *)kobj->priv;
	return sprintf(buf, "%llu.%09llu", ce->min_delta_ns / 1000000000ULL, ce->min_delta_ns % 1000000000ULL);
}

static ssize_t clockevent_read_max_delta(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clockevent_t * ce = (struct clockevent_t *)kobj->priv;
	return sprintf(buf, "%llu.%09llu", ce->max_delta_ns / 1000000000ULL, ce->max_delta_ns % 1000000000ULL);
}

struct clockevent_t * search_clockevent(const char * name)
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
	irq_flags_t flags;

	if(!ce || !ce->name)
		return FALSE;

	if(!ce->init || !ce->next)
		return FALSE;

	if(search_clockevent(ce->name))
		return FALSE;

	cl = malloc(sizeof(struct clockevent_list_t));
	if(!cl)
		return FALSE;

	ce->data = NULL;
	ce->handler = __ce_dummy_handler;
	ce->kobj = kobj_alloc_directory(ce->name);
	kobj_add_regular(ce->kobj, "mult", clockevent_read_mult, NULL, ce);
	kobj_add_regular(ce->kobj, "shift", clockevent_read_shift, NULL, ce);
	kobj_add_regular(ce->kobj, "frequency", clockevent_read_frequency, NULL, ce);
	kobj_add_regular(ce->kobj, "min_delta", clockevent_read_min_delta, NULL, ce);
	kobj_add_regular(ce->kobj, "max_delta", clockevent_read_max_delta, NULL, ce);
	kobj_add(search_class_clockevent_kobj(), ce->kobj);
	cl->ce = ce;

	spin_lock_irqsave(&__clockevent_list_lock, flags);
	list_add_tail(&cl->entry, &(__clockevent_list.entry));
	spin_unlock_irqrestore(&__clockevent_list_lock, flags);

	return TRUE;
}

bool_t unregister_clockevent(struct clockevent_t * ce)
{
	struct clockevent_list_t * pos, * n;
	irq_flags_t flags;

	if(!ce || !ce->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__clockevent_list.entry), entry)
	{
		if(pos->ce == ce)
		{
			spin_lock_irqsave(&__clockevent_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__clockevent_list_lock, flags);

			kobj_remove(search_class_clockevent_kobj(), pos->ce->kobj);
			kobj_remove_self(ce->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

struct clockevent_t * clockevent_best(void)
{
	struct clockevent_t * ce, * best = &__ce_dummy;
	struct clockevent_list_t * pos, * n;
	u64_t freq = 0;
	u64_t f;

	list_for_each_entry_safe(pos, n, &(__clockevent_list.entry), entry)
	{
		ce = pos->ce;
		if(!ce || !ce->init)
			continue;

		if(!ce->init(ce))
			continue;

		f = ((u64_t)1000000000ULL * ce->mult) >> ce->shift;
		if(f > freq)
		{
			best = ce;
			freq = f;
		}
	}
	return best;
}

bool_t clockevent_set_event_handler(struct clockevent_t * ce, void (*handler)(struct clockevent_t *, void *), void * data)
{
	if(!ce)
		return FALSE;
	ce->data = data;
	ce->handler = handler ? handler : __ce_dummy_handler;
	return TRUE;
}

bool_t clockevent_set_event_next(struct clockevent_t * ce, ktime_t now, ktime_t expires)
{
	u64_t delta;

	if(!ce)
		return FALSE;

	if(ktime_before(expires, now))
		return FALSE;

	delta = ktime_to_ns(ktime_sub(expires, now));
	if(delta > ce->max_delta_ns)
		delta = ce->max_delta_ns;
	if(delta < ce->min_delta_ns)
		delta = ce->min_delta_ns;
	return ce->next(ce, ((u64_t)delta * ce->mult) >> ce->shift);
}
