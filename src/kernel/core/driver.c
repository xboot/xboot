/*
 * kernel/core/driver.c
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

#include <xboot/driver.h>

static struct hlist_head __driver_hash[CONFIG_DRIVER_HASH_SIZE];
static spinlock_t __driver_lock = SPIN_LOCK_INIT();

static struct hlist_head * driver_hash(const char * name)
{
	unsigned char * p = (unsigned char *)name;
	unsigned int seed = 131;
	unsigned int hash = 0;

	while(*p)
	{
		hash = hash * seed + (*p++);
	}
	return &__driver_hash[hash % ARRAY_SIZE(__driver_hash)];
}

static struct kobj_t * search_class_driver_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "driver");
}

static ssize_t driver_write_probe(struct kobj_t * kobj, void * buf, size_t size)
{
	struct driver_t * drv = (struct driver_t *)kobj->priv;
	struct dtnode_t n;
	json_value * v;
	char * p;
	int i;

	if(buf && (size > 0))
	{
		v = json_parse(buf, size);
		if(v && (v->type == json_object))
		{
			for(i = 0; i < v->u.object.length; i++)
			{
				p = (char *)(v->u.object.values[i].name);
				n.name = strsep(&p, "@");
				n.addr = p ? strtoull(p, NULL, 0) : 0;
				n.value = (json_value *)(v->u.object.values[i].value);

				if(strcmp(drv->name, n.name) == 0)
					drv->probe(drv, &n);
			}
		}
		json_value_free(v);
	}
	return size;
}

struct driver_t * search_driver(const char * name)
{
	struct driver_t * pos;
	struct hlist_node * n;

	if(!name)
		return NULL;

	hlist_for_each_entry_safe(pos, n, driver_hash(name), node)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

bool_t register_driver(struct driver_t * drv)
{
	irq_flags_t flags;

	if(!drv || !drv->name)
		return FALSE;

	if(!drv->probe || !drv->remove)
		return FALSE;

	if(!drv->suspend || !drv->resume)
		return FALSE;

	if(search_driver(drv->name))
		return FALSE;

	drv->kobj = kobj_alloc_directory(drv->name);
	kobj_add_regular(drv->kobj, "probe", NULL, driver_write_probe, drv);
	kobj_add(search_class_driver_kobj(), drv->kobj);

	spin_lock_irqsave(&__driver_lock, flags);
	init_hlist_node(&drv->node);
	hlist_add_head(&drv->node, driver_hash(drv->name));
	spin_unlock_irqrestore(&__driver_lock, flags);

	return TRUE;
}

bool_t unregister_driver(struct driver_t * drv)
{
	irq_flags_t flags;

	if(!drv || !drv->name)
		return FALSE;

	if(hlist_unhashed(&drv->node))
		return FALSE;

	spin_lock_irqsave(&__driver_lock, flags);
	hlist_del(&drv->node);
	spin_unlock_irqrestore(&__driver_lock, flags);
	kobj_remove_self(drv->kobj);

	return TRUE;
}

void probe_device(const char * json, int length)
{
	struct driver_t * drv;
	struct device_t * dev;
	struct dtnode_t n;
	json_value * v;
	char * p;
	int i;

	if(json && (length > 0))
	{
		v = json_parse(json, length);
		if(v && (v->type == json_object))
		{
			for(i = 0; i < v->u.object.length; i++)
			{
				p = (char *)(v->u.object.values[i].name);
				n.name = strsep(&p, "@");
				n.addr = p ? strtoull(p, NULL, 0) : 0;
				n.value = (json_value *)(v->u.object.values[i].value);

				drv = search_driver(n.name);
				if(drv && (dev = drv->probe(drv, &n)))
					LOG("Probe device '%s' with %s", dev->name, drv->name);
				else
					LOG("Fail to probe device with %s", n.name);
			}
		}
		json_value_free(v);
	}
}

static __init void driver_pure_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(__driver_hash); i++)
		init_hlist_head(&__driver_hash[i]);
}
pure_initcall(driver_pure_init);
