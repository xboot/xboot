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

static inline struct task_t * next_ready_task(struct scheduler_t * sched)
{
	if(list_empty(&sched->ready))
		return NULL;
	if(!sched->running || (sched->running->status != TASK_STATUS_READY) || list_is_last(&sched->running->list, &sched->ready))
		return list_first_entry(&sched->ready, struct task_t, list);
	return list_next_entry(sched->running, list);
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
	struct task_t * task = t->sched->running;
	struct task_t * next;
	irq_flags_t flags;

	t->fctx = from.fctx;
	task->func(task, task->data);
	next = next_ready_task(task->sched);
	spin_lock_irqsave(&task->sched->lock, flags);
	list_del(&task->list);
	list_add_tail(&task->list, &task->sched->dead);
	task->status = TASK_STATUS_DEAD;
	spin_unlock_irqrestore(&task->sched->lock, flags);
	if(next && (next != t->sched->running))
		scheduler_switch_task(t->sched, next);
}

static struct scheduler_t * scheduler_alloc(void)
{
	struct scheduler_t * sched;

	sched = malloc(sizeof(struct scheduler_t));
	if(!sched)
		return NULL;

	init_list_head(&sched->dead);
	init_list_head(&sched->ready);
	init_list_head(&sched->suspend);
	spin_lock_init(&sched->lock);
	sched->running = NULL;
	return sched;
}

static void scheduler_free(struct scheduler_t * sched)
{
	struct task_t * pos, * n;

	if(!sched)
		return;

	list_for_each_entry_safe(pos, n, &sched->dead, list)
	{
		task_destory(pos);
	}
	list_for_each_entry_safe(pos, n, &sched->ready, list)
	{
		task_destory(pos);
	}
	list_for_each_entry_safe(pos, n, &sched->suspend, list)
	{
		task_destory(pos);
	}
	free(sched);
}

static void scheduler_start(struct scheduler_t * sched)
{
	if(sched && !list_empty(&sched->ready))
	{
		sched->running = list_first_entry(&sched->ready, struct task_t, list);
		scheduler_switch_task(sched, sched->running);
	}
}

void scheduler_loop(void)
{
	scheduler_start(__sched[smp_processor_id()]);
}

struct task_t * task_create(struct scheduler_t * sched, const char * path, task_func_t func, void * data, size_t stksz, int weight)
{
	struct task_t * task;
	irq_flags_t flags;
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

	spin_lock_irqsave(&sched->lock, flags);
	init_list_head(&task->list);
	list_add_tail(&task->list, &sched->ready);
	spin_unlock_irqrestore(&sched->lock, flags);

	task->path = strdup(path);
	task->status = TASK_STATUS_READY;
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

void task_destory(struct task_t * task)
{
	irq_flags_t flags;

	if(task)
	{
		spin_lock_irqsave(&task->sched->lock, flags);
		list_del(&task->list);
		task->status = TASK_STATUS_DEAD;
		spin_unlock_irqrestore(&task->sched->lock, flags);
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
	irq_flags_t flags;

	if(task)
	{
		spin_lock_irqsave(&task->sched->lock, flags);
		list_del(&task->list);
		list_add_tail(&task->list, &task->sched->suspend);
		task->status = TASK_STATUS_SUSPEND;
		spin_unlock_irqrestore(&task->sched->lock, flags);
	}
}

void task_resume(struct task_t * task)
{
	irq_flags_t flags;

	if(task)
	{
		spin_lock_irqsave(&task->sched->lock, flags);
		list_del(&task->list);
		list_add_tail(&task->list, &task->sched->ready);
		task->status = TASK_STATUS_READY;
		spin_unlock_irqrestore(&task->sched->lock, flags);
	}
}

void task_wait(struct task_t * task)
{
	if(task)
	{
		while(task->status != TASK_STATUS_DEAD)
			task_yield();
	}
}

void task_yield(void)
{
	struct task_t * self = task_self();
	struct task_t * next = next_ready_task(self->sched);

	if(next && (next != self))
		scheduler_switch_task(self->sched, next);
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
