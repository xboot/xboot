/*
 * driver/nvmem/nvmem.c
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

#include <crc32.h>
#include <nvmem/nvmem.h>

static ssize_t nvmem_read_summary(struct kobj_t * kobj, void * buf, size_t size)
{
	struct nvmem_t * m = (struct nvmem_t *)kobj->priv;
	return kvdb_summary(m->db, buf);
}

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

static bool_t nvmem_init_kvdb(struct nvmem_t * m)
{
	uint32_t c, crc = 0;
	char h[8];
	char * s;
	int l, size;

	if(!m)
		return FALSE;
	m->db = NULL;

	size = nvmem_capacity(m);
	if(size < (8 + 1))
		return FALSE;

	if(nvmem_read(m, h, 0, 8) != 8)
		return FALSE;
	c = (h[3] << 24) | (h[2] << 16) | (h[1] << 8) | (h[0] << 0);
	l = (h[7] << 24) | (h[6] << 16) | (h[5] << 8) | (h[4] << 0);

	m->db = kvdb_alloc(size);
	if(m->db)
	{
		if((l > 0) && (l + 8 < size))
		{
			s = malloc(l);
			if(!s)
			{
				kvdb_free(m->db);
				return FALSE;
			}
			if(nvmem_read(m, s, 8, l) != l)
			{
				kvdb_free(m->db);
				free(s);
				return FALSE;
			}
			crc = crc32_sum(crc, (const uint8_t *)(&h[4]), 4);
			crc = crc32_sum(crc, (const uint8_t *)s, l);
			if(crc == c)
				kvdb_from_string(m->db, s);
			else
				nvmem_sync(m);
			free(s);
		}
		else
		{
			nvmem_sync(m);
		}
	}
	return TRUE;
}

bool_t register_nvmem(struct device_t ** device, struct nvmem_t * m)
{
	struct device_t * dev;

	if(!m || !m->name)
		return FALSE;

	if(!m->capacity && !m->read)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	nvmem_init_kvdb(m);
	dev->name = strdup(m->name);
	dev->type = DEVICE_TYPE_NVMEM;
	dev->priv = m;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "summary", nvmem_read_summary, NULL, m);
	kobj_add_regular(dev->kobj, "capacity", nvmem_read_capacity, NULL, m);

	if(!register_device(dev))
	{
		if(m->db)
			kvdb_free(m->db);
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

	if(m->db)
		kvdb_free(m->db);
	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int nvmem_capacity(struct nvmem_t * m)
{
	if(m && m->capacity)
		return m->capacity(m);
	return 0;
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

void nvmem_set(struct nvmem_t * m, const char * key, const char * value)
{
	if(m && m->db)
		kvdb_set(m->db, key, value);
}

char * nvmem_get(struct nvmem_t * m, const char * key, const char * def)
{
	if(m && m->db)
		return kvdb_get(m->db, key, def);
	return (char *)def;
}

void nvmem_clear(struct nvmem_t * m)
{
	if(m && m->db)
		return kvdb_clear(m->db);
}

void nvmem_sync(struct nvmem_t * m)
{
	uint32_t c = 0;
	char h[8];
	char * s;
	int l;

	if(m && m->db)
	{
		s = kvdb_to_string(m->db);
		if(s)
		{
			l = strlen(s) + 1;
			h[4] = (l >>  0) & 0xff;
			h[5] = (l >>  8) & 0xff;
			h[6] = (l >> 16) & 0xff;
			h[7] = (l >> 24) & 0xff;
			c = crc32_sum(c, (const uint8_t *)(&h[4]), 4);
			c = crc32_sum(c, (const uint8_t *)s, l);
			h[0] = (c >>  0) & 0xff;
			h[1] = (c >>  8) & 0xff;
			h[2] = (c >> 16) & 0xff;
			h[3] = (c >> 24) & 0xff;
			nvmem_write(m, h, 0, 8);
			nvmem_write(m, s, 8, l);
			free(s);
		}
	}
}
