/*
 * driver/pressure/pressure.c
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

#include <xboot.h>
#include <pressure/pressure.h>

static ssize_t pressure_read_pascal(struct kobj_t * kobj, void * buf, size_t size)
{
	struct pressure_t * p = (struct pressure_t *)kobj->priv;
	int pa = pressure_get_pascal(p);
	return sprintf(buf, "%dPa", pa);
}

struct pressure_t * search_pressure(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_PRESSURE);
	if(!dev)
		return NULL;
	return (struct pressure_t *)dev->priv;
}

struct pressure_t * search_first_pressure(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_PRESSURE);
	if(!dev)
		return NULL;
	return (struct pressure_t *)dev->priv;
}

bool_t register_pressure(struct device_t ** device,struct pressure_t * p)
{
	struct device_t * dev;

	if(!p || !p->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(p->name);
	dev->type = DEVICE_TYPE_PRESSURE;
	dev->priv = p;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "pascal", pressure_read_pascal, NULL, p);

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

bool_t unregister_pressure(struct pressure_t * p)
{
	struct device_t * dev;

	if(!p || !p->name)
		return FALSE;

	dev = search_device(p->name, DEVICE_TYPE_PRESSURE);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int pressure_get_pascal(struct pressure_t * p)
{
	if(p && p->get)
		return p->get(p);
	return 0;
}
