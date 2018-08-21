/*
 * kernel/core/profiler.c
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

#include <xboot.h>
#include <xboot/profiler.h>

static struct hlist_head __profiler_hash[CONFIG_PROFILER_HASH_SIZE];
static spinlock_t __profiler_lock = SPIN_LOCK_INIT();

static void __cpu_profiler_start(int event, int data)
{
}
extern __typeof(__cpu_profiler_start) cpu_profiler_start __attribute__((weak, alias("__cpu_profiler_start")));

static void __cpu_profiler_stop(int event, int data)
{
}
extern __typeof(__cpu_profiler_stop) cpu_profiler_stop __attribute__((weak, alias("__cpu_profiler_stop")));

static uint64_t __cpu_profiler_read(int event, int data)
{
	return 0;
}
extern __typeof(__cpu_profiler_read) cpu_profiler_read __attribute__((weak, alias("__cpu_profiler_read")));

static void __cpu_profiler_reset(void)
{
}
extern __typeof(__cpu_profiler_reset) cpu_profiler_reset __attribute__((weak, alias("__cpu_profiler_reset")));

static inline uint32_t string_hash(const char * s)
{
	uint32_t nr = 1, nr2 = 4;
	int len = strlen(s);

	while(len--)
	{
		nr ^= (((nr & 63) + nr2) * ((uint32_t)(uint8_t)*s++)) + (nr << 8);
		nr2 += 3;
	}
	return nr;
}

struct profiler_t * profiler_search(const char * name)
{
	struct profiler_t * p;
	struct hlist_node * n;
	uint32_t index = string_hash(name) % CONFIG_PROFILER_HASH_SIZE;

	hlist_for_each_entry_safe(p, n, &__profiler_hash[index], node)
	{
		if(strcmp(p->name, name) == 0)
			return p;
	}
	return NULL;
}

void profiler_snap(const char * name, int event, int data)
{
	struct profiler_t * p;
	irq_flags_t flags;
	uint32_t index;

	p = profiler_search(name);
	if(p)
	{
		if(event == 0)
		{
			p->end = ktime_to_ns(ktime_get());
		}
		else
		{
			p->end = cpu_profiler_read(p->event, p->data);
		}
		p->count++;
	}
	else
	{
		p = malloc(sizeof(struct profiler_t));
		if(!p)
			return;

		index = string_hash(name) % CONFIG_PROFILER_HASH_SIZE;
		init_hlist_node(&p->node);
		p->name = strdup(name);
		p->event = event;
		p->data = data;
		if(event == 0)
		{
			p->end = p->begin = ktime_to_ns(ktime_get());
		}
		else
		{
			cpu_profiler_start(p->event, p->data);
			p->end = p->begin = cpu_profiler_read(p->event, p->data);
		}
		p->count = 1;
		spin_lock_irqsave(&__profiler_lock, flags);
		hlist_add_head(&p->node, &__profiler_hash[index]);
		spin_unlock_irqrestore(&__profiler_lock, flags);
	}
}

void profiler_dump(void)
{
	struct profiler_t * p;
	struct hlist_node * n;
	int i;

	printf("Profiler analysis:\r\n");
	for(i = 0; i < ARRAY_SIZE(__profiler_hash); i++)
	{
		hlist_for_each_entry_safe(p, n, &__profiler_hash[i], node)
		{
			if(p->event == 0)
			{
				printf("[%s] %lld, %lld, [%lld ~ %lld]\r\n", p->name, p->count, (p->end - p->begin) / ((p->count > 1) ? (p->count - 1) : 1), p->begin, p->end);
			}
			else
			{
				printf("[%s] %lld, %lld, [%lld ~ %lld]\r\n", p->name, p->count, (p->end - p->begin) / ((p->count > 1) ? (p->count - 1) : 1), p->begin, p->end);
			}
		}
	}
}

void profiler_reset(void)
{
	struct profiler_t * p;
	struct hlist_node * n;
	irq_flags_t flags;
	int i;

	for(i = 0; i < ARRAY_SIZE(__profiler_hash); i++)
	{
		hlist_for_each_entry_safe(p, n, &__profiler_hash[i], node)
		{
			spin_lock_irqsave(&__profiler_lock, flags);
			hlist_del(&p->node);
			if(p->event != 0)
				cpu_profiler_stop(p->event, p->data);
			free(p->name);
			free(p);
			spin_unlock_irqrestore(&__profiler_lock, flags);
		}
	}
	cpu_profiler_reset();
}

static __init void profiler_pure_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(__profiler_hash); i++)
		init_hlist_head(&__profiler_hash[i]);
}
pure_initcall(profiler_pure_init);
