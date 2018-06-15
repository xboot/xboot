/*
 * driver/proximity/proximity.c
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
#include <proximity/proximity.h>

static ssize_t proximity_read_distance(struct kobj_t * kobj, void * buf, size_t size)
{
	struct proximity_t * p = (struct proximity_t *)kobj->priv;
	int d = proximity_get_distance(p);
	return sprintf(buf, "%dmm", d);
}

struct proximity_t * search_proximity(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_PROXIMITY);
	if(!dev)
		return NULL;
	return (struct proximity_t *)dev->priv;
}

struct proximity_t * search_first_proximity(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_PROXIMITY);
	if(!dev)
		return NULL;
	return (struct proximity_t *)dev->priv;
}

bool_t register_proximity(struct device_t ** device,struct proximity_t * p)
{
	struct device_t * dev;

	if(!p || !p->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(p->name);
	dev->type = DEVICE_TYPE_PROXIMITY;
	dev->priv = p;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "distance", proximity_read_distance, NULL, p);

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

bool_t unregister_proximity(struct proximity_t * p)
{
	struct device_t * dev;

	if(!p || !p->name)
		return FALSE;

	dev = search_device(p->name, DEVICE_TYPE_PROXIMITY);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int proximity_get_distance(struct proximity_t * p)
{
	if(p && p->get)
		return p->get(p);
	return 0;
}
