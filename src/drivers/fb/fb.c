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

	if(!register_device(dev))
	{
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

	if(fb->backlight)
		fb->backlight(fb, 0);

	return TRUE;
}

bool_t unregister_framebuffer(struct fb_t * fb)
{
	struct device_t * dev;
	struct fb_t * driver;

	if(!fb || !fb->name)
		return FALSE;

	dev = search_device_with_type(fb->name, DEVICE_TYPE_FRAMEBUFFER);
	if(!dev)
		return FALSE;

	driver = (struct fb_t *)(dev->driver);
	if(driver)
	{
		if(fb->backlight)
			fb->backlight(fb, 0);

		if(driver->exit)
			(driver->exit)(driver);
	}

	if(!unregister_device(dev))
		return FALSE;

	free(dev->name);
	free(dev);

	return TRUE;
}
