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
#include <tui/tui.h>
#include <tui/widget/workspace.h>
#include <tui/widget/button.h>


#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)

static x_s32 test(x_s32 argc, const x_s8 **argv)
{
//	console_stdio_save("/etc/console.xml");

#if 0

	struct tui_workspace * ws;
	struct tui_button * btn1, * btn2, * btn3;
	x_s32 i;

/*	for(i=0; i<256; i++)
	{
		console_setcolor(get_stdout(), TCOLOR_WHITE, i);
		putcode('A');
	}*/
//	return 0;

//	console_setcursor(get_stdout(), FALSE);

	ws = tui_workspace_new(get_stdout(), (x_s8 *)"ws");
	btn1 = tui_button_new((struct tui_widget *)ws, (const x_s8 *)"btn1", (const x_s8 *)"btn1");
	btn2 = tui_button_new((struct tui_widget *)ws, (const x_s8 *)"btn2", (const x_s8 *)"btn2");
	btn3 = tui_button_new((struct tui_widget *)ws, (const x_s8 *)"btn3", (const x_s8 *)"btn3");

	tui_widget_setbounds((struct tui_widget *)btn1, 1,1,8,3);
	tui_widget_setbounds((struct tui_widget *)btn2, 10,10,8,3);
	tui_widget_setbounds((struct tui_widget *)btn2, 18,13,8,3);

	tui_widget_paint((struct tui_widget *)ws, 0, 0, 80, 25);

	for(i=1; i<10; i++)
	{
		tui_widget_setbounds((struct tui_widget *)btn1, 1+i,1,8+i,3+i);
		tui_widget_setbounds((struct tui_widget *)btn2, 10-i,10,8,3+i);
		tui_widget_setbounds((struct tui_widget *)btn3, 18-i,13+i,8+i,3);

		tui_widget_paint((struct tui_widget *)ws, 0, 0, 80, 25);

		mdelay(200);
	}

	tui_widget_destroy((struct tui_widget *)ws);

#endif
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
