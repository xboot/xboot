/*
 * driver/led/ledstrip.c
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
#include <led/ledstrip.h>

static ssize_t ledstrip_read_count(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)kobj->priv;
	int c = ledstrip_get_count(strip);
	return sprintf(buf, "%d", c);
}

static ssize_t ledstrip_write_count(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)kobj->priv;
	int c = strtol(buf, NULL, 0);
	ledstrip_set_count(strip, c);
	return size;
}

static ssize_t ledstrip_read_color(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)kobj->priv;
	int c = ledstrip_get_count(strip);
	int i, len;
	uint32_t color;
	for(i = 0, len = 0; i < c; i++)
	{
		color = ledstrip_get_color(strip, i);
		len += sprintf((char *)(buf + len), "[%3d][%3d %3d %3d]\r\n", i, (color >> 16) & 0xff, (color >> 8) & 0xff, (color >> 0) & 0xff);
	}
	return len;
}

static ssize_t ledstrip_write_color(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)kobj->priv;
	int i;
	uint32_t color;
	if(sscanf(buf, "%d:%x", &i, &color) == 2)
		ledstrip_set_color(strip, i, color);
	return size;
}

static ssize_t ledstrip_write_refresh(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledstrip_t * strip = (struct ledstrip_t *)kobj->priv;
	ledstrip_refresh(strip);
	return size;
}

struct ledstrip_t * search_ledstrip(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_LEDSTRIP);
	if(!dev)
		return NULL;
	return (struct ledstrip_t *)dev->priv;
}

bool_t register_ledstrip(struct device_t ** device, struct ledstrip_t * strip)
{
	struct device_t * dev;

	if(!strip || !strip->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(strip->name);
	dev->type = DEVICE_TYPE_LEDSTRIP;
	dev->priv = strip;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "count", ledstrip_read_count, ledstrip_write_count, strip);
	kobj_add_regular(dev->kobj, "color", ledstrip_read_color, ledstrip_write_color, strip);
	kobj_add_regular(dev->kobj, "refresh", NULL, ledstrip_write_refresh, strip);

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

bool_t unregister_ledstrip(struct ledstrip_t * strip)
{
	struct device_t * dev;

	if(!strip || !strip->name)
		return FALSE;

	dev = search_device(strip->name, DEVICE_TYPE_LEDSTRIP);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void ledstrip_set_count(struct ledstrip_t * strip, int c)
{
	if(strip && strip->set_count)
	{
		if(c < 1)
			c = 1;
		strip->set_count(strip, c);
	}
}

int ledstrip_get_count(struct ledstrip_t * strip)
{
	if(strip && strip->get_count)
		return strip->get_count(strip);
	return 0;
}

void ledstrip_set_color(struct ledstrip_t * strip, int i, uint32_t color)
{
	int c = ledstrip_get_count(strip);

	if(strip && strip->set_color)
	{
		if((i >= 0) && (i < c))
			strip->set_color(strip, i, color & 0x00ffffff);
	}
}

uint32_t ledstrip_get_color(struct ledstrip_t * strip, int i)
{
	int c = ledstrip_get_count(strip);

	if(strip && strip->get_color)
	{
		if((i >= 0) && (i < c))
			return strip->get_color(strip, i);
	}
	return 0;
}

void ledstrip_refresh(struct ledstrip_t * strip)
{
	if(strip && strip->refresh)
		strip->refresh(strip);
}
