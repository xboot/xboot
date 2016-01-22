/*
 * kernel/core/machine.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <sha256.h>
#include <xboot/machine.h>

struct machine_list_t
{
	struct machine_t * mach;
	struct list_head entry;
};

static struct machine_list_t __machine_list = {
	.entry = {
		.next	= &(__machine_list.entry),
		.prev	= &(__machine_list.entry),
	},
};
static spinlock_t __machine_list_lock = SPIN_LOCK_INIT();
static struct machine_t * __machine = NULL;

static const char * __machine_uniqueid(struct machine_t * mach)
{
	const char * id = NULL;

	if(mach && mach->uniqueid)
		id = mach->uniqueid(mach);
	return id ? id : "0123456789";
}

static struct kobj_t * search_class_machine_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "machine");
}

static ssize_t machine_read_description(struct kobj_t * kobj, void * buf, size_t size)
{
	struct machine_t * mach = (struct machine_t *)kobj->priv;
	return sprintf(buf, "%s", mach->desc);
}

static ssize_t machine_read_map(struct kobj_t * kobj, void * buf, size_t size)
{
	struct machine_t * mach = (struct machine_t *)kobj->priv;
	struct mmap_t * m = (struct mmap_t *)mach->map;
	char * p = buf;
	int len = 0;

	while(m->size != 0)
	{
		len += sprintf((char *)(p + len), " %s: %p - %p\r\n", m->name, m->virt, m->phys);
		m++;
	}
	return len;
}

static ssize_t machine_read_uniqueid(struct kobj_t * kobj, void * buf, size_t size)
{
	struct machine_t * mach = (struct machine_t *)kobj->priv;
	return sprintf(buf, "%s", __machine_uniqueid(mach));
}

static struct machine_t * search_machine(const char * name)
{
	struct machine_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__machine_list.entry), entry)
	{
		if(strcmp(pos->mach->name, name) == 0)
			return pos->mach;
	}

	return NULL;
}

bool_t register_machine(struct machine_t * mach)
{
	struct machine_list_t * ml;
	irq_flags_t flags;

	if(!mach || !mach->name || !mach->detect)
		return FALSE;

	if(search_machine(mach->name))
		return FALSE;

	ml = malloc(sizeof(struct machine_list_t));
	if(!ml)
		return FALSE;

	mach->kobj = kobj_alloc_directory(mach->name);
	kobj_add_regular(mach->kobj, "description", machine_read_description, NULL, mach);
	kobj_add_regular(mach->kobj, "map", machine_read_map, NULL, mach);
	kobj_add_regular(mach->kobj, "uniqueid", machine_read_uniqueid, NULL, mach);
	kobj_add(search_class_machine_kobj(), mach->kobj);
	ml->mach = mach;

	spin_lock_irqsave(&__machine_list_lock, flags);
	list_add_tail(&ml->entry, &(__machine_list.entry));
	spin_unlock_irqrestore(&__machine_list_lock, flags);

	if((__machine == NULL) && mach->detect(mach))
	{
		if(mach->memmap)
			mach->memmap(mach);
		__machine = mach;
	}
	return TRUE;
}

bool_t unregister_machine(struct machine_t * mach)
{
	struct machine_list_t * pos, * n;
	irq_flags_t flags;

	if(!mach || !mach->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__machine_list.entry), entry)
	{
		if(pos->mach == mach)
		{
			spin_lock_irqsave(&__machine_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__machine_list_lock, flags);

			kobj_remove(search_class_machine_kobj(), pos->mach->kobj);
			kobj_remove_self(mach->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

inline __attribute__((always_inline)) struct machine_t * get_machine(void)
{
	return __machine;
}

bool_t machine_shutdown(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->shutdown)
		return mach->shutdown(mach);
	return FALSE;
}

bool_t machine_reboot(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->reboot)
		return mach->reboot(mach);
	return FALSE;
}

bool_t machine_sleep(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->sleep)
		return mach->sleep(mach);
	return FALSE;
}

bool_t machine_cleanup(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->cleanup)
		return mach->cleanup(mach);
	return FALSE;
}

const char * machine_uniqueid(void)
{
	struct machine_t * mach = get_machine();
	return __machine_uniqueid(mach);
}

int machine_keygen(const char * msg, void * key)
{
	struct machine_t * mach = get_machine();
	int len;

	if(mach && mach->keygen && ((len = mach->keygen(mach, msg, key)) > 0))
		return len;
	sha256_hash(msg, strlen(msg), key);
	return 32;
}

static virtual_addr_t __phys_to_virt(physical_addr_t phys)
{
	struct machine_t * mach = get_machine();
	struct mmap_t * m;

	if(mach)
	{
		m = (struct mmap_t *)mach->map;
		while(m->size > 0)
		{
			if((phys >= m->phys) && (phys <= m->phys + m->size - 1))
				return (virtual_addr_t)(m->virt + (phys - m->phys));
			m++;
		}
	}
	return (virtual_addr_t)phys;
}
extern __typeof(__phys_to_virt) phys_to_virt __attribute__((weak, alias("__phys_to_virt")));

static physical_addr_t __virt_to_phys(virtual_addr_t virt)
{
	struct machine_t * mach = get_machine();
	struct mmap_t * m;

	if(mach)
	{
		m = (struct mmap_t *)mach->map;
		while(m->size > 0)
		{
			if((virt >= m->virt) && (virt <= m->virt + m->size - 1))
				return (physical_addr_t)(m->phys + (virt - m->virt));
			m++;
		}
	}
	return (physical_addr_t)virt;
}
extern __typeof(__virt_to_phys) virt_to_phys __attribute__((weak, alias("__virt_to_phys")));
