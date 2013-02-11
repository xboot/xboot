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

struct fb * fb;
struct surface_t * screen;
u32_t c;
struct surface_t * obj;

void show_image_init(void)
{
	fb = search_framebuffer("fb");
	screen = &fb->info->surface;
	c = surface_map_color(screen, get_named_color("white"));
}

void show_image(pixman_image_t * image)
{
    int width, height;
    pixman_format_code_t format;
    pixman_image_t * copy;

	width = pixman_image_get_width(image);
	height = pixman_image_get_height(image);

	format = pixman_image_get_format(image);

    /* Three cases:
     *
     *  - image is a8r8g8b8_sRGB: we will display without modification
     *    under the assumption that the monitor is sRGB
     *
     *  - image is a8r8g8b8: we will display without modification
     *    under the assumption that whoever created the image
     *    probably did it wrong by using sRGB inputs
     *
     *  - other: we will convert to a8r8g8b8 under the assumption that
     *    whoever created the image probably did it wrong.
     */
	switch (format)
	{
	case PIXMAN_a8r8g8b8_sRGB:
	case PIXMAN_a8r8g8b8:
		copy = pixman_image_ref(image);
		break;

	default:
		copy = pixman_image_create_bits(PIXMAN_a8r8g8b8, width, height, NULL,
				-1);
		pixman_image_composite32(PIXMAN_OP_SRC, image, NULL, copy, 0, 0, 0, 0,
				0, 0, width, height);
		break;
	}

	show_image_init();
	obj = surface_alloc(pixman_image_get_data(copy), width, height, PIXEL_FORMAT_ARGB_8888);

	fb->swap(fb);
	surface_fill(screen, &screen->clip, c, BLEND_MODE_REPLACE);
	surface_blit(screen, NULL, obj, NULL, BLEND_MODE_REPLACE);
	fb->flush(fb);
	surface_free(obj);
}

static int test(int argc, char ** argv)
{
	cairo_surface_t *surface;
	cairo_t *cr;

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 320, 320);
	cr = cairo_create(surface);

	cairo_save(cr);
	cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
	cairo_paint(cr);
	cairo_restore(cr);

	// "romdisk/system/media/image/battery/battery_6.png"
	/////////////////////////////////////
	double dashes[] = {50.0,  /* ink */
	                   10.0,  /* skip */
	                   10.0,  /* ink */
	                   10.0   /* skip*/
	                  };
	int    ndash  = sizeof (dashes)/sizeof(dashes[0]);
	double offset = -50.0;

	cairo_set_dash (cr, dashes, ndash, offset);
	cairo_set_line_width (cr, 10.0);

	cairo_move_to (cr, 128.0, 25.6);
	cairo_line_to (cr, 230.4, 230.4);
	cairo_rel_line_to (cr, -102.4, 0.0);
	cairo_curve_to (cr, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);

	cairo_stroke (cr);
	/////////////////////////////

	show_image(to_image_surface(surface)->pixman_image);
	cairo_surface_write_to_png(surface, "/image.png");

	cairo_destroy(cr);
	cairo_surface_destroy(surface);

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
