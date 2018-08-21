/*
 * driver/gyroscope/gyroscope.c
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
