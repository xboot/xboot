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

static ssize_t pwm_read_duty(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	return sprintf(buf, "%d", pwm->__duty);
}

static ssize_t pwm_write_duty(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	u32_t duty = strtol(buf, NULL, 0);
	pwm_config(pwm, duty, pwm->__period);
	return size;
}

static ssize_t pwm_read_period(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	return sprintf(buf, "%d", pwm->__period);
}

static ssize_t pwm_write_period(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	u32_t period = strtol(buf, NULL, 0);
	pwm_config(pwm, pwm->__duty, period);
	return size;
}

static ssize_t pwm_read_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	return sprintf(buf, "%d", pwm->__enable ? 1 : 0);
}

static ssize_t pwm_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pwm_t * pwm = (struct pwm_t *)kobj->priv;
	int enable = strtol(buf, NULL, 0);
	if(enable != 0)
		pwm_start(pwm, pwm->__duty, pwm->__period);
	else
		pwm_stop(pwm);
	return size;
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
	struct pwm_list_t * pl;

	if(!pwm || !pwm->name)
		return FALSE;

	if(search_pwm(pwm->name))
		return FALSE;

	pl = malloc(sizeof(struct pwm_list_t));
	if(!pl)
		return FALSE;

	pwm->__duty = 0;
	pwm->__period = 0;
	pwm->__enable = FALSE;
	pwm->kobj = kobj_alloc_directory(pwm->name);
	kobj_add_regular(pwm->kobj, "duty", pwm_read_duty, pwm_write_duty, pwm);
	kobj_add_regular(pwm->kobj, "period", pwm_read_period, pwm_write_period, pwm);
	kobj_add_regular(pwm->kobj, "enable", pwm_read_enable, pwm_write_enable, pwm);
	kobj_add(search_class_pwm_kobj(), pwm->kobj);
	pl->pwm = pwm;

	spin_lock_irq(&__pwm_list_lock);
	list_add_tail(&pl->entry, &(__pwm_list.entry));
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

void pwm_start(struct pwm_t * pwm, u32_t duty, u32_t period)
{
	if(pwm && pwm->start)
	{
		pwm->start(pwm, duty, period);
		pwm->__enable = TRUE;
	}
}

void pwm_config(struct pwm_t * pwm, u32_t duty, u32_t period)
{
	if(pwm && pwm->config)
	{
		if(duty > period)
			duty = period;
		pwm->config(pwm, duty, period);
		pwm->__duty = duty;
		pwm->__period = period;
	}
}

void pwm_stop(struct pwm_t * pwm)
{
	if(pwm && pwm->stop)
	{
		pwm->stop(pwm);
		pwm->__enable = FALSE;
	}
}
