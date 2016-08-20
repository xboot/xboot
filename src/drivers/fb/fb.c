/*
 * driver/fb/fb.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <fb/fb.h>

static ssize_t fb_read_width(struct kobj_t * kobj, void * buf, size_t size)
{
	struct fb_t * fb = (struct fb_t *)kobj->priv;
	return sprintf(buf, "%u", fb->width);
}

static ssize_t fb_read_height(struct kobj_t * kobj, void * buf, size_t size)
{
	struct fb_t * fb = (struct fb_t *)kobj->priv;
	return sprintf(buf, "%u", fb->height);
}

static ssize_t fb_read_xdpi(struct kobj_t * kobj, void * buf, size_t size)
{
	struct fb_t * fb = (struct fb_t *)kobj->priv;
	return sprintf(buf, "%u", fb->xdpi);
}

static ssize_t fb_read_ydpi(struct kobj_t * kobj, void * buf, size_t size)
{
	struct fb_t * fb = (struct fb_t *)kobj->priv;
	return sprintf(buf, "%u", fb->ydpi);
}

static ssize_t fb_read_bpp(struct kobj_t * kobj, void * buf, size_t size)
{
	struct fb_t * fb = (struct fb_t *)kobj->priv;
	return sprintf(buf, "%u", fb->bpp);
}

static ssize_t fb_read_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct fb_t * fb = (struct fb_t *)kobj->priv;
	int brightness;

	brightness = fb_get_backlight(fb);
	return sprintf(buf, "%d", brightness);
}

static ssize_t fb_write_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	struct fb_t * fb = (struct fb_t *)kobj->priv;
	int brightness = strtol(buf, NULL, 0);

	fb_set_backlight(fb, brightness);
	return size;
}

static ssize_t fb_read_max_brightness(struct kobj_t * kobj, void * buf, size_t size)
{
	return sprintf(buf, "%u", CONFIG_MAX_BRIGHTNESS);
}

struct fb_t * search_fb(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_FB);
	if(!dev)
		return NULL;

	return (struct fb_t *)dev->priv;
}

struct fb_t * search_first_fb(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_FB);
	if(!dev)
		return NULL;

	return (struct fb_t *)dev->priv;
}

bool_t register_fb(struct device_t ** device, struct fb_t * fb)
{
	struct device_t * dev;

	if(!fb || !fb->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(fb->name);
	dev->type = DEVICE_TYPE_FB;
	dev->priv = fb;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "width", fb_read_width, NULL, fb);
	kobj_add_regular(dev->kobj, "height", fb_read_height, NULL, fb);
	kobj_add_regular(dev->kobj, "xdpi", fb_read_xdpi, NULL, fb);
	kobj_add_regular(dev->kobj, "ydpi", fb_read_ydpi, NULL, fb);
	kobj_add_regular(dev->kobj, "bpp", fb_read_bpp, NULL, fb);
	kobj_add_regular(dev->kobj, "brightness", fb_read_brightness, fb_write_brightness, fb);
	kobj_add_regular(dev->kobj, "max_brightness", fb_read_max_brightness, NULL, fb);

	if(fb->create)
		fb->alone = (fb->create)(fb);

	if(fb->present)
		fb->present(fb, fb->alone);

	if(fb->setbl)
		fb->setbl(fb, 0);

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

bool_t unregister_fb(struct fb_t * fb)
{
	struct device_t * dev;
	struct fb_t * driver;

	if(!fb || !fb->name)
		return FALSE;

	dev = search_device(fb->name, DEVICE_TYPE_FB);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	driver = (struct fb_t *)(dev->driver);
	if(driver)
	{
		if(driver->setbl)
			driver->setbl(driver, 0);
	}

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);

	return TRUE;
}

void fb_set_backlight(struct fb_t * fb, int brightness)
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

int fb_get_backlight(struct fb_t * fb)
{
	if(fb && fb->getbl)
		return fb->getbl(fb);
	return 0;
}
