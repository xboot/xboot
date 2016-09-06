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

/*
 * Dummy clockevent, 1us - 1MHZ
 */
static void __ce_dummy_handler(struct clockevent_t * ce, void * data)
{
}

static bool_t __ce_dummy_next(struct clockevent_t * ce, u64_t evt)
{
	return TRUE;
}

static struct clockevent_t __ce_dummy = {
	.name = "ce-dummy",
	.mult = 4294967,
	.shift = 32,
	.min_delta_ns = 1000,
	.max_delta_ns = 4294967591000,
	.data = NULL,
	.handler = __ce_dummy_handler,
	.next = __ce_dummy_next,
};
static struct clockevent_t * __clockevent = &__ce_dummy;
static spinlock_t __clockevent_lock = SPIN_LOCK_INIT();

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
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_CLOCKEVENT);
	if(!dev)
		return NULL;

	return (struct clockevent_t *)dev->priv;
}

struct clockevent_t * search_first_clockevent(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_CLOCKEVENT);
	if(!dev)
		return NULL;

	return (struct clockevent_t *)dev->priv;
}

bool_t register_clockevent(struct device_t ** device, struct clockevent_t * ce)
{
	struct device_t * dev;
	irq_flags_t flags;

	if(!ce || !ce->name || !ce->next)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	ce->data = NULL;
	ce->handler = __ce_dummy_handler;

	dev->name = strdup(ce->name);
	dev->type = DEVICE_TYPE_CLOCKEVENT;
	dev->priv = ce;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "mult", clockevent_read_mult, NULL, ce);
	kobj_add_regular(dev->kobj, "shift", clockevent_read_shift, NULL, ce);
	kobj_add_regular(dev->kobj, "frequency", clockevent_read_frequency, NULL, ce);
	kobj_add_regular(dev->kobj, "min_delta", clockevent_read_min_delta, NULL, ce);
	kobj_add_regular(dev->kobj, "max_delta", clockevent_read_max_delta, NULL, ce);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(__clockevent == &__ce_dummy)
	{
		spin_lock_irqsave(&__clockevent_lock, flags);
		__clockevent = ce;
		timer_bind_clockevent(__clockevent);
		spin_unlock_irqrestore(&__clockevent_lock, flags);
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_clockevent(struct clockevent_t * ce)
{
	struct device_t * dev;
	struct clockevent_t * c;
	irq_flags_t flags;

	if(!ce || !ce->name || !ce->next)
		return FALSE;

	dev = search_device(ce->name, DEVICE_TYPE_CLOCKEVENT);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	if(__clockevent == ce)
	{
		if(!(c = search_first_clockevent()))
			c = &__ce_dummy;

		spin_lock_irqsave(&__clockevent_lock, flags);
		__clockevent = c;
		timer_bind_clockevent(__clockevent);
		spin_unlock_irqrestore(&__clockevent_lock, flags);
	}

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
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
