/*
 * kernel/core/semaphore.c
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
#include <xboot/semaphore.h>

void semaphore_init(struct semaphore_t * sem, int v)
{
	atomic_set(&sem->value, v);
	init_list_head(&sem->waiting);
}

void semaphore_post(struct semaphore_t * sem)
{
	struct task_t * pos, * n;
	int v = atomic_inc_return(&sem->value);

	list_for_each_entry_safe(pos, n, &sem->waiting, sem_list)
	{
		if(v-- > 0)
		{
			list_del_init(&pos->sem_list);
			task_resume(pos);
			task_yield();
		}
	}
	task_yield();
}

void semaphore_wait(struct semaphore_t * sem)
{
	struct task_t * task = task_self();

	if(atomic_get(&sem->value) > 0)
	{
		atomic_dec(&sem->value);
	}
	else
	{
		if(list_empty_careful(&task->sem_list))
			list_add_tail(&task_self()->sem_list, &sem->waiting);
		task_suspend(task_self());
		if(atomic_get(&sem->value) > 0)
			atomic_dec(&sem->value);
	}
}
