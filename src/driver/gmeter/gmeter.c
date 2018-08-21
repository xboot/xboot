/*
 * driver/gmeter/gmeter.c
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
#include <gmeter/gmeter.h>

static ssize_t gmeter_read_acceleration(struct kobj_t * kobj, void * buf, size_t size)
{
	struct gmeter_t * g = (struct gmeter_t *)kobj->priv;
	int x = 0, y = 0, z = 0;
	gmeter_get_acceleration(g, &x, &y, &z);
	return sprintf(buf, "[%d.%06d %d.%06d %d.%06d] m/s^2", x / 1000000, abs(x % 1000000), y / 1000000, abs(y % 1000000), z / 1000000, abs(z % 1000000));
}

struct gmeter_t * search_gmeter(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_GMETER);
	if(!dev)
		return NULL;
	return (struct gmeter_t *)dev->priv;
}

struct gmeter_t * search_first_gmeter(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_GMETER);
	if(!dev)
		return NULL;
	return (struct gmeter_t *)dev->priv;
}

bool_t register_gmeter(struct device_t ** device,struct gmeter_t * g)
{
	struct device_t * dev;

	if(!g || !g->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(g->name);
	dev->type = DEVICE_TYPE_GMETER;
	dev->priv = g;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "acceleration", gmeter_read_acceleration, NULL, g);

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

bool_t unregister_gmeter(struct gmeter_t * g)
{
	struct device_t * dev;

	if(!g || !g->name)
		return FALSE;

	dev = search_device(g->name, DEVICE_TYPE_GMETER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

bool_t gmeter_get_acceleration(struct gmeter_t * g, int * x, int * y, int * z)
{
	if(g && g->get)
		return g->get(g, x, y, z);
	return FALSE;
}
