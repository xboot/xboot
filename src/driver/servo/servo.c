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

static ssize_t servo_read_angle(struct kobj_t * kobj, void * buf, size_t size)
{
	struct servo_t * servo = (struct servo_t *)kobj->priv;
	return sprintf(buf, "%d", servo_get_angle(servo));
}

static ssize_t servo_write_angle(struct kobj_t * kobj, void * buf, size_t size)
{
	struct servo_t * servo = (struct servo_t *)kobj->priv;
	servo_set_angle(servo, strtol(buf, NULL, 0));
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

bool_t register_servo(struct device_t ** device, struct servo_t * servo)
{
	struct device_t * dev;

	if(!servo || !servo->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(servo->name);
	dev->type = DEVICE_TYPE_SERVO;
	dev->priv = servo;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "angle", servo_read_angle, servo_write_angle, servo);

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

bool_t unregister_servo(struct servo_t * servo)
{
	struct device_t * dev;

	if(!servo || !servo->name)
		return FALSE;

	dev = search_device(servo->name, DEVICE_TYPE_SERVO);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void servo_set_angle(struct servo_t * servo, int angle)
{
	if(servo && servo->set)
	{
		if(angle < -180)
			angle = -180;
		else if(angle > 180)
			angle = 180;
		servo->set(servo, angle);
	}
}

int servo_get_angle(struct servo_t * servo)
{
	if(servo && servo->get)
		return servo->get(servo);
	return 0;
}
