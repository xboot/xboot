/*
 * kernel/xfs/archiver.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <xfs/archiver.h>

static struct list_head __archiver_list = {
	.next = &__archiver_list,
	.prev = &__archiver_list,
};
static spinlock_t __archiver_lock = SPIN_LOCK_INIT();

bool_t register_archiver(struct xfs_archiver_t * archiver)
{
	irq_flags_t flags;

	if(!archiver || !archiver->name)
		return FALSE;

	spin_lock_irqsave(&__archiver_lock, flags);
	init_list_head(&archiver->list);
	list_add_tail(&archiver->list, &__archiver_list);
	spin_unlock_irqrestore(&__archiver_lock, flags);

	return TRUE;
}

bool_t unregister_archiver(struct xfs_archiver_t * archiver)
{
	irq_flags_t flags;

	if(!archiver || !archiver->name)
		return FALSE;

	spin_lock_irqsave(&__archiver_lock, flags);
	list_del(&archiver->list);
	spin_unlock_irqrestore(&__archiver_lock, flags);

	return TRUE;
}

void * mount_archiver(const char * path, struct xfs_archiver_t ** archiver, int * writable)
{
	const char * ext = fileext(path);
	struct xfs_archiver_t * pos, * n;
	void * m;

	if(ext)
	{
		if(strcasecmp(ext, "x") == 0)
			ext = "tar";
		list_for_each_entry_safe(pos, n, &__archiver_list, list)
		{
			if(strcasecmp(pos->name, ext) == 0)
			{
				m = pos->mount(path, writable);
				if(m)
				{
					if(archiver)
						*archiver = pos;
					return m;
				}
			}
		}
	}

	list_for_each_entry_safe(pos, n, &__archiver_list, list)
	{
		m = pos->mount(path, writable);
		if(m)
		{
			if(archiver)
				*archiver = pos;
			return m;
		}
	}

	return NULL;
}
