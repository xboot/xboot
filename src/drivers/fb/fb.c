/*
 * drivers/fb/fb.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot.h>
#include <malloc.h>
#include <xboot/chrdev.h>
#include <xboot/ioctl.h>
#include <console/console.h>
#include <fb/logo.h>
#include <fb/graphic.h>
#include <fb/fb.h>

struct fbcon_cell
{
	x_u32 cp;
	x_u32 fc, bc;
	x_bool dirty;
};

/*
 * defined the framebuffer console information
 */
struct fb_console_info
{
	/* the console name */
	char name[32+1];

	/* framebuffer driver */
	struct fb * fb;

	/* console width and height */
	x_s32 w, h;

	/* console current x, y */
	x_s32 x, y;

	/* console front color and background color */
	enum console_color f, b;
	x_u32 fc, bc;

	/* cursor status, on or off */
	x_bool cursor;

	/* fb console's cell */
	struct fbcon_cell * cell;

	/*
	 * below for priv data
	 */
};

/*
 * fb open
 */
static x_s32 fb_open(struct chrdev * dev)
{
	return 0;
}

/*
 * fb read
 */
static x_s32 fb_read(struct chrdev * dev, x_u8 * buf, x_s32 count)
{
	struct fb * fb = (struct fb *)(dev->driver);
	x_u8 * p = (x_u8 *)((x_u32)(fb->info->bitmap.data));
	x_s32 i;

	for(i = 0; i < count; i++)
	{
		buf[i] = p[i];
	}

	return i;
}

/*
 * fb write.
 */
static x_s32 fb_write(struct chrdev * dev, const x_u8 * buf, x_s32 count)
{
	struct fb * fb = (struct fb *)(dev->driver);
	x_u8 * p = (x_u8 *)((x_u32)(fb->info->bitmap.data));
	x_s32 i;

	for(i = 0; i < count; i++)
	{
		p[i] = buf[i];
	}

	return i;
}

/*
 * fb ioctl
 */
static x_s32 fb_ioctl(struct chrdev * dev, x_u32 cmd, void * arg)
{
	struct fb * fb = (struct fb *)(dev->driver);

	if(fb->ioctl)
		return ((fb->ioctl)(cmd, arg));

	return -1;
}

/*
 * fb close
 */
static x_s32 fb_close(struct chrdev * dev)
{
	return 0;
}

/*
 * get console's width and height
 */
static x_bool fbcon_getwh(struct console * console, x_s32 * w, x_s32 * h)
{
	struct fb_console_info * info = console->priv;

	if(w)
		*w = info->w;

	if(h)
		*h = info->h;

	return TRUE;
}

/*
 * get cursor position
 */
static x_bool fbcon_getxy(struct console * console, x_s32 * x, x_s32 * y)
{
	struct fb_console_info * info = console->priv;

	if(x)
		*x = info->x;

	if(y)
		*y = info->y;

	return TRUE;
}

/*
 * set cursor position
 */
static x_bool fbcon_gotoxy(struct console * console, x_s32 x, x_s32 y)
{
	struct fb_console_info * info = console->priv;
/*
	*x = info->x;

	if(y)
		*y = info->y;*/

	return TRUE;
}

/*
 * search framebuffer by name.
 */
struct fb * search_framebuffer(const char * name)
{
	struct fb * fb;
	struct chrdev * dev;

	dev = search_chrdev(name);
	if(!dev)
		return NULL;

	if(dev->type != CHR_DEV_FRAMEBUFFER)
		return NULL;

	fb = (struct fb *)dev->driver;

	return fb;
}

/*
 * register framebuffer driver.
 */
x_bool register_framebuffer(struct fb * fb)
{
	struct chrdev * dev;
	x_u8 brightness;

	if(!fb || !fb->info || !fb->info->name)
		return FALSE;

	dev = malloc(sizeof(struct chrdev));
	if(!dev)
		return FALSE;

	dev->name		= fb->info->name;
	dev->type		= CHR_DEV_FRAMEBUFFER;
	dev->open 		= fb_open;
	dev->read 		= fb_read;
	dev->write 		= fb_write;
	dev->ioctl 		= fb_ioctl;
	dev->close		= fb_close;
	dev->driver 	= fb;

	if(!register_chrdev(dev))
	{
		free(dev);
		return FALSE;
	}

	if(search_chrdev_with_type(dev->name, CHR_DEV_FRAMEBUFFER) == NULL)
	{
		unregister_chrdev(dev->name);
		free(dev);
		return FALSE;
	}

	if(fb->init)
		(fb->init)();

	display_logo(fb);

	if(fb->ioctl)
	{
		brightness = 0xff;
		(fb->ioctl)(IOCTL_SET_FB_BACKLIGHT, &brightness);
	}

	return TRUE;
}

/*
 * unregister framebuffer driver
 */
x_bool unregister_framebuffer(struct fb * fb)
{
	struct chrdev * dev;
	struct fb * driver;
	x_u8 brightness;

	if(!fb || !fb->info || !fb->info->name)
		return FALSE;

	dev = search_chrdev_with_type(fb->info->name, CHR_DEV_FRAMEBUFFER);
	if(!dev)
		return FALSE;

	driver = (struct fb *)(dev->driver);
	if(driver)
	{
		if(driver->ioctl)
		{
			brightness = 0x00;
			(fb->ioctl)(IOCTL_SET_FB_BACKLIGHT, &brightness);
		}

		if(driver->exit)
			(driver->exit)();
	}

	if(!unregister_chrdev(dev->name))
		return FALSE;

	free(dev);

	return TRUE;
}
