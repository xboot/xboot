/*
 * kernel/core/task.c
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
#include <xfs/xfs.h>
#include <xboot/task.h>

struct transfer_t
{
	void * fctx;
	void * priv;
};

extern void * make_fcontext(void * stack, size_t size, void (*func)(struct transfer_t));
extern struct transfer_t jump_fcontext(void * fctx, void * priv);

struct scheduler_t * __sched[CONFIG_MAX_SMP_CPUS] = { 0 };

static const int nice_to_weight[40] = {
 /* -20 */     88761,     71755,     56483,     46273,     36291,
 /* -15 */     29154,     23254,     18705,     14949,     11916,
 /* -10 */      9548,      7620,      6100,      4904,      3906,
 /*  -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /*  10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};

static const uint32_t nice_to_wmult[40] = {
 /* -20 */     48388,     59856,     76040,     92818,    118348,
 /* -15 */    147320,    184698,    229616,    287308,    360437,
 /* -10 */    449829,    563644,    704093,    875809,   1099582,
 /*  -5 */   1376151,   1717300,   2157191,   2708050,   3363326,
 /*   0 */   4194304,   5237765,   6557202,   8165337,  10153587,
 /*   5 */  12820798,  15790321,  19976592,  24970740,  31350126,
 /*  10 */  39045157,  49367440,  61356676,  76695844,  95443717,
 /*  15 */ 119304647, 148102320, 186737708, 238609294, 286331153,
};

static inline uint64_t mul_u32_u32(uint32_t a, uint32_t b)
{
	return (uint64_t)a * b;
}

static inline uint64_t mul_u64_u32_shr(uint64_t a, uint32_t mul, unsigned int shift)
{
	uint32_t ah, al;
	uint64_t ret;

	al = a;
	ah = a >> 32;

	ret = mul_u32_u32(al, mul) >> shift;
	if(ah)
		ret += mul_u32_u32(ah, mul) << (32 - shift);
	return ret;
}

static inline uint64_t calc_delta(struct task_t * task, uint64_t delta)
{
	uint64_t fact = 1024;
	int shift = 32;

	if(unlikely(fact >> 32))
	{
		while(fact >> 32)
		{
			fact >>= 1;
			shift--;
		}
	}
	fact = (uint64_t)(uint32_t)fact * task->inv_weight;

	while(fact >> 32)
	{
		fact >>= 1;
		shift--;
	}
	return mul_u64_u32_shr(delta, fact, shift);
}

static inline uint64_t calc_delta_fair(struct task_t * task, uint64_t delta)
{
	if(unlikely(task->weight != 1024))
		delta = calc_delta(task, delta);
	return delta;
}

static struct scheduler_t * scheduler_alloc(void)
{
	struct scheduler_t * sched;

	sched = malloc(sizeof(struct scheduler_t));
	if(!sched)
		return NULL;

	sched->ready = RB_ROOT_CACHED;
	init_list_head(&sched->suspend);
	spin_lock_init(&sched->lock);
	sched->running = NULL;
	sched->min_vtime = 0;

	return sched;
}

static void scheduler_free(struct scheduler_t * sched)
{
	struct task_t * pos, * n;

	if(!sched)
		return;

	list_for_each_entry_safe(pos, n, &sched->suspend, list)
	{
		task_destroy(pos);
	}
	rbtree_postorder_for_each_entry_safe(pos, n, &sched->ready.rb_root, node)
	{
		task_destroy(pos);
	}
	task_destroy(sched->running);

	free(sched);
}

static inline struct task_t * scheduler_next_ready_task(struct scheduler_t * sched)
{
	struct rb_node * leftmost = rb_first_cached(&sched->ready);

	if(!leftmost)
		return NULL;
	return rb_entry(leftmost, struct task_t, node);
}

static inline void scheduler_enqueue_task(struct scheduler_t * sched, struct task_t * task)
{
	struct rb_node ** link = &sched->ready.rb_root.rb_node;
	struct rb_node * parent = NULL;
	struct task_t * next, * entry;
	int leftmost = 1;

	while(*link)
	{
		parent = *link;
		entry = rb_entry(parent, struct task_t, node);
		if((int64_t)(task->vtime - entry->vtime) < 0)
		{
			link = &parent->rb_left;
		}
		else
		{
			link = &parent->rb_right;
			leftmost = 0;
		}
	}

	rb_link_node(&task->node, parent, link);
	rb_insert_color_cached(&task->node, &sched->ready, leftmost);
	next = scheduler_next_ready_task(sched);
	if(likely(next))
		sched->min_vtime = next->vtime;
	else if(sched->running)
		sched->min_vtime = sched->running->vtime;
	else
		sched->min_vtime = 0;
}

static inline void scheduler_dequeue_task(struct scheduler_t * sched, struct task_t * task)
{
	struct task_t * next;

	rb_erase_cached(&task->node, &sched->ready);
	next = scheduler_next_ready_task(sched);
	if(likely(next))
		sched->min_vtime = next->vtime;
	else if(sched->running)
		sched->min_vtime = sched->running->vtime;
	else
		sched->min_vtime = 0;
}

static inline void scheduler_switch_task(struct scheduler_t * sched, struct task_t * task)
{
	struct task_t * running = sched->running;
	sched->running = task;
	struct transfer_t from = jump_fcontext(task->fctx, running);
	struct task_t * t = (struct task_t *)from.priv;
	t->fctx = from.fctx;
}

