/*
 * driver/framebuffer/framebuffer.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <framebuffer/framebuffer.h>

static ssize_t framebuffer_read_width(struct kobj_t * kobj, void * buf, size_t size)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)kobj->priv;
	return sprintf(buf, "%u", framebuffer_get_width(fb));
}

static ssize_t framebuffer_read_height(struct kobj_t * kobj, void * buf, size_t size)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)kobj->priv;
	return sprintf(buf, "%u", framebuffer_get_height(fb));
}

static ssize_t framebuffer_read_pwidth(struct kobj_t * kobj, void * buf, size_t size)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)kobj->priv;
	return sprintf(buf, "%u", framebuffer_get_pwidth(fb));
}

static ssize_t framebuffer_read_pheight(struct kobj_t * kobj, void * buf, size_t size)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)kobj->priv;
	return sprintf(buf, "%u", framebuffer_get_pheight(fb));
}

static ssize_t framebuffer_read_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)kobj->priv;
	int brightness;

	brightness = framebuffer_get_backlight(fb);
	return sprintf(buf, "%d", brightness);
}

static ssize_t framebuffer_write_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)kobj->priv;
	int brightness = strtol(buf, NULL, 0);

	framebuffer_set_backlight(fb, brightness);
	return size;
}

static ssize_t framebuffer_read_max_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	return sprintf(buf, "%u", CONFIG_MAX_BRIGHTNESS);
}

struct framebuffer_t * search_framebuffer(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_FRAMEBUFFER);
	if(!dev)
		return NULL;
	return (struct framebuffer_t *)dev->priv;
}

struct framebuffer_t * search_first_framebuffer(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_FRAMEBUFFER);
	if(!dev)
		return NULL;
	return (struct framebuffer_t *)dev->priv;
}

struct device_t * register_framebuffer(struct framebuffer_t * fb, struct driver_t * drv)
{
	struct device_t * dev;

	if(!fb || !fb->name)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(fb->name);
	dev->type = DEVICE_TYPE_FRAMEBUFFER;
	dev->driver = drv;
	dev->priv = fb;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "width", framebuffer_read_width, NULL, fb);
	kobj_add_regular(dev->kobj, "height", framebuffer_read_height, NULL, fb);
	kobj_add_regular(dev->kobj, "pwidth", framebuffer_read_pwidth, NULL, fb);
	kobj_add_regular(dev->kobj, "pheight", framebuffer_read_pheight, NULL, fb);
	kobj_add_regular(dev->kobj, "brightness", framebuffer_read_brightness, framebuffer_write_brightness, fb);
	kobj_add_regular(dev->kobj, "max_brightness", framebuffer_read_max_brightness, NULL, fb);

	if(fb->setbl)
		fb->setbl(fb, 0);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_framebuffer(struct framebuffer_t * fb)
{
	struct device_t * dev;

	if(fb && fb->name)
	{
		if(fb->setbl)
			fb->setbl(fb, 0);
		dev = search_device(fb->name, DEVICE_TYPE_FRAMEBUFFER);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

void framebuffer_set_backlight(struct framebuffer_t * fb, int brightness)
{
	if(fb && fb->setbl)
	{
		if(brightness < 0)
			brightness = 0;
		else if(brightness > CONFIG_MAX_BRIGHTNESS)
			brightness = CONFIG_MAX_BRIGHTNESS;
		fb->setbl(fb, brightness);
	}
}

int framebuffer_get_backlight(struct framebuffer_t * fb)
{
	if(fb && fb->getbl)
		return fb->getbl(fb);
	return 0;
}
