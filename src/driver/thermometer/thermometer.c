/*
 * driver/thermometer/thermometer.c
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
#include <thermometer/thermometer.h>

static ssize_t thermometer_read_temperature(struct kobj_t * kobj, void * buf, size_t size)
{
	struct thermometer_t * thermometer = (struct thermometer_t *)kobj->priv;
	int temperature = thermometer_get_temperature(thermometer);
	return sprintf(buf, "%d.%dC", temperature / 10, abs(temperature % 10));
}

struct thermometer_t * search_thermometer(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return NULL;

	return (struct thermometer_t *)dev->priv;
}

struct thermometer_t * search_first_thermometer(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return NULL;

	return (struct thermometer_t *)dev->priv;
}

bool_t register_thermometer(struct device_t ** device,struct thermometer_t * thermometer)
{
	struct device_t * dev;

	if(!thermometer || !thermometer->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(thermometer->name);
	dev->type = DEVICE_TYPE_THERMOMETER;
	dev->priv = thermometer;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "temperature", thermometer_read_temperature, NULL, thermometer);

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

bool_t unregister_thermometer(struct thermometer_t * thermometer)
{
	struct device_t * dev;

	if(!thermometer || !thermometer->name)
		return FALSE;

	dev = search_device(thermometer->name, DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int thermometer_get_temperature(struct thermometer_t * thermometer)
{
	if(thermometer && thermometer->get)
		return thermometer->get(thermometer);
	return 0;
}
