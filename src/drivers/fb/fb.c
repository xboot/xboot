/*
 * drivers/fb/fb.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

static void fb_suspend(struct device_t * dev)
{
	struct fb_t * fb;

	if(!dev || dev->type != DEVICE_TYPE_FRAMEBUFFER)
		return;

	fb = (struct fb_t *)(dev->driver);
	if(!fb)
		return;

	if(fb->suspend)
		fb->suspend(fb);
}

static void fb_resume(struct device_t * dev)
{
	struct fb_t * fb;

	if(!dev || dev->type != DEVICE_TYPE_FRAMEBUFFER)
		return;

	fb = (struct fb_t *)(dev->driver);
	if(!fb)
		return;

	if(fb->resume)
		fb->resume(fb);
}

struct fb_t * search_framebuffer(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_FRAMEBUFFER);
	if(!dev)
		return NULL;

	return (struct fb_t *)dev->driver;
}

struct fb_t * search_first_framebuffer(void)
{
	struct device_t * dev;

	dev = search_first_device_with_type(DEVICE_TYPE_FRAMEBUFFER);
	if(!dev)
		return NULL;

	return (struct fb_t *)dev->driver;
}

bool_t register_framebuffer(struct fb_t * fb)
{
	struct device_t * dev;
	int brightness = 0;

	if(!fb || !fb->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(fb->name);
	dev->type = DEVICE_TYPE_FRAMEBUFFER;
	dev->suspend = fb_suspend;
	dev->resume = fb_resume;
	dev->driver = fb;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(fb->init)
		(fb->init)(fb);

	if(fb->create)
		fb->alone = (fb->create)(fb);

	if(fb->present)
		fb->present(fb, fb->alone);

	if(fb->ioctl)
		fb->ioctl(fb, IOCTL_FB_SET_BACKLIGHT_BRIGHTNESS, &brightness);

	return TRUE;
}

bool_t unregister_framebuffer(struct fb_t * fb)
{
	struct device_t * dev;
	struct fb_t * driver;
	int brightness = 0;

	if(!fb || !fb->name)
		return FALSE;

	dev = search_device_with_type(fb->name, DEVICE_TYPE_FRAMEBUFFER);
	if(!dev)
		return FALSE;

	driver = (struct fb_t *)(dev->driver);
	if(driver)
	{
		if(fb->ioctl)
			fb->ioctl(fb, IOCTL_FB_SET_BACKLIGHT_BRIGHTNESS, &brightness);

		if(driver->exit)
			(driver->exit)(driver);
	}

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);

	return TRUE;
}

void framebuffer_set_backlight_brightness(struct fb_t * fb, int brightness)
{
	if(fb && fb->ioctl)
	{
		if(brightness < 0)
			brightness = 0;
		else if(brightness > CONFIG_MAX_BRIGHTNESS)
			brightness = CONFIG_MAX_BRIGHTNESS;
		fb->ioctl(fb, IOCTL_FB_SET_BACKLIGHT_BRIGHTNESS, &brightness);
	}
}

int framebuffer_get_backlight_brightness(struct fb_t * fb)
{
	int brightness = 0;

	if(fb && fb->ioctl)
		fb->ioctl(fb, IOCTL_FB_GET_BACKLIGHT_BRIGHTNESS, &brightness);
	return brightness;
}
