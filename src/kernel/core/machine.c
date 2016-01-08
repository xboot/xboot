/*
 * kernel/core/machine.c
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
		id = mach->uniqueid();
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

static ssize_t machine_read_banks(struct kobj_t * kobj, void * buf, size_t size)
{
	struct machine_t * mach = (struct machine_t *)kobj->priv;
	char * p = buf;
	int i, len = 0;
	u64_t from, to;

	for(i = 0; i < ARRAY_SIZE(mach->banks); i++)
	{
		if( (mach->banks[i].start == 0) && (mach->banks[i].size == 0) )
			break;
		from = mach->banks[i].start;
		to = mach->banks[i].start + mach->banks[i].size - 1;
		len += sprintf((char *)(p + len), " bank%d: 0x%016Lx - 0x%016Lx\r\n", i, from, to);
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

	if(!mach || !mach->name)
		return FALSE;

	if(search_machine(mach->name))
		return FALSE;

	ml = malloc(sizeof(struct machine_list_t));
	if(!ml)
		return FALSE;

	mach->kobj = kobj_alloc_directory(mach->name);
	kobj_add_regular(mach->kobj, "description", machine_read_description, NULL, mach);
	kobj_add_regular(mach->kobj, "banks", machine_read_banks, NULL, mach);
	kobj_add_regular(mach->kobj, "uniqueid", machine_read_uniqueid, NULL, mach);
	kobj_add(search_class_machine_kobj(), mach->kobj);
	ml->mach = mach;

	spin_lock_irqsave(&__machine_list_lock, flags);
	list_add_tail(&ml->entry, &(__machine_list.entry));
	spin_unlock_irqrestore(&__machine_list_lock, flags);

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

struct machine_t * get_machine(void)
{
	return __machine;
}

bool_t machine_poweroff(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->poweroff)
		return mach->poweroff();
	return FALSE;
}

bool_t machine_reboot(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->reboot)
		return mach->reboot();
	return FALSE;
}

bool_t machine_sleep(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->sleep)
		return mach->sleep();
	return FALSE;
}

bool_t machine_cleanup(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->cleanup)
		return mach->cleanup();
	return FALSE;
}

bool_t machine_keygen(const void * msg, int len, void * key)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->keygen && mach->keygen(msg, len, key))
		return TRUE;
	sha256_hash(msg, len, key);
	return TRUE;
}

const char * machine_uniqueid(void)
{
	struct machine_t * mach = get_machine();
	return __machine_uniqueid(mach);
}

void subsys_init_machine(void)
{
	struct machine_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__machine_list.entry), entry)
	{
		if(pos->mach->detect && pos->mach->detect())
		{
			__machine = pos->mach;

			if(pos->mach->poweron)
				pos->mach->poweron();

			extern void mmu_setup(struct machine_t * mach);
			mmu_setup(pos->mach);

			LOG("Found machine [%s]", get_machine()->name);
			return;
		}
	}

	LOG("Not found any machine");
}