static void fcontext_entry_func(struct transfer_t from)
{
	struct task_t * t = (struct task_t *)from.priv;
	struct scheduler_t * sched = t->sched;
	struct task_t * next, * task = sched->running;

	t->fctx = from.fctx;
	task->func(task, task->data);
	task_destroy(task);

	next = scheduler_next_ready_task(sched);
	if(likely(next))
	{
		scheduler_dequeue_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = ktime_to_ns(ktime_get());
		scheduler_switch_task(sched, next);
	}
}

void scheduler_loop(void)
{
	struct scheduler_t * sched;
	struct task_t * next;

	sched = __sched[smp_processor_id()];
	next = scheduler_next_ready_task(sched);
	if(next)
	{
		sched->running = next;
		scheduler_dequeue_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = ktime_to_ns(ktime_get());
		scheduler_switch_task(sched, next);
	}
}

struct task_t * task_create(struct scheduler_t * sched, const char * path, task_func_t func, void * data, size_t stksz, int nice)
{
	struct task_t * task;
	void * stack;

	if(!sched || !func)
		return NULL;

	if(stksz <= 0)
		stksz = CONFIG_TASK_STACK_SIZE;

	task = malloc(sizeof(struct task_t));
	if(!task)
		return NULL;

	stack = malloc(stksz);
	if(!stack)
	{
		free(task);
		return NULL;
	}

	RB_CLEAR_NODE(&task->node);
	init_list_head(&task->list);
	init_list_head(&task->slist);
	init_list_head(&task->rlist);
	init_list_head(&task->mlist);
	list_add_tail(&task->list, &sched->suspend);
	task->path = strdup(path);
	task->status = TASK_STATUS_SUSPEND;
	task->start = ktime_to_ns(ktime_get());
	task->time = 0;
	task->vtime = 0;
	task->sched = sched;
	task->stack = stack;
	task->stksz = stksz;
	task->fctx = make_fcontext(task->stack + stksz, task->stksz, fcontext_entry_func);
	task->func = func;
	task->data = data;
	task->__errno = 0;
	task->__xfs_ctx = xfs_alloc(task->path);
	task_renice(task, nice);

	return task;
}

void task_destroy(struct task_t * task)
{
	if(task)
	{
		list_del(&task->list);
		if(task->__xfs_ctx)
			xfs_free(task->__xfs_ctx);
		if(task->path)
			free(task->path);
		free(task->stack);
		free(task);
	}
}

void task_renice(struct task_t * task, int nice)
{
	if(nice < -20)
		nice = -20;
	else if(nice > 19)
		nice = 19;
	task->nice = nice;
	nice += 20;
	task->weight = nice_to_weight[nice];
	task->inv_weight = nice_to_wmult[nice];
}

void task_suspend(struct task_t * task)
{
	struct task_t * next;
	uint64_t now, detla;

	if(task)
	{
		if(task->status == TASK_STATUS_READY)
		{
			task->status = TASK_STATUS_SUSPEND;
			list_add_tail(&task->list, &task->sched->suspend);
			scheduler_dequeue_task(task->sched, task);
		}
		else if(task->status == TASK_STATUS_RUNNING)
		{
			now = ktime_to_ns(ktime_get());
			detla = now - task->start;

			task->time += detla;
			task->vtime += calc_delta_fair(task, detla);
			task->status = TASK_STATUS_SUSPEND;
			list_add_tail(&task->list, &task->sched->suspend);

			next = scheduler_next_ready_task(task->sched);
			if(next)
			{
				scheduler_dequeue_task(task->sched, next);
				next->status = TASK_STATUS_RUNNING;
				next->start = now;
				scheduler_switch_task(task->sched, next);
			}
		}
	}
}

void task_resume(struct task_t * task)
{
	if(task && (task->status == TASK_STATUS_SUSPEND))
	{
		task->vtime = task->sched->min_vtime;
		task->status = TASK_STATUS_READY;
		list_del(&task->list);
		scheduler_enqueue_task(task->sched, task);
	}
}

void task_yield(void)
{
	struct scheduler_t * sched = scheduler_self();
	struct task_t * next, * self = task_self();
	uint64_t now = ktime_to_ns(ktime_get());
	uint64_t detla = now - self->start;

	self->time += detla;
	self->vtime += calc_delta_fair(self, detla);

	if((int64_t)(self->vtime - sched->min_vtime) < 0)
	{
		self->start = now;
	}
	else
	{
		self->status = TASK_STATUS_READY;
		scheduler_enqueue_task(sched, self);
		next = scheduler_next_ready_task(sched);
		scheduler_dequeue_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = now;
		if(likely(next != self))
			scheduler_switch_task(sched, next);
	}
}

static void idle_task(struct task_t * task, void * data)
{
	while(1)
	{
		task_yield();
	}
}

static __init void task_pure_init(void)
{
	struct task_t * task;
	int i;

	for(i = 0; i < CONFIG_MAX_SMP_CPUS; i++)
	{
		__sched[i] = scheduler_alloc();

		task = task_create(__sched[i], "idle", idle_task, NULL, SZ_8K, 0);
		task->nice = 26;
		task->weight = 3;
		task->inv_weight = 1431655765;
		task_resume(task);
	}
}

static __exit void task_pure_exit(void)
{
	int i;

	for(i = 0; i < CONFIG_MAX_SMP_CPUS; i++)
		scheduler_free(__sched[i]);
}

pure_initcall(task_pure_init);
pure_exitcall(task_pure_exit);
