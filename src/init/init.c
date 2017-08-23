/*
 * init/init.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <shell/system.h>
#include <fb/fb.h>
#include <init.h>

void do_system_logo(void)
{
	struct device_t * pos;
	struct hlist_node * n;
	cairo_surface_t * logo;
	cairo_surface_t * cs;
	cairo_t * cr;
	struct fb_t * fb;
	int x, y;

	logo = cairo_image_surface_create_from_png("/framework/assets/images/logo.png");

	hlist_for_each_entry_safe(pos, n, &__device_hash[DEVICE_TYPE_FB], node)
	{
		if((fb = (struct fb_t *)(pos->priv)))
		{
			cs = cairo_xboot_surface_create(fb, fb->alone);
			cr = cairo_create(cs);

			cairo_save(cr);
			cairo_set_source_rgb(cr, 0.2, 0.6, 0.8);
			cairo_paint(cr);
			cairo_restore(cr);

			x = (cairo_image_surface_get_width(cs) - cairo_image_surface_get_width(logo)) / 2;
			y = (cairo_image_surface_get_height(cs) - cairo_image_surface_get_height(logo)) / 2;
			cairo_set_source_surface(cr, logo, x, y);
			cairo_paint(cr);

			cairo_destroy(cr);
			cairo_xboot_surface_present(cs);
			cairo_surface_destroy(cs);

			fb_set_backlight(fb, CONFIG_MAX_BRIGHTNESS);
		}
	}

	cairo_surface_destroy(logo);
}

void do_system_autoboot(void)
{
	int delay = CONFIG_AUTO_BOOT_DELAY * 1000;

	do {
		if(getchar() != EOF)
		{
			printf("\r\n");
			return;
		}

		mdelay(10);
		delay -= 10;
		if(delay < 0)
			delay = 0;
		printf("\rPress any key to stop autoboot:%3d.%03d%s", delay / 1000, delay % 1000, (delay == 0) ? "\r\n" : "");
	} while(delay > 0);

	system(CONFIG_AUTO_BOOT_COMMAND);
}
