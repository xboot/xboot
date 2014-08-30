/*
 * kernel/core/machine.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <spinlock.h>
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

extern void mmu_setup(struct machine_t * mach);
bool_t init_system_machine(void)
{
	struct machine_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__machine_list.entry), entry)
	{
		if(pos->mach->detect && pos->mach->detect())
		{
			__machine = pos->mach;

			if(pos->mach->powerup)
				pos->mach->powerup();
			if(pos->mach->getmode)
				xboot_set_mode(pos->mach->getmode());

			mmu_setup(pos->mach);
			return TRUE;
		}
	}

	return FALSE;
}

struct machine_t * get_machine(void)
{
	return __machine;
}

bool_t register_machine(struct machine_t * mach)
{
	struct machine_list_t * ml;

	if(!mach || !mach->name)
		return FALSE;

	if(search_machine(mach->name))
		return FALSE;

	ml = malloc(sizeof(struct machine_list_t));
	if(!ml)
		return FALSE;

	ml->mach = mach;

	spin_lock_irq(&__machine_list_lock);
	list_add_tail(&ml->entry, &(__machine_list.entry));
	spin_unlock_irq(&__machine_list_lock);

	return TRUE;
}

bool_t unregister_machine(struct machine_t * mach)
{
	struct machine_list_t * pos, * n;

	if(!mach || !mach->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__machine_list.entry), entry)
	{
		if(pos->mach == mach)
		{
			spin_lock_irq(&__machine_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__machine_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t machine_shutdown(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->shutdown)
		return mach->shutdown();
	return FALSE;
}

bool_t machine_reset(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->reset)
		return mach->reset();
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

bool_t machine_authentication(void)
{
	struct machine_t * mach = get_machine();

	if(mach && mach->authentication)
		return mach->authentication();
	return FALSE;
}
