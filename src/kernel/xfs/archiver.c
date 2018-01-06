/*
 * kernel/xfs/archiver.c
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

static const char * fileext(const char * filename)
{
	const char * ret = NULL;
	const char * p;

	if(filename != NULL)
	{
		ret = p = strchr(filename, '.');
		while(p != NULL)
		{
			p = strchr(p + 1, '.');
			if(p != NULL)
				ret = p;
		}
		if(ret != NULL)
			ret++;
	}
	return (ret && (strcasecmp(ret, "x") == 0)) ? "tar" : ret;
}

void * mount_archiver(const char * path, struct xfs_archiver_t ** archiver, int * writable)
{
	const char * ext = fileext(path);
	struct xfs_archiver_t * pos, * n;
	void * m;

	if(ext)
	{
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
