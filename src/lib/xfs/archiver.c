/*
 * xfs/archiver.c
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
	return ret;
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
