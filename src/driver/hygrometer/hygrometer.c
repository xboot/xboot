/*
 * driver/hygrometer/hygrometer.c
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
#include <hygrometer/hygrometer.h>

static ssize_t hygrometer_read_humidity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct hygrometer_t * hygrometer = (struct hygrometer_t *)kobj->priv;
	int humidity = hygrometer_get_humidity(hygrometer);
	return sprintf(buf, "%d%%", humidity);
}

struct hygrometer_t * search_hygrometer(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return NULL;
	return (struct hygrometer_t *)dev->priv;
}

struct hygrometer_t * search_first_hygrometer(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return NULL;
	return (struct hygrometer_t *)dev->priv;
}

bool_t register_hygrometer(struct device_t ** device,struct hygrometer_t * hygrometer)
{
	struct device_t * dev;

	if(!hygrometer || !hygrometer->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(hygrometer->name);
	dev->type = DEVICE_TYPE_THERMOMETER;
	dev->priv = hygrometer;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "humidity", hygrometer_read_humidity, NULL, hygrometer);

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

bool_t unregister_hygrometer(struct hygrometer_t * hygrometer)
{
	struct device_t * dev;

	if(!hygrometer || !hygrometer->name)
		return FALSE;

	dev = search_device(hygrometer->name, DEVICE_TYPE_THERMOMETER);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int hygrometer_get_humidity(struct hygrometer_t * hygrometer)
{
	if(hygrometer && hygrometer->get)
		return hygrometer->get(hygrometer);
	return 0;
}
