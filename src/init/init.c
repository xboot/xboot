/*
 * init/init.c
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
#include <cairo-xboot.h>
#include <shell/system.h>
#include <console/console.h>
#include <framebuffer/framebuffer.h>
#include <init.h>

void do_showlogo(void)
{
	struct device_t * pos, * n;
	cairo_surface_t * logo;
	cairo_surface_t * cs;
	cairo_t * cr;
	struct framebuffer_t * fb;
	int x, y;

	logo = cairo_image_surface_create_from_png("/framework/assets/images/logo.png");

	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_FRAMEBUFFER], head)
	{
		if((fb = (struct framebuffer_t *)(pos->priv)))
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

			framebuffer_set_backlight(fb, CONFIG_MAX_BRIGHTNESS);
		}
	}

	cairo_surface_destroy(logo);
}

static void __do_autoboot(void)
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
extern __typeof(__do_autoboot) do_autoboot __attribute__((weak, alias("__do_autoboot")));
