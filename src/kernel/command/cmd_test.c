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
#include <input/input.h>
#include <input/keyboard/keyboard.h>
#include <console/console.h>
#include <console/curses.h>
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
	struct tui_workspace * ws;
	struct tui_button * btn;

	ws = tui_workspace_new(get_stdout(), (x_s8 *)"ws");
	btn = tui_button_new((struct tui_widget *)ws, (const x_s8 *)"btn", (const x_s8 *)"ok");

	tui_widget_setbounds(btn, 2,2,10,10);
	tui_widget_paint((struct tui_widget *)ws, 0, 0, 40, 20);
	tui_widget_destory((struct tui_widget *)ws);

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
