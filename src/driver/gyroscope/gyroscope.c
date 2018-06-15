/*
 * driver/gyroscope/gyroscope.c
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
#include <gyroscope/gyroscope.h>

static ssize_t gyroscope_read_palstance(struct kobj_t * kobj, void * buf, size_t size)
{
	struct gyroscope_t * g = (struct gyroscope_t *)kobj->priv;
	int x = 0, y = 0, z = 0;
	gyroscope_get_palstance(g, &x, &y, &z);
	return sprintf(buf, "[%d.%06d %d.%06d %d.%06d] rad/s", x / 1000000, abs(x % 1000000), y / 1000000, abs(y % 1000000), z / 1000000, abs(z % 1000000));
}

struct gyroscope_t * search_gyroscope(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_GYROSCOPE);
	if(!dev)
		return NULL;
	return (struct gyroscope_t *)dev->priv;
}

struct gyroscope_t * search_first_gyroscope(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_GYROSCOPE);
	if(!dev)
		return NULL;
	return (struct gyroscope_t *)dev->priv;
}

bool_t register_gyroscope(struct device_t ** device,struct gyroscope_t * g)
{
	struct device_t * dev;

	if(!g || !g->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(g->name);
	dev->type = DEVICE_TYPE_GYROSCOPE;
	dev->priv = g;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "palstance", gyroscope_read_palstance, NULL, g);

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

bool_t unregister_gyroscope(struct gyroscope_t * g)
{
	struct device_t * dev;

	if(!g || !g->name)
		return FALSE;

	dev = search_device(g->name, DEVICE_TYPE_GYROSCOPE);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

bool_t gyroscope_get_palstance(struct gyroscope_t * g, int * x, int * y, int * z)
{
	if(g && g->get)
		return g->get(g, x, y, z);
	return FALSE;
}
