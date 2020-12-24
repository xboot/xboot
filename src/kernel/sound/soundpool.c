/*
 * kernel/sound/soundpool.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <sound/soundpool.h>

struct list_head __soundpool_list = {
	.next = &__soundpool_list,
	.prev = &__soundpool_list,
};
static spinlock_t __soundpool_lock = SPIN_LOCK_INIT();

bool_t soundpool_add(struct sound_t * snd)
{
	irq_flags_t flags;

	if(!snd)
		return FALSE;

	if(!list_empty(&snd->list))
		return FALSE;

	spin_lock_irqsave(&__soundpool_lock, flags);
	list_add_tail(&snd->list, &__soundpool_list);
	spin_unlock_irqrestore(&__soundpool_lock, flags);
	return TRUE;
}

bool_t soundpool_remove(struct sound_t * snd)
{
	irq_flags_t flags;

	if(!snd)
		return FALSE;

	spin_lock_irqsave(&__soundpool_lock, flags);
	list_del(&snd->list);
	spin_unlock_irqrestore(&__soundpool_lock, flags);
	return TRUE;
}
