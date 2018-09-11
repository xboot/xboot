/*
 * driver/clocksource/clocksource.c
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

#include <clocksource/clocksource.h>

/*
 * Dummy clocksource, 10us - 100KHZ
 */
static u64_t __cs_dummy_read(struct clocksource_t * cs)
{
	static volatile u64_t __cs_dummy_cycle = 0;
	return __cs_dummy_cycle++;
}

static struct clocksource_t __cs_dummy = {
	.keeper = {
		.interval = 35184372083832,
		.last = 0,
		.nsec = 0,
		.lock = { 0 },
	},
	.name = "cs-dummy",
	.mask = CLOCKSOURCE_MASK(64),
	.mult = 2621440000,
	.shift = 18,
	.read = __cs_dummy_read,
};
static struct clocksource_t * __clocksource = &__cs_dummy;
static spinlock_t __clocksource_lock = SPIN_LOCK_INIT();

static ssize_t clocksource_read_mult(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%u", cs->mult);
}

static ssize_t clocksource_read_shift(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%u", cs->shift);
}

static ssize_t clocksource_read_period(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t period = ((u64_t)cs->mult) >> cs->shift;
	return sprintf(buf, "%llu.%09llu", period / 1000000000ULL, period % 1000000000ULL);
}

static ssize_t clocksource_read_deferment(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t max = clocksource_deferment(cs);
	return sprintf(buf, "%llu.%09llu", max / 1000000000ULL, max % 1000000000ULL);
}

static ssize_t clocksource_read_cycle(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%llu", clocksource_cycle(cs));
}

static ssize_t clocksource_read_time(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t cycle = clocksource_cycle(cs);
	u64_t time = clocksource_delta2ns(cs, cycle);
	return sprintf(buf, "%llu.%09llu", time / 1000000000ULL, time % 1000000000ULL);
}

static int clocksource_keeper_timer_function(struct timer_t * timer, void * data)
{
	struct clocksource_t * cs = (struct clocksource_t *)(data);
	u64_t now, delta, offset;
	irq_flags_t flags;

	write_seqlock_irqsave(&cs->keeper.lock, flags);
	now = clocksource_cycle(cs);
	delta = clocksource_delta(cs, cs->keeper.last, now);
	offset = clocksource_delta2ns(cs, delta);
	cs->keeper.nsec += offset;
	cs->keeper.last = now;
	write_sequnlock_irqrestore(&cs->keeper.lock, flags);

	timer_forward_now(timer, ns_to_ktime(cs->keeper.interval));
	return 1;
}

struct clocksource_t * search_clocksource(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_CLOCKSOURCE);
	if(!dev)
		return NULL;
	return (struct clocksource_t *)dev->priv;
}

struct clocksource_t * search_first_clocksource(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_CLOCKSOURCE);
	if(!dev)
		return NULL;
	return (struct clocksource_t *)dev->priv;
}

bool_t register_clocksource(struct device_t ** device, struct clocksource_t * cs)
{
	struct device_t * dev;
	irq_flags_t flags;

	if(!cs || !cs->name || !cs->read)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	cs->keeper.interval = clocksource_deferment(cs) >> 1;
	cs->keeper.last = clocksource_cycle(cs);
	cs->keeper.nsec = 0;
	seqlock_init(&cs->keeper.lock);
	timer_init(&cs->keeper.timer, clocksource_keeper_timer_function, cs);

	dev->name = strdup(cs->name);
	dev->type = DEVICE_TYPE_CLOCKSOURCE;
	dev->driver = NULL;
	dev->priv = cs;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "mult", clocksource_read_mult, NULL, cs);
	kobj_add_regular(dev->kobj, "shift", clocksource_read_shift, NULL, cs);
	kobj_add_regular(dev->kobj, "period", clocksource_read_period, NULL, cs);
	kobj_add_regular(dev->kobj, "deferment", clocksource_read_deferment, NULL, cs);
	kobj_add_regular(dev->kobj, "cycle", clocksource_read_cycle, NULL, cs);
	kobj_add_regular(dev->kobj, "time", clocksource_read_time, NULL, cs);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(__clocksource == &__cs_dummy)
	{
		spin_lock_irqsave(&__clocksource_lock, flags);
		__clocksource = cs;
		spin_unlock_irqrestore(&__clocksource_lock, flags);
	}
	timer_start_now(&cs->keeper.timer, ns_to_ktime(cs->keeper.interval));

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_clocksource(struct clocksource_t * cs)
{
	struct device_t * dev;
	struct clocksource_t * c;
	irq_flags_t flags;

	if(!cs || !cs->name || !cs->read)
		return FALSE;

	dev = search_device(cs->name, DEVICE_TYPE_CLOCKSOURCE);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	timer_cancel(&cs->keeper.timer);
	if(__clocksource == cs)
	{
		if(!(c = search_first_clocksource()))
			c = &__cs_dummy;

		spin_lock_irqsave(&__clocksource_lock, flags);
		__clocksource = c;
		spin_unlock_irqrestore(&__clocksource_lock, flags);
	}

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

ktime_t clocksource_ktime_get(struct clocksource_t * cs)
{
	if(cs)
		return clocksource_keeper_read(cs);
	return ns_to_ktime(0);
}

ktime_t ktime_get(void)
{
	return clocksource_keeper_read(__clocksource);
}
