/*
 * xboot/kernel/command/cmd_test.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <rand.h>
#include <div64.h>
#include <fifo.h>
#include <vsprintf.h>
#include <byteorder.h>
#include <sha.h>
#include <xml.h>
#include <io.h>
#include <time/delay.h>
#include <time/timer.h>
#include <time/xtime.h>
#include <time/tick.h>
#include <xboot/list.h>
#include <xboot/log.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/scank.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <xboot/chrdev.h>
#include <shell/env.h>
#include <shell/command.h>
#include <fb/fb.h>
#include <loop/loop.h>
#include <fb/graphic.h>
#include <fb/fbpixel.h>
#include <fb/fbscale.h>
#include <fb/font.h>
#include <fb/logo.h>
#include <rtc/rtc.h>
#include <xboot/proc.h>
#include <terminal/terminal.h>
#include <terminal/curses.h>
#include <fs/vfs/vfs.h>
#include <fs/fsapi.h>
#include <gui/color.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>


#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)

static x_s32 test(x_s32 argc, const x_s8 **argv)
{
	struct fb * fb;
	struct font_glyph * glyph;
	struct color * c;
	x_u32 color;
	x_s32 i;

	fb = search_framebuffer("fb");
	if(!fb)
	{
		printk("no found framebuffer device\r\n");
		return -1;
	}

/*
	glyph = malloc(sizeof(struct font_glyph));
	if(!glyph)
	{
		printk("malloc glyph fail\r\n");
		return -1;
	}

	c = get_color_by_name("red");
	color = fb_map_color(fb, c->r, c->g, c->b, c->a);
	fb_fill_rect(fb, color, 0, 0, 100, 100);
*/

	c = get_color_by_name("mediumturquoise");
	color = fb_map_color(fb, c->r, c->g, c->b, c->a);
	fb_draw_text(fb, "abcdefghLLLasdf阿哦俄", NULL, color, 100-4, 100-8);


/*
	c = get_color_by_name("peru");
	color = fb_map_color(fb, c->r, c->g, c->b, c->a);

	for(i=0; i< 800; i++)
	{
		bitmap_set_pixel(&(fb->info->bitmap), i, 100, color);
		bitmap_set_pixel(&(fb->info->bitmap), i, 300, color);
	}
	for(i=0; i< 480; i++)
	{
		bitmap_set_pixel(&(fb->info->bitmap), 100, i, color);
		bitmap_set_pixel(&(fb->info->bitmap), 300, i, color);
	}

	struct rect rect, to;
	rect_set(&rect, 100,100, 300, 300);

	//rect_set(&to, 100+20,100+20, 100+50, 100+80);

	rect_set(&to, 20,20, 50+300, 80);

	rect_align(&rect, &to, ALIGN_LEFT_TOP);

	c = get_color_by_name("yellow");
	color = fb_map_color(fb, c->r, c->g, c->b, c->a);
	fb_fill_rect(fb, color, to.left, to.top, to.right- to.left, to.bottom - to.top);
*/

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

module_init(test_cmd_init, LEVEL_COMMAND);
module_exit(test_cmd_exit, LEVEL_COMMAND);

#endif
