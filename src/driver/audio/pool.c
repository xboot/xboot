/*
 * driver/audio/pool.c
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

#include <audio/pool.h>

struct sound_list_t __sound_pool = {
	.entry = {
		.next	= &(__sound_pool.entry),
		.prev	= &(__sound_pool.entry),
	},
};
static spinlock_t __sound_pool_lock = SPIN_LOCK_INIT();

static inline int sound_pool_search(struct sound_t * snd)
{
	struct sound_list_t * pos, * n;

	if(!snd)
		return 0;

	list_for_each_entry_safe(pos, n, &(__sound_pool.entry), entry)
	{
		if(pos->snd == snd)
			return 1;
	}
	return 0;
}

void sound_pool_add(struct sound_t * snd)
{
	struct sound_list_t * sl;
	irq_flags_t flags;

	if(!snd)
		return;

	if(sound_pool_search(snd))
		return;

	sl = malloc(sizeof(struct sound_list_t));
	if(!sl)
		return;

	sl->snd = snd;
	spin_lock_irqsave(&__sound_pool_lock, flags);
	list_add_tail(&sl->entry, &(__sound_pool.entry));
	spin_unlock_irqrestore(&__sound_pool_lock, flags);
}

void sound_pool_del(struct sound_t * snd)
{
	struct sound_list_t * pos, * n;
	irq_flags_t flags;

	if(!snd)
		return;

	list_for_each_entry_safe(pos, n, &(__sound_pool.entry), entry)
	{
		if(pos->snd == snd)
		{
			spin_lock_irqsave(&__sound_pool_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__sound_pool_lock, flags);
			free(pos);
		}
	}
}

void sound_pool_clr(void)
{
	struct sound_list_t * pos, * n;
	irq_flags_t flags;

	spin_lock_irqsave(&__sound_pool_lock, flags);
	list_for_each_entry_safe(pos, n, &(__sound_pool.entry), entry)
	{
		list_del(&(pos->entry));
		free(pos);
	}
	spin_unlock_irqrestore(&__sound_pool_lock, flags);
}
