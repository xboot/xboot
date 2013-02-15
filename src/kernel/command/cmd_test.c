/*
 * xboot/kernel/command/cmd_test.c
 */

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <div64.h>
#include <fifo.h>
#include <byteorder.h>
#include <sha.h>
#include <xml.h>
#include <io.h>
#include <math.h>
#include <time/delay.h>
#include <time/timer.h>
#include <time/xtime.h>
#include <time/tick.h>
#include <xboot/machine.h>
#include <xboot/list.h>
#include <xboot/log.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <xboot/chrdev.h>
#include <xboot/module.h>
#include <xboot/proc.h>
#include <shell/exec.h>
#include <fb/fb.h>
#include <fb/logo.h>
#include <rtc/rtc.h>
#include <input/input.h>
#include <input/keyboard/keyboard.h>
#include <console/console.h>
#include <loop/loop.h>
#include <command/command.h>
#include <fs/vfs/vfs.h>
#include <fs/fileio.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>
#include <graphic/surface.h>

#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)
#include "pixman.h"
#include <cairo.h>
#include "cairo-types-private.h"
#include "cairo-image-surface-private.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

void show_to_framebuffer(cairo_surface_t * surface)
{
	static struct fb * fb;
	static struct surface_t * screen;
	static u32_t c;
	static bool_t init = FALSE;
	pixman_image_t * image;
	struct surface_t * obj;
	int width, height;

	if(! init)
	{
		fb = search_framebuffer("fb");
		screen = &fb->info->surface;
		c = surface_map_color(screen, get_named_color("darkkhaki"));
		init = TRUE;
	}

	if (!surface)
		return;

	image = to_image_surface(surface)->pixman_image;
	if (!image)
		return;

	width = pixman_image_get_width(image);
	height = pixman_image_get_height(image);
	obj = surface_alloc(pixman_image_get_data(image), width, height, PIXEL_FORMAT_ARGB_8888);
	fb->swap(fb);
	surface_fill(screen, &screen->clip, c, BLEND_MODE_REPLACE);
	surface_blit(screen, NULL, obj, NULL, BLEND_MODE_REPLACE);
	fb->flush(fb);
	surface_free(obj);
}

static int test(int argc, char ** argv)
{
	cairo_surface_t * cs;
	cairo_t * cr;

	cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 480);
	cr = cairo_create(cs);

	cairo_save(cr);
	cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
	cairo_paint(cr);
	cairo_restore(cr);

	/* START ===================================== */
	double xc = 128.0;
	double yc = 128.0;
	double radius = 100.0;
	double angle1 = 45.0  * (M_PI/180.0);  /* angles are specified */
	double angle2 = 180.0 * (M_PI/180.0);  /* in radians           */

	cairo_set_line_width (cr, 10.0);
	cairo_arc (cr, xc, yc, radius, angle1, angle2);
	cairo_stroke (cr);

	/* draw helping lines */
	cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
	cairo_set_line_width (cr, 6.0);

	cairo_arc (cr, xc, yc, 10.0, 0, 2*M_PI);
	cairo_fill (cr);

	cairo_arc (cr, xc, yc, radius, angle1, angle1);
	cairo_line_to (cr, xc, yc);
	cairo_arc (cr, xc, yc, radius, angle2, angle2);
	cairo_line_to (cr, xc, yc);
	cairo_stroke (cr);
	/* END ===================================== */

	show_to_framebuffer(cs);
	cairo_surface_write_to_png(cs, "/image.png");

	cairo_destroy(cr);
	cairo_surface_destroy(cs);
	return 0;
}

static struct command test_cmd = {
	.name		= "test",
	.func		= test,
	.desc		= "test command for debug\r\n",
	.usage		= "test [arg ...]\r\n",
	.help		= "    test command for debug software by programmer.\r\n"
};

static __init void test_cmd_init(void)
{
	if(!command_register(&test_cmd))
		LOG_E("register 'test' command fail");
}

static __exit void test_cmd_exit(void)
{
	if(!command_unregister(&test_cmd))
		LOG_E("unregister 'test' command fail");
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);

#endif
