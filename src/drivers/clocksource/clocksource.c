/*
 * driver/clocksource/clocksource.c
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

#include <xboot.h>
#include <clocksource/clocksource.h>

struct clocksource_list_t
{
	struct clocksource_t * cs;
	struct list_head entry;
};

struct clocksource_list_t __clocksource_list = {
	.entry = {
		.next	= &(__clocksource_list.entry),
		.prev	= &(__clocksource_list.entry),
	},
};
static spinlock_t __clocksource_list_lock = SPIN_LOCK_INIT();

/*
 * Dummy clocksource
 */
static volatile u64_t __cs_dummy_cycle = 0;
static u64_t __cs_dummy_read(struct clocksource_t * cs)
{
	__cs_dummy_cycle += 1;
	return __cs_dummy_cycle;
}

static struct clocksource_t __cs_dummy = {
	.name	= "dummy",
	.mult	= 1000,
	.shift	= 0,
	.mask	= CLOCKSOURCE_MASK(64),
	.last	= 0,
	.usec	= 0,
	.init	= NULL,
	.read	= __cs_dummy_read,
};
static struct clocksource_t * __current_clocksource = &__cs_dummy;

static inline u64_t __clocksource_gettime(struct clocksource_t * cs)
{
	cycle_t now, delta;
	u64_t usec, offset;

	now = cs->read(cs);
	delta = (now - cs->last) & cs->mask;
	cs->last = now;
	offset = ((u64_t)delta * cs->mult) >> cs->shift;
	usec = cs->usec + offset;
	cs->usec = usec;
	schedule_poller_task(usec);
	return usec;
}

u64_t clocksource_gettime(void)
{
	return __clocksource_gettime(__current_clocksource);
}
EXPORT_SYMBOL(clocksource_gettime);

static struct kobj_t * search_class_clocksource_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "clocksource");
}

static ssize_t classsource_read_mult(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%u", cs->mult);
}

static ssize_t classsource_read_shift(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	return sprintf(buf, "%u", cs->shift);
}

static ssize_t classsource_read_period(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t period = ((u64_t)cs->mult) >> cs->shift;
	return sprintf(buf, "%llu.%06llu", period / 1000000, period % 1000000);
}

static ssize_t classsource_read_cycle(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	cycle_t cycle;
	cycle = cs->read(cs) & cs->mask;
	return sprintf(buf, "%llu", cycle);
}

static ssize_t classsource_read_time(struct kobj_t * kobj, void * buf, size_t size)
{
	struct clocksource_t * cs = (struct clocksource_t *)kobj->priv;
	u64_t time = __clocksource_gettime(cs);
	return sprintf(buf, "%llu.%06llu", time / 1000000, time % 1000000);
}

static struct clocksource_t * search_clocksource(const char * name)
{
	struct clocksource_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		if(strcmp(pos->cs->name, name) == 0)
			return pos->cs;
	}

	return NULL;
}

u32_t clocksource_hz2mult(u32_t hz, u32_t shift)
{
	u64_t tmp = ((u64_t)1000000) << shift;

	tmp += hz / 2;
	tmp = tmp / hz;
	return (u32_t)tmp;
}

void clocksource_calc_mult_shift(u32_t * mult, u32_t * shift, u32_t from, u32_t to, u32_t maxsec)
{
	u64_t tmp;
	u32_t sft, sftacc = 32;

	tmp = ((u64_t)maxsec * from) >> 32;
	while(tmp)
	{
		tmp >>=1;
		sftacc--;
	}

	for(sft = 32; sft > 0; sft--)
	{
		tmp = (u64_t) to << sft;
		tmp += from / 2;
		tmp = tmp / from;
		if ((tmp >> sftacc) == 0)
			break;
	}
	*mult = tmp;
	*shift = sft;
}

bool_t register_clocksource(struct clocksource_t * cs)
{
	struct clocksource_list_t * cl;

	if(!cs || !cs->name)
		return FALSE;

	if(search_clocksource(cs->name))
		return FALSE;

	cl = malloc(sizeof(struct clocksource_list_t));
	if(!cl)
		return FALSE;

	cs->last = 0;
	cs->usec = 0;
	cs->kobj = kobj_alloc_directory(cs->name);
	kobj_add_regular(cs->kobj, "mult", classsource_read_mult, NULL, cs);
	kobj_add_regular(cs->kobj, "shift", classsource_read_shift, NULL, cs);
	kobj_add_regular(cs->kobj, "period", classsource_read_period, NULL, cs);
	kobj_add_regular(cs->kobj, "cycle", classsource_read_cycle, NULL, cs);
	kobj_add_regular(cs->kobj, "time", classsource_read_time, NULL, cs);
	kobj_add(search_class_clocksource_kobj(), cs->kobj);
	cl->cs = cs;

	spin_lock_irq(&__clocksource_list_lock);
	list_add_tail(&cl->entry, &(__clocksource_list.entry));
	spin_unlock_irq(&__clocksource_list_lock);

	return TRUE;
}

bool_t unregister_clocksource(struct clocksource_t * cs)
{
	struct clocksource_list_t * pos, * n;

	if(!cs || !cs->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		if(pos->cs == cs)
		{
			spin_lock_irq(&__clocksource_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__clocksource_list_lock);

			kobj_remove(search_class_clocksource_kobj(), pos->cs->kobj);
			kobj_remove_self(cs->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

void subsys_init_clocksource(void)
{
	struct clocksource_list_t * pos, * n;
	struct clocksource_t * cs;
	u64_t period = ~0;
	u64_t t;

	list_for_each_entry_safe(pos, n, &(__clocksource_list.entry), entry)
	{
		cs = pos->cs;
		if(!cs)
			continue;

		if(cs->init)
			cs->init(cs);

		t = ((u64_t)cs->mult) >> cs->shift;
		if(t < period)
		{
			__current_clocksource = cs;
			period = t;
		}
	}

	LOG("Attach system clocksource [%s]", __current_clocksource->name);
}
