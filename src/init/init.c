/*
 * init/init.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <cairo-xboot.h>
#include <console/console.h>
#include <fb/fb.h>
#include <init.h>

void do_system_rootfs(void)
{
	LOG("Mount root filesystem");

	if(mount(NULL, "/" , "ramfs", 0) != 0)
		LOG("Failed to mount root filesystem");

	if(chdir("/") != 0)
		LOG("Can't change directory to '/'");

	if(mkdir("/sys", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("Failed to create directory '/sys'");

	if(mount(NULL, "/sys" , "sysfs", 0) != 0)
		LOG("Failed to mount sys filesystem");

	if(mkdir("/romdisk", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("Failed to create directory '/romdisk'");

	if(mount("romdisk", "/romdisk" , "cpiofs", 0) != 0)
		LOG("Failed to mount romdisk");

	if(mkdir("/etc", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("Failed to create directory '/etc'");

	if(mkdir("/tmp", S_IRWXU|S_IRWXG|S_IRWXO) != 0)
		LOG("Failed to create directory '/tmp'");

	if(mkdir("/mnt", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("Failed to create directory '/mnt'");
}

void do_system_logo(void)
{
	struct device_list_t * pos, * n;
	cairo_surface_t * watermark;
	cairo_surface_t * logo;
	cairo_surface_t * cs;
	cairo_t * cr;
	struct fb_t * fb;
	int x, y;

	LOG("Display system logo");

	if(! machine_authentication())
		watermark = cairo_image_surface_create_from_png("/romdisk/framework/assets/images/watermark.png");
	else
		watermark = NULL;
	logo = cairo_image_surface_create_from_png("/romdisk/framework/assets/images/logo.png");

	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type != DEVICE_TYPE_FRAMEBUFFER)
			continue;

		fb = (struct fb_t *)(pos->device->driver);
		if(fb)
		{
			cs = cairo_xboot_surface_create(fb, fb->alone);
			cr = cairo_create(cs);

			cairo_save(cr);
			cairo_set_source_rgb(cr, 0, 0, 0);
			cairo_paint(cr);
			cairo_restore(cr);

			x = (cairo_image_surface_get_width(cs) - cairo_image_surface_get_width(logo)) / 2;
			y = (cairo_image_surface_get_height(cs) - cairo_image_surface_get_height(logo)) / 2;
			cairo_set_source_surface(cr, logo, x, y);
			cairo_paint(cr);

			if(watermark)
			{
				x = (cairo_image_surface_get_width(cs) - cairo_image_surface_get_width(watermark)) / 2;
				y = (cairo_image_surface_get_height(cs) - cairo_image_surface_get_height(watermark)) / 2;
				cairo_set_source_surface(cr, watermark, x, y);
				cairo_paint_with_alpha(cr, 0.9);
			}

			cairo_destroy(cr);
			cairo_xboot_surface_present(cs);
			cairo_surface_destroy(cs);

			framebuffer_set_backlight_brightness(fb, CONFIG_MAX_BRIGHTNESS);
		}
	}

	if(watermark)
		cairo_surface_destroy(watermark);
	cairo_surface_destroy(logo);
}

void do_system_wait(void)
{
	u32_t timeout;

	if(HZ > 0)
	{
		LOG("Wait a moment, if necessary");

		/*
		 * Wait a moments
		 */
		timeout = msecs_to_jiffies(1000);
		while(time_before(jiffies, timeout));
	}
}
