/*
 * kernel/core/waiter.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/waiter.h>

void waiter_init(struct waiter_t * w)
{
	if(w)
	{
		w->count = 0;
		spin_lock_init(&w->lock);
	}
}

void waiter_add(struct waiter_t * w, int v)
{
	if(w)
	{
		spin_lock(&w->lock);
		w->count += v;
		spin_unlock(&w->lock);
	}
}

void waiter_sub(struct waiter_t * w, int v)
{
	if(w)
	{
		spin_lock(&w->lock);
		w->count -= v;
		spin_unlock(&w->lock);
	}
}

void waiter_wait(struct waiter_t * w)
{
	struct task_t * self = task_self();

	if(w)
	{
		while(w->count != 0)
		{
			task_dynice_increase(self);
			task_yield();
		}
		task_dynice_restore(self);
	}
}
