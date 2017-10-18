/*
 * driver/nvmem/nvmem.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <nvmem/nvmem.h>

static ssize_t nvmem_read_capacity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct nvmem_t * m = (struct nvmem_t *)kobj->priv;
	return sprintf(buf, "%d", nvmem_capacity(m));
}

struct nvmem_t * search_nvmem(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_NVMEM);
	if(!dev)
		return NULL;

	return (struct nvmem_t *)dev->priv;
}

struct nvmem_t * search_first_nvmem(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_NVMEM);
	if(!dev)
		return NULL;

	return (struct nvmem_t *)dev->priv;
}

bool_t register_nvmem(struct device_t ** device, struct nvmem_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(m->name);
	dev->type = DEVICE_TYPE_NVMEM;
	dev->priv = m;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "capacity", nvmem_read_capacity, NULL, m);

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

bool_t unregister_nvmem(struct nvmem_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	dev = search_device(m->name, DEVICE_TYPE_NVMEM);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int nvmem_read(struct nvmem_t * m, void * buf, int offset, int count)
{
	int capacity;

	if(m && m->read)
	{
		capacity = m->capacity(m);
		if(offset < 0)
			offset = 0;
		else if(offset > capacity)
			offset = capacity;
		if(count < 0)
			count = 0;
		else if(count > capacity - offset)
			count = capacity - offset;
		return m->read(m, buf, offset, count);
	}
	return 0;
}

int nvmem_write(struct nvmem_t * m, void * buf, int offset, int count)
{
	int capacity;

	if(m && m->write)
	{
		capacity = m->capacity(m);
		if(offset < 0)
			offset = 0;
		else if(offset > capacity)
			offset = capacity;
		if(count < 0)
			count = 0;
		else if(count > capacity - offset)
			count = capacity - offset;
		return m->write(m, buf, offset, count);
	}
	return 0;
}

int nvmem_capacity(struct nvmem_t * m)
{
	if(m && m->capacity)
		return m->capacity(m);
	return 0;
}
