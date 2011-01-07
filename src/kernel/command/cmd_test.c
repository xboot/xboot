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
#include <color.h>
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
#include <command/command.h>
#include <fb/fb.h>
#include <loop/loop.h>
#include <fb/graphic.h>
#include <fb/fbpixel.h>
#include <fb/fbscale.h>
#include <fb/font.h>
#include <fb/logo.h>
#include <rtc/rtc.h>
#include <xboot/proc.h>
#include <input/input.h>
#include <input/keyboard/keyboard.h>
#include <console/console.h>
#include <fs/vfs/vfs.h>
#include <fs/fsapi.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>


#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)

static const x_u8 data[16][8] = {
	[0x0] = {
			0x40,
			0xA0,
			0xA0,
			0xA0,
			0x40,
			0x00,
			0x00,
			0x00,
	},

	[0x1] = {
			0x40,
			0xC0,
			0x40,
			0x40,
			0xE0,
			0x00,
			0x00,
			0x00,
	},

	[0x2] = {
			0xC0,
			0x20,
			0x40,
			0x80,
			0xE0,
			0x00,
			0x00,
			0x00,
	},

	[0x3] = {
			0xC0,
			0x20,
			0x40,
			0x20,
			0xC0,
			0x00,
			0x00,
			0x00,
	},

	[0x4] = {
			0xA0,
			0xA0,
			0xE0,
			0x20,
			0x20,
			0x00,
			0x00,
			0x00,
	},

	[0x5] = {
			0xE0,
			0x80,
			0xC0,
			0x20,
			0xC0,
			0x00,
			0x00,
			0x00,
	},

	[0x6] = {
			0x60,
			0x80,
			0xC0,
			0xA0,
			0x40,
			0x00,
			0x00,
			0x00,
	},

	[0x7] = {
			0xE0,
			0x20,
			0x40,
			0x40,
			0x40,
			0x00,
			0x00,
			0x00,
	},

	[0x8] = {
			0x40,
			0xA0,
			0x40,
			0xA0,
			0x40,
			0x00,
			0x00,
			0x00,
	},

	[0x9] = {
			0x40,
			0xA0,
			0x60,
			0x20,
			0xC0,
			0x00,
			0x00,
			0x00,
	},

	[0xa] = {
			0x40,
			0xA0,
			0xE0,
			0xA0,
			0xA0,
			0x00,
			0x00,
			0x00,
	},

	[0xb] = {
			0xC0,
			0xA0,
			0xC0,
			0xA0,
			0xC0,
		0x00,
		0x00,
		0x00,
	},

	[0xc] = {
			0x40,
			0xA0,
			0x80,
			0xA0,
			0x40,
		0x00,
		0x00,
		0x00,
	},

	[0xd] = {
			0xC0,
			0xA0,
			0xA0,
			0xA0,
			0xC0,
		0x00,
		0x00,
		0x00,
	},

	[0xe] = {
			0xE0,
			0x80,
			0xC0,
			0x80,
			0xE0,
		0x00,
		0x00,
		0x00,
	},

	[0xf] = {
			0xE0,
			0x80,
			0xC0,
			0x80,
			0x80,
		0x00,
		0x00,
		0x00,
	}
};

static void print(x_u32 v)
{
	int i;

	printk("\t0x%02x,", v);

	printk("\t/* ");
	for(i = 7; i >= 0; i--)
	{
		if( (v >> i) & 0x1 )
			printk("X");
		else
			printk("_");
	}
	printk(" */\r\n");
}

static x_s32 test(x_s32 argc, const x_s8 **argv)
{
	int i, j;

	for(j = 0; j< 16; j++)
	{
		printk("[0x%x] = {\r\n", j);
		for(i = 0; i < 8; i++)
		{
			if(i < 2)
				print(0);
			else
				print(data[j][i-2] >> 4);
		}
		printk("},\r\n\r\n");
	}

//	return 0;


	struct font * font;
	struct fb * fb;

	font = get_font("-FontForge-MingLiU-Book-R-Normal--6-60-75-75-M-30-ISO10646-1");
	if(!font)
	{
		printk("get font fail\r\n");
		return -1;
	}

	fb = search_framebuffer("fb");
	if(!fb)
	{
		printk("get fb fail\r\n");
		return -1;
	}

	char tt[257];

	fb_draw_text(fb, "0123456789", font, 0xff0, 0, 0);

	for(j = 0; j < 10; j++)
	{
		putcode(0x3c00 + j);
	}

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
