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
/*	switch (format)
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
*/
//	show_image_init();
	obj = surface_alloc(pixman_image_get_data(image), width, height, PIXEL_FORMAT_ARGB_8888);
	fb->swap(fb);
	surface_fill(screen, &screen->clip, c, BLEND_MODE_REPLACE);
	surface_blit(screen, NULL, obj, NULL, BLEND_MODE_REPLACE);
	fb->flush(fb);
	surface_free(obj);
}

void lcd(cairo_surface_t * surface)
{
	static int init = 0;

	if(!init)
	{
		show_image_init();
		init = 1;
	}

	show_image(to_image_surface(surface)->pixman_image);
}

/*
 *
 *
 */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Draws a clock on a normalized Cairo context */
static void
draw (cairo_t *cr)
{
    static struct tm __tm;
    struct tm *tm = &__tm;
    double seconds, minutes, hours;

    tm->tm_sec++;
    if(tm->tm_sec >= 60)
    {
    	tm->tm_sec = 0;
    	tm->tm_min++;
    	if(tm->tm_min >= 60)
    	{
    		tm->tm_min = 0;
    		tm->tm_hour++;
    		if(tm->tm_hour >= 24)
    			tm->tm_hour = 0;
    	}
    }

    /* compute the angles for the indicators of our clock */
    seconds = tm->tm_sec * M_PI / 30;
    minutes = tm->tm_min * M_PI / 30;
    hours = tm->tm_hour * M_PI / 6;

    /* Fill the background with white. */
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    /* who doesn't want all those nice line settings :) */
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width (cr, 0.1);

    /* translate to the center of the rendering context and draw a black */
    /* clock outline */
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_translate (cr, 0.5, 0.5);
    cairo_arc (cr, 0, 0, 0.4, 0, M_PI * 2);
    cairo_stroke (cr);

    /* draw a white dot on the current second. */
    cairo_set_source_rgba (cr, 1, 1, 1, 0.6);
    cairo_arc (cr,
	       sin (seconds) * 0.4, -cos (seconds) * 0.4,
	       0.05, 0, M_PI * 2);
    cairo_fill (cr);

    /* draw the minutes indicator */
    cairo_set_source_rgba (cr, 0.2, 0.2, 1, 0.6);
    cairo_move_to (cr, 0, 0);
    cairo_line_to (cr, sin (minutes) * 0.4, -cos (minutes) * 0.4);
    cairo_stroke (cr);

    /* draw the hours indicator      */
    cairo_move_to (cr, 0, 0);
    cairo_line_to (cr, sin (hours) * 0.2, -cos (hours) * 0.2);
    cairo_stroke (cr);
}

void run(void)
{
	cairo_surface_t *surface;
	cairo_t * cr;
	int width = 200;
	int height = 200;
	static int count = 0;
	static int flag_w = 1;
	static int flag_h = 1;

	show_image_init();

	while (1)
	{
		if(flag_w)
		{
			if(++width >=799)
				flag_w = 0;
		}
		else
		{
			if(--width < 100)
				flag_w = 1;
		}

		if(flag_h)
		{
			if(++height >=479)
				flag_h = 0;
		}
		else
		{
			if(--height < 100)
				flag_h = 1;
		}

		surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
		cr = cairo_create(surface);

		cairo_scale(cr, width, height);
		draw(cr);

//		printk("count=%d\r\n", count++);
		show_image(to_image_surface(surface) ->pixman_image);

		cairo_destroy(cr);
		cairo_surface_destroy(surface);
	}

}
/*
 *
 *
 */

static int test(int argc, char ** argv)
{
	cairo_surface_t *surface;
	cairo_t *cr;

	run();
	return 0 ;

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 750, 430);
	cr = cairo_create(surface);

	cairo_save(cr);
	cairo_set_source_rgb(cr, 0.95, 0.95, 0.95);
	cairo_paint(cr);
	cairo_restore(cr);

	// /romdisk/system/media/image/battery/battery_6.png
	/////////////////////////////////////

	FT_Library library;
	FT_Face face;
	FT_Init_FreeType( &library );
	FT_New_Face( library, "/romdisk/DroidSansFallback.ttf", 0, &face );
	cairo_font_face_t * myfont_face;
	myfont_face =  cairo_ft_font_face_create_for_ft_face(face, 0);

	cairo_set_font_face(cr, myfont_face);

	//////////////////////////
	cairo_text_extents_t extents;

	const char *utf8 = "111啊A哦哦o";
	double x,y;

/*	cairo_select_font_face (cr, "Sans",
	    CAIRO_FONT_SLANT_NORMAL,
	    CAIRO_FONT_WEIGHT_NORMAL);*/

	cairo_set_font_size (cr, 152.0);
	cairo_text_extents (cr, utf8, &extents);
	x = 128.0-(extents.width/2 + extents.x_bearing);
	y = 128.0-(extents.height/2 + extents.y_bearing);

	cairo_move_to (cr, x, y);
	cairo_show_text (cr, utf8);

	/* draw helping lines */
	cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
	cairo_set_line_width (cr, 6.0);
	cairo_arc (cr, x, y, 10.0, 0, 2*M_PI);
	cairo_fill (cr);
	cairo_move_to (cr, 128.0, 0);
	cairo_rel_line_to (cr, 0, 256);
	cairo_move_to (cr, 0, 128.0);
	cairo_rel_line_to (cr, 256, 0);
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
