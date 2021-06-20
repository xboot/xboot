/*
 * driver/nvmem/nvmem.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
	struct hmap_entry_t * e;
	int len = 0;

	hmap_sort(m->kvdb.map);
	hmap_for_each_entry(e, m->kvdb.map)
	{
		len += sprintf((char *)(buf + len), "%s = %s\r\n", e->key, e->value);
	}
	return len;
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

static void hmap_entry_callback(struct hmap_entry_t * e)
{
	if(e)
		free(e->value);
}

static int kvdb_timer_function(struct timer_t * timer, void * data)
{
	struct nvmem_t * m = (struct nvmem_t *)data;
	struct hmap_entry_t * e;
	irq_flags_t flags;
	uint32_t c = 0;
	char h[8], * s;
	int size, l = 0;

	if(m->kvdb.dirty)
	{
		spin_lock_irqsave(&m->kvdb.lock, flags);
		hmap_sort(m->kvdb.map);

		size = nvmem_capacity(m);
		if(size > 0)
		{
			s = malloc(size);
			if(s)
			{
				memset(s, 0, size);
				hmap_for_each_entry(e, m->kvdb.map)
				{
					if(l + strlen(e->key) + strlen(e->value) + 3 > size)
						break;
					l += sprintf((char *)(s + l), "%s=%s;", e->key, e->value);
				}
				l += 1;
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
		m->kvdb.dirty = 0;
		spin_unlock_irqrestore(&m->kvdb.lock, flags);
	}
	return 0;
}

static void nvmem_init_kvdb(struct nvmem_t * m)
{
	irq_flags_t flags;
	uint32_t c, crc = 0;
	char h[8];
	char * p, * s;
	char * r, * k, * v;
	int l, size;

	if(m)
	{
		timer_init(&m->kvdb.timer, kvdb_timer_function, m);
		m->kvdb.map = hmap_alloc(0);
		spin_lock_init(&m->kvdb.lock);
		m->kvdb.dirty = 0;

		spin_lock_irqsave(&m->kvdb.lock, flags);
		size = nvmem_capacity(m);
		if((size > 8) && (nvmem_read(m, h, 0, 8) == 8))
		{
			c = (h[3] << 24) | (h[2] << 16) | (h[1] << 8) | (h[0] << 0);
			l = (h[7] << 24) | (h[6] << 16) | (h[5] << 8) | (h[4] << 0);
			if((l > 0) && (l + 8 < size))
			{
				s = malloc(l);
				if(nvmem_read(m, s, 8, l) == l)
				{
					crc = crc32_sum(crc, (const uint8_t *)(&h[4]), 4);
					crc = crc32_sum(crc, (const uint8_t *)s, l);
					if(crc == c)
					{
						p = s;
						while((r = strsep(&p, ";\r\n")) != NULL)
						{
							if(strchr(r, '='))
							{
								k = strim(strsep(&r, "="));
								v = strim(r);
								k = (k && (*k != '\0')) ? k : NULL;
								v = (v && (*v != '\0')) ? v : NULL;
								if(k && v)
									hmap_add(m->kvdb.map, k, strdup(v));
							}
						}
					}
				}
				free(s);
			}
		}
		spin_unlock_irqrestore(&m->kvdb.lock, flags);
	}
}

struct device_t * register_nvmem(struct nvmem_t * m, struct driver_t * drv)
{
	struct device_t * dev;

	if(!m || !m->name)
		return NULL;

	if(!m->capacity && !m->read)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	nvmem_init_kvdb(m);
	dev->name = strdup(m->name);
	dev->type = DEVICE_TYPE_NVMEM;
	dev->driver = drv;
	dev->priv = m;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "summary", nvmem_read_summary, NULL, m);
	kobj_add_regular(dev->kobj, "capacity", nvmem_read_capacity, NULL, m);

	if(!register_device(dev))
	{
		timer_cancel(&m->kvdb.timer);
		hmap_free(m->kvdb.map, hmap_entry_callback);
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_nvmem(struct nvmem_t * m)
{
	struct device_t * dev;

	if(m && m->name)
	{
		dev = search_device(m->name, DEVICE_TYPE_NVMEM);
		if(dev && unregister_device(dev))
		{
			timer_cancel(&m->kvdb.timer);
			hmap_free(m->kvdb.map, hmap_entry_callback);
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
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
	irq_flags_t flags;
	char * v;

	spin_lock_irqsave(&m->kvdb.lock, flags);
	v = hmap_search(m->kvdb.map, key);
	if(v)
	{
		m->kvdb.dirty = 1;
		hmap_remove(m->kvdb.map, key);
		free(v);
	}
	if(value)
	{
		m->kvdb.dirty = 1;
		hmap_add(m->kvdb.map, key, strdup(value));
	}
	if(m->kvdb.dirty)
		timer_start_now(&m->kvdb.timer, ms_to_ktime(10000));
	spin_unlock_irqrestore(&m->kvdb.lock, flags);
}

const char * nvmem_get(struct nvmem_t * m, const char * key, const char * def)
{
	irq_flags_t flags;
	const char * v;

	spin_lock_irqsave(&m->kvdb.lock, flags);
	v = hmap_search(m->kvdb.map, key);
	spin_unlock_irqrestore(&m->kvdb.lock, flags);
	if(!v)
		v = def;
	return v;
}

void nvmem_clear(struct nvmem_t * m)
{
	irq_flags_t flags;

	spin_lock_irqsave(&m->kvdb.lock, flags);
	hmap_clear(m->kvdb.map, hmap_entry_callback);
	m->kvdb.dirty = 1;
	timer_start_now(&m->kvdb.timer, ms_to_ktime(10000));
	spin_unlock_irqrestore(&m->kvdb.lock, flags);
}
