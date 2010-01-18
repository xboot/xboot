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
#include <time/delay.h>
#include <time/timer.h>
#include <time/xtime.h>
#include <time/tick.h>
#include <xboot/list.h>
#include <xboot/io.h>
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
#include <fb/logo.h>
#include <rtc/rtc.h>
#include <xboot/proc.h>
#include <terminal/terminal.h>
#include <terminal/curses.h>
#include <mmc/mmc.h>
#include <mmc/mmc_host.h>
#include <fs/vfs/vfs.h>
#include <fs/fsapi.h>


#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)

static x_s32 test(x_s32 argc, const x_s8 **argv)
{
	x_s32 i = 0;

	if(argc != 3)
	{
		printk("!= 3\r\n");
		return -1;
	}

	i = simple_strtou32(argv[1], NULL, 0);

	if(i == 0)
	{
		printk("register_loop\r\n");
		if(register_loop((char *)argv[2]))
			printk("ok\r\n");
		else
			printk("false\r\n");
	}
	else if(i == 1)
	{
		printk("uregister_loop\r\n");
		if(unregister_loop((char *)argv[2]))
			printk("u ok\r\n");
		else
			printk("u false\r\n");
	}
	else
	{
		printk("i=%d\r\n", i);
		return -1;
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
