/*
 * drivers/audio/pool.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
