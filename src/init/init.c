/*
 * init/init.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <graphic/surface.h>
#include <init.h>

void do_showlogo(void)
{
	struct device_t * pos, * n;
	struct xfs_context_t * ctx;
	struct surface_t * s, * logo;
	struct framebuffer_t * fb;
	struct matrix_t m;
	struct color_t c;

	if(!list_empty_careful(&__device_head[DEVICE_TYPE_FRAMEBUFFER]))
	{
		ctx = xfs_alloc("/framework", 0);
		if(ctx)
		{
			logo = surface_alloc_from_xfs(ctx, "assets/images/logo.png");
			if(logo)
			{
				list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_FRAMEBUFFER], head)
				{
					if((fb = (struct framebuffer_t *)(pos->priv)))
					{
						s = framebuffer_create_surface(fb);
						matrix_init_identity(&m);
						color_init(&c, 0x33, 0x99, 0xcc, 0xff);
						surface_fill(s, NULL, &m, surface_get_width(s), surface_get_height(s), &c);
						matrix_init_translate(&m, (surface_get_width(s) - surface_get_width(logo)) / 2, (surface_get_height(s) - surface_get_height(logo)) / 2);
						surface_blit(s, NULL, &m, logo);
						framebuffer_present_surface(fb, s, NULL);
						framebuffer_destroy_surface(fb, s);
						framebuffer_set_backlight(fb, CONFIG_MAX_BRIGHTNESS * 618 / 1000);
					}
				}
				surface_free(logo);
			}
		}
		xfs_free(ctx);
	}
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

	shell_system(CONFIG_AUTO_BOOT_COMMAND);
}
extern __typeof(__do_autoboot) do_autoboot __attribute__((weak, alias("__do_autoboot")));
