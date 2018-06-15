/*
 * driver/hygrometer/hygrometer.c
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
#include <hygrometer/hygrometer.h>

static ssize_t hygrometer_read_humidity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct hygrometer_t * hygrometer = (struct hygrometer_t *)kobj->priv;
	int humidity = hygrometer_get_humidity(hygrometer);
	return sprintf(buf, "%d%%", humidity);
}

struct hygrometer_t * search_hygrometer(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return NULL;
	return (struct hygrometer_t *)dev->priv;
}

struct hygrometer_t * search_first_hygrometer(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return NULL;
	return (struct hygrometer_t *)dev->priv;
}

bool_t register_hygrometer(struct device_t ** device,struct hygrometer_t * hygrometer)
{
	struct device_t * dev;

	if(!hygrometer || !hygrometer->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(hygrometer->name);
	dev->type = DEVICE_TYPE_THERMOMETER;
	dev->priv = hygrometer;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "humidity", hygrometer_read_humidity, NULL, hygrometer);

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

bool_t unregister_hygrometer(struct hygrometer_t * hygrometer)
{
	struct device_t * dev;

	if(!hygrometer || !hygrometer->name)
		return FALSE;

	dev = search_device(hygrometer->name, DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int hygrometer_get_humidity(struct hygrometer_t * hygrometer)
{
	if(hygrometer && hygrometer->get)
		return hygrometer->get(hygrometer);
	return 0;
}
