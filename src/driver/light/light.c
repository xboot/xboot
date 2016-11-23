/*
 * driver/light/light.c
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
#include <light/light.h>

static ssize_t light_read_illuminance(struct kobj_t * kobj, void * buf, size_t size)
{
	struct light_t * light = (struct light_t *)kobj->priv;
	int lux = light_get_illuminance(light);
	return sprintf(buf, "%dlx", lux);
}

struct light_t * search_light(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_LIGHT);
	if(!dev)
		return NULL;

	return (struct light_t *)dev->priv;
}

struct light_t * search_first_light(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_LIGHT);
	if(!dev)
		return NULL;

	return (struct light_t *)dev->priv;
}

bool_t register_light(struct device_t ** device,struct light_t * light)
{
	struct device_t * dev;

	if(!light || !light->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(light->name);
	dev->type = DEVICE_TYPE_LIGHT;
	dev->priv = light;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "illuminance", light_read_illuminance, NULL, light);

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

bool_t unregister_light(struct light_t * light)
{
	struct device_t * dev;

	if(!light || !light->name)
		return FALSE;

	dev = search_device(light->name, DEVICE_TYPE_LIGHT);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int light_get_illuminance(struct light_t * light)
{
	if(light && light->get)
		return light->get(light);
	return 0;
}
