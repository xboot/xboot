/*
 * kernel/core/task.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <xfs/xfs.h>
#include <xboot/task.h>

struct transfer_t
{
	void * fctx;
	void * priv;
};

extern void * make_fcontext(void * stack, size_t size, void (*func)(struct transfer_t));
extern struct transfer_t jump_fcontext(void * fctx, void * priv);

struct scheduler_t * __sched[CONFIG_MAX_CPUS] = { 0 };

static struct scheduler_t * scheduler_alloc(void)
{
	struct scheduler_t * sched;

	sched = malloc(sizeof(struct scheduler_t));
	if(!sched)
		return NULL;

	sched->root = RB_ROOT;
	init_list_head(&sched->head);
	spin_lock_init(&sched->lock);
	sched->running = NULL;
	sched->next = NULL;
	sched->min_vruntime = 0;

	return sched;
}

static void scheduler_free(struct scheduler_t * sched)
{
	struct task_t * pos, * n;

	if(!sched)
		return;

	list_for_each_entry_safe(pos, n, &sched->head, list)
	{
		task_destroy(pos);
	}
	free(sched);
}

static inline struct task_t * scheduler_next_task(struct scheduler_t * sched)
{
	return sched->next;
}

static inline void scheduler_add_task(struct scheduler_t * sched, struct task_t * task)
{
	struct rb_node ** p = &sched->root.rb_node;
	struct rb_node * parent = NULL;
	struct task_t * ptr;

	while(*p)
	{
		parent = *p;
		ptr = rb_entry(parent, struct task_t, node);
		if(task->vruntime < ptr->vruntime)
			p = &(*p)->rb_left;
		else
			p = &(*p)->rb_right;
	}
	rb_link_node(&task->node, parent, p);
	rb_insert_color(&task->node, &sched->root);

	if(!sched->next || (task->vruntime < sched->next->vruntime))
	{
		sched->next = task;
		sched->min_vruntime = sched->next->vruntime;
	}
}

static inline void scheduler_del_task(struct scheduler_t * sched, struct task_t * task)
{
	if(sched->next == task)
	{
		struct rb_node * rbn = rb_next(&task->node);
		if(rbn)
		{
			sched->next = rb_entry(rbn, struct task_t, node);
			sched->min_vruntime = sched->next->vruntime;
		}
		else
		{
			sched->next = NULL;
			sched->min_vruntime = 0;
		}
	}
	rb_erase(&task->node, &sched->root);
	RB_CLEAR_NODE(&task->node);
}

static inline void scheduler_switch_task(struct scheduler_t * sched, struct task_t * task)
{
	struct task_t * running = sched->running;
	sched->running = task;
	struct transfer_t from = jump_fcontext(task->fctx, running);
	struct task_t * t = (struct task_t *)from.priv;
	t->fctx = from.fctx;
}

static void context_entry(struct transfer_t from)
{
	struct task_t * t = (struct task_t *)from.priv;
	struct scheduler_t * sched = t->sched;
	struct task_t * next, * task = sched->running;

	t->fctx = from.fctx;
	task->func(task, task->data);
	task->status = TASK_STATUS_DEAD;
	task_destroy(task);

	next = scheduler_next_task(sched);
	if(next)
	{
		scheduler_del_task(sched, next);
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
	next = scheduler_next_task(sched);
	if(next)
	{
		sched->running = next;
		scheduler_del_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = ktime_to_ns(ktime_get());
		scheduler_switch_task(sched, next);
	}
}

struct task_t * task_create(struct scheduler_t * sched, const char * path, task_func_t func, void * data, size_t stksz, int weight)
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
	list_add_tail(&task->list, &sched->head);
	task->path = strdup(path);
	task->status = TASK_STATUS_SUSPEND;
	task->vruntime = sched->min_vruntime;
	task->start = ktime_to_ns(ktime_get());
	task->sched = sched;
	task->stack = stack + stksz;
	task->stksz = stksz;
	task->weight = weight;
	task->fctx = make_fcontext(task->stack, task->stksz, context_entry);
	task->func = func;
	task->data = data;
	task->__errno = 0;
	task->__xfs_ctx = xfs_alloc(task->path);

	return task;
}

void task_destroy(struct task_t * task)
{
	if(task && (task->status == TASK_STATUS_DEAD))
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

void task_suspend(struct task_t * task)
{
	if(task && (task->status != TASK_STATUS_SUSPEND))
	{
		task->status = TASK_STATUS_SUSPEND;
		scheduler_del_task(task->sched, task);
	}
}

void task_resume(struct task_t * task)
{
	if(task && (task->status != TASK_STATUS_READY))
	{
		task->status = TASK_STATUS_READY;
		scheduler_add_task(task->sched, task);
	}
}

void task_yield(void)
{
	struct scheduler_t * sched = scheduler_self();
	struct task_t * next, * self = task_self();
	uint64_t now = ktime_to_ns(ktime_get());

	self->vruntime += now - self->start;
	if(self->vruntime < sched->min_vruntime)
	{
		self->start = now;
	}
	else
	{
		self->status = TASK_STATUS_READY;
		scheduler_add_task(sched, self);
		next = scheduler_next_task(sched);
		scheduler_del_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = now;
		if(next != self)
			scheduler_switch_task(sched, next);
	}
}

static __init void task_pure_init(void)
{
	int i;

	for(i = 0; i < CONFIG_MAX_CPUS; i++)
		__sched[i] = scheduler_alloc();
}

static __exit void task_pure_exit(void)
{
	int i;

	for(i = 0; i < CONFIG_MAX_CPUS; i++)
		scheduler_free(__sched[i]);
}

pure_initcall(task_pure_init);
pure_exitcall(task_pure_exit);
