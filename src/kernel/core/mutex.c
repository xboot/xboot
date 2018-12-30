/*
 * kernel/core/mutex.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/mutex.h>

void mutex_init(struct mutex_t * m)
{
	atomic_set(&m->atomic, 1);
	init_list_head(&m->mwait);
	spin_lock_init(&m->lock);
}

void mutex_lock(struct mutex_t * m)
{
	struct task_t * self;

	while(atomic_cmpxchg(&m->atomic, 1, 0) != 1)
	{
		self = task_self();
		spin_lock(&m->lock);
		if(list_empty_careful(&self->mlist))
			list_add_tail(&self->mlist, &m->mwait);
		spin_unlock(&m->lock);
		task_suspend(self);
	}
}

void mutex_unlock(struct mutex_t * m)
{
	struct task_t * pos, * n;

	if(atomic_cmpxchg(&m->atomic, 0, 1) == 0)
	{
		spin_lock(&m->lock);
		if(!list_empty(&m->mwait))
		{
			list_for_each_entry_safe(pos, n, &m->mwait, mlist)
			{
				list_del_init(&pos->mlist);
				task_resume(pos);
				break;
			}
		}
		spin_unlock(&m->lock);
	}
}
