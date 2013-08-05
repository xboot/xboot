/*
 * init/init.c
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
#include <cairo-xboot.h>
#include <time/delay.h>
#include <time/xtime.h>
#include <xboot/menu.h>
#include <console/console.h>
#include <input/keyboard/keyboard.h>
#include <fb/fb.h>
#include <init.h>

void do_system_rootfs(void)
{
	LOG("mount root filesystem");

	if(mount(NULL, "/" , "ramfs", 0) != 0)
		LOG("failed to mount root filesystem");

	if(chdir("/") != 0)
		LOG("can't change directory to '/'");

	if(mkdir("/proc", S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("failed to create directory '/proc'");

	if(mount(NULL, "/proc" , "procfs", 0) != 0)
		LOG("failed to mount proc filesystem");

	if(mkdir("/dev", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("failed to create directory '/dev'");

	if(mount(NULL, "/dev" , "devfs", 0) != 0)
		LOG("failed to mount dev filesystem");

	if(mkdir("/romdisk", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("failed to create directory '/romdisk'");

	if(mount("/dev/romdisk", "/romdisk" , "cpiofs", 0) != 0)
		LOG("failed to mount romdisk");

	if(mkdir("/etc", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("failed to create directory '/etc'");

	if(mkdir("/tmp", S_IRWXU|S_IRWXG|S_IRWXO) != 0)
		LOG("failed to create directory '/tmp'");

	if(mkdir("/mnt", S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) != 0)
		LOG("failed to create directory '/mnt'");
}

void do_system_logo(void)
{
	cairo_surface_t * logo, * watermark;
	cairo_surface_t * cs;
	cairo_t * cr;
	struct fb_t * fb;
	int x, y;

	logo = cairo_image_surface_create_from_png("/romdisk/system/media/images/logo.png");
	watermark = cairo_image_surface_create_from_png("/romdisk/system/media/images/watermark.png");

	/* ...xxx */
	fb = get_default_framebuffer();

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

	if(!machine_authentication())
	{
		x = (cairo_image_surface_get_width(cs) - cairo_image_surface_get_width(watermark)) / 2;
		y = (cairo_image_surface_get_height(cs) - cairo_image_surface_get_height(watermark)) / 2;
		cairo_set_source_surface(cr, watermark, x, y);
		cairo_paint_with_alpha(cr, 0.9);
	}

	cairo_destroy(cr);
	cairo_xboot_surface_present(cs);
	cairo_surface_destroy(cs);

	u8_t brightness;
	if(fb->ioctl)
	{
		brightness = 0xff;
		(fb->ioctl)(fb, IOCTL_SET_FB_BACKLIGHT, &brightness);
	}

	cairo_surface_destroy(logo);
	cairo_surface_destroy(watermark);
}

void do_system_cfg(void)
{
	LOG("load system configure");

	/*
	 * load the setting of stdin, stdout and stderr console
	 */
	if(! console_stdio_load("/etc/console.xml"))
	{
		if(! console_stdio_load("/romdisk/etc/console.xml"))
			LOG("can not setting the standard console");
	}

	/*
	 * load environment variable
	 */
	if(! loadenv("/etc/environment.xml"))
	{
		if(! loadenv("/romdisk/etc/environment.xml"))
			LOG("can not load environment variable");
	}

	/*
	 * load menu context
	 */
	if(! menu_load("/etc/menu.xml"))
	{
		if(! menu_load("/romdisk/etc/menu.xml"))
			LOG("can not load menu context");
	}
}

void do_system_wait(void)
{
	u32_t timeout;

	if(get_system_hz() > 0)
	{
		LOG("wait a moment, if necessary");

		/*
		 * wait a moment for uptime until one seconds
		 */
		timeout = 0 + get_system_hz() * 1;
		while(time_before(jiffies, timeout));
	}
}
