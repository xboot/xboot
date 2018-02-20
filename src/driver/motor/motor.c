/*
 * driver/motor/motor.c
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

#include <xboot.h>
#include <motor/motor.h>

static ssize_t motor_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct motor_t * m = (struct motor_t *)kobj->priv;
	motor_enable(m);
	return size;
}

static ssize_t motor_write_disable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct motor_t * m = (struct motor_t *)kobj->priv;
	motor_disable(m);
	return size;
}

static ssize_t motor_write_speed(struct kobj_t * kobj, void * buf, size_t size)
{
	struct motor_t * m = (struct motor_t *)kobj->priv;
	motor_set_speed(m, strtol(buf, NULL, 0));
	return size;
}

struct motor_t * search_motor(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_MOTOR);
	if(!dev)
		return NULL;

	return (struct motor_t *)dev->priv;
}

bool_t register_motor(struct device_t ** device, struct motor_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(m->name);
	dev->type = DEVICE_TYPE_MOTOR;
	dev->priv = m;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "enable", NULL, motor_write_enable, m);
	kobj_add_regular(dev->kobj, "disable", NULL, motor_write_disable, m);
	kobj_add_regular(dev->kobj, "speed", NULL, motor_write_speed, m);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_motor(struct motor_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = search_device(m->name, DEVICE_TYPE_MOTOR);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void motor_enable(struct motor_t * m)
{
	if(m && m->enable)
		m->enable(m);
}

void motor_disable(struct motor_t * m)
{
	if(m && m->disable)
		m->disable(m);
}

void motor_set_speed(struct motor_t * m, int speed)
{
	if(m && m->set)
		m->set(m, speed);
}
