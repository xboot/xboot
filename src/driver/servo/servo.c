/*
 * driver/servo/servo.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
