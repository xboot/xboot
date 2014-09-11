/*
 * driver/pwm/pwm.c
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
#include <pwm/pwm.h>

struct pwm_list_t
{
	struct pwm_t * pwm;
	struct list_head entry;
};

struct pwm_list_t __pwm_list = {
	.entry = {
		.next	= &(__pwm_list.entry),
		.prev	= &(__pwm_list.entry),
	},
};
static spinlock_t __pwm_list_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_class_pwm_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "pwm");
}

struct pwm_t * search_pwm(const char * name)
{
	struct pwm_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__pwm_list.entry), entry)
	{
		if(strcmp(pos->pwm->name, name) == 0)
			return pos->pwm;
	}

	return NULL;
}

bool_t register_pwm(struct pwm_t * pwm)
{
	struct pwm_list_t * cl;

	if(!pwm || !pwm->name)
		return FALSE;

	if(search_pwm(pwm->name))
		return FALSE;

	cl = malloc(sizeof(struct pwm_list_t));
	if(!cl)
		return FALSE;

	pwm->kobj = kobj_alloc_directory(pwm->name);
	kobj_add(search_class_pwm_kobj(), pwm->kobj);
	cl->pwm = pwm;

	spin_lock_irq(&__pwm_list_lock);
	list_add_tail(&cl->entry, &(__pwm_list.entry));
	spin_unlock_irq(&__pwm_list_lock);

	return TRUE;
}

bool_t unregister_pwm(struct pwm_t * pwm)
{
	struct pwm_list_t * pos, * n;

	if(!pwm || !pwm->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__pwm_list.entry), entry)
	{
		if(pos->pwm == pwm)
		{
			spin_lock_irq(&__pwm_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__pwm_list_lock);

			kobj_remove(search_class_pwm_kobj(), pos->pwm->kobj);
			kobj_remove_self(pwm->kobj);
			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}
