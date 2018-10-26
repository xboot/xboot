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
#include <context.h>
#include <xboot/task.h>

static inline struct task_t * next_ready_task(struct scheduler_t * s)
{
	if(list_empty(&s->ready))
		return NULL;
	if(!s->running || (s->running->status != TASK_STATUS_READY) || list_is_last(&s->running->list, &s->ready))
		return list_first_entry(&s->ready, struct task_t, list);
	return list_next_entry(s->running, list);
}

static inline void scheduler_switch_task(struct scheduler_t * s, struct task_t * task)
{
	struct task_t * running = s->running;
	s->running = task;
	struct transfer_t from = jump_context(task->ctx, running);
	struct task_t * t = (struct task_t *)from.priv;
	t->ctx = from.ctx;
}

static void context_entry(struct transfer_t from)
{
	struct task_t * t = (struct task_t *)from.priv;
	struct task_t * task = t->s->running;
	struct task_t * next;
	irq_flags_t flags;

	t->ctx = from.ctx;
	task->func(task->s, task->data);
	next = next_ready_task(task->s);
	spin_lock_irqsave(&task->s->lock, flags);
	list_del(&task->list);
	list_add_tail(&task->list, &task->s->dead);
	task->status = TASK_STATUS_DEAD;
	spin_unlock_irqrestore(&task->s->lock, flags);
	if(next && (next != t->s->running))
		scheduler_switch_task(t->s, next);
}

struct scheduler_t * scheduler_alloc(void)
{
	struct scheduler_t * s;

	s = malloc(sizeof(struct scheduler_t));
	if(!s)
		return NULL;

	init_list_head(&s->dead);
	init_list_head(&s->ready);
	init_list_head(&s->suspend);
	spin_lock_init(&s->lock);
	s->running = NULL;
	return s;
}

void scheduler_free(struct scheduler_t * s)
{
	struct task_t * pos, * n;

	if(!s)
		return;

	list_for_each_entry_safe(pos, n, &s->dead, list)
	{
		task_destory(s, pos);
	}
	list_for_each_entry_safe(pos, n, &s->ready, list)
	{
		task_destory(s, pos);
	}
	list_for_each_entry_safe(pos, n, &s->suspend, list)
	{
		task_destory(s, pos);
	}
	free(s);
}

void scheduler_loop(struct scheduler_t * s)
{
	if(s && !list_empty(&s->ready))
	{
		s->running = list_first_entry(&s->ready, struct task_t, list);
		scheduler_switch_task(s, s->running);
	}
}

struct task_t * task_create(struct scheduler_t * s, const char * name, task_func_t func, void * data, size_t size)
{
	struct task_t * task;
	irq_flags_t flags;
	void * stack;

	if(!s || !func)
		return NULL;

	if(size <= 0)
		size = 8192;

	task = malloc(sizeof(struct task_t));
	if(!task)
		return NULL;

	stack = malloc(size);
	if(!stack)
	{
		free(task);
		return NULL;
	}

	spin_lock_irqsave(&s->lock, flags);
	init_list_head(&task->list);
	list_add_tail(&task->list, &s->ready);
	spin_unlock_irqrestore(&s->lock, flags);

	task->name = strdup(name ? name : "unknown");
	task->status = TASK_STATUS_READY;
	task->s = s;
	task->stack = stack + size;
	task->size = size;
	task->ctx = make_context(task->stack, task->size, context_entry);
	task->func = func;
	task->data = data;

	return task;
}

void task_destory(struct scheduler_t * s, struct task_t * task)
{
	irq_flags_t flags;

	if(s && task)
	{
		spin_lock_irqsave(&s->lock, flags);
		list_del(&task->list);
		task->status = TASK_STATUS_DEAD;
		spin_unlock_irqrestore(&s->lock, flags);
		free(task->name);
		free(task->stack);
		free(task);
	}
}

void task_suspend(struct scheduler_t * s, struct task_t * task)
{
	irq_flags_t flags;

	if(s && task)
	{
		spin_lock_irqsave(&s->lock, flags);
		list_del(&task->list);
		list_add_tail(&task->list, &task->s->suspend);
		task->status = TASK_STATUS_SUSPEND;
		spin_unlock_irqrestore(&s->lock, flags);
	}
}

void task_resume(struct scheduler_t * s, struct task_t * task)
{
	irq_flags_t flags;

	if(s && task)
	{
		spin_lock_irqsave(&s->lock, flags);
		list_del(&task->list);
		list_add_tail(&task->list, &task->s->ready);
		task->status = TASK_STATUS_READY;
		spin_unlock_irqrestore(&s->lock, flags);
	}
}

void task_yield(struct scheduler_t * s)
{
	struct task_t * next = next_ready_task(s);

	if(next && (next != s->running))
		scheduler_switch_task(s, next);
}
