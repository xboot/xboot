/*
 * driver/servo/servo.c
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
#include <servo/servo.h>

static ssize_t servo_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct servo_t * m = (struct servo_t *)kobj->priv;
	servo_enable(m);
	return size;
}

static ssize_t servo_write_disable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct servo_t * m = (struct servo_t *)kobj->priv;
	servo_disable(m);
	return size;
}

static ssize_t servo_write_angle(struct kobj_t * kobj, void * buf, size_t size)
{
	struct servo_t * m = (struct servo_t *)kobj->priv;
	servo_set_angle(m, strtol(buf, NULL, 0));
	return size;
}

struct servo_t * search_servo(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_SERVO);
	if(!dev)
		return NULL;
	return (struct servo_t *)dev->priv;
}

bool_t register_servo(struct device_t ** device, struct servo_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(m->name);
	dev->type = DEVICE_TYPE_SERVO;
	dev->priv = m;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "enable", NULL, servo_write_enable, m);
	kobj_add_regular(dev->kobj, "disable", NULL, servo_write_disable, m);
	kobj_add_regular(dev->kobj, "angle", NULL, servo_write_angle, m);

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

bool_t unregister_servo(struct servo_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = search_device(m->name, DEVICE_TYPE_SERVO);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void servo_enable(struct servo_t * m)
{
	if(m && m->enable)
		m->enable(m);
}

void servo_disable(struct servo_t * m)
{
	if(m && m->disable)
		m->disable(m);
}

void servo_set_angle(struct servo_t * m, int angle)
{
	if(m && m->set)
	{
		if(angle < -180)
			angle = -180;
		else if(angle > 180)
			angle = 180;
		m->set(m, angle);
	}
}
