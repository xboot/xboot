/*
 * driver/light/light.c
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
#include <light/light.h>

static ssize_t light_read_illuminance(struct kobj_t * kobj, void * buf, size_t size)
{
	struct light_t * light = (struct light_t *)kobj->priv;
	int lux = light_get_illuminance(light);
	return sprintf(buf, "%dlx", lux);
}

struct light_t * search_light(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_LIGHT);
	if(!dev)
		return NULL;
	return (struct light_t *)dev->priv;
}

struct light_t * search_first_light(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_LIGHT);
	if(!dev)
		return NULL;
	return (struct light_t *)dev->priv;
}

bool_t register_light(struct device_t ** device,struct light_t * light)
{
	struct device_t * dev;

	if(!light || !light->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(light->name);
	dev->type = DEVICE_TYPE_LIGHT;
	dev->driver = NULL;
	dev->priv = light;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "illuminance", light_read_illuminance, NULL, light);

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

bool_t unregister_light(struct light_t * light)
{
	struct device_t * dev;

	if(!light || !light->name)
		return FALSE;

	dev = search_device(light->name, DEVICE_TYPE_LIGHT);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int light_get_illuminance(struct light_t * light)
{
	if(light && light->get)
		return light->get(light);
	return 0;
}
