/*
 * driver/rng/rng.c
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
#include <rng/rng.h>

static ssize_t rng_read_random(struct kobj_t * kobj, void * buf, size_t size)
{
	struct rng_t * rng = (struct rng_t *)kobj->priv;
	unsigned char dat = 0;

	rng_read_data(rng, &dat, 1, 0);
	return sprintf(buf, "%02x", dat);
}

struct rng_t * search_rng(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_RNG);
	if(!dev)
		return NULL;

	return (struct rng_t *)dev->priv;
}

struct rng_t * search_first_rng(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_RNG);
	if(!dev)
		return NULL;

	return (struct rng_t *)dev->priv;
}

bool_t register_rng(struct device_t ** device, struct rng_t * rng)
{
	struct device_t * dev;

	if(!rng || !rng->name || !rng->read)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(rng->name);
	dev->type = DEVICE_TYPE_RNG;
	dev->priv = rng;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "random", rng_read_random, NULL, rng);

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

bool_t unregister_rng(struct rng_t * rng)
{
	struct device_t * dev;

	if(!rng || !rng->name)
		return FALSE;

	dev = search_device(rng->name, DEVICE_TYPE_RNG);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int rng_read_data(struct rng_t * rng, void * buf, int max, int wait)
{
	if(rng && rng->read)
		return rng->read(rng, buf, max, wait);
	return 0;
}
