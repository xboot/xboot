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

EXPORT_SYMBOL(search_framebuffer);
EXPORT_SYMBOL(__get_runtime);

static int test(int argc, char ** argv)
{
	extern void test_elf(char * path);
	if(argc >= 2)
	{
		test_elf(argv[1]);
	}
	else
	{
		void (*p)(void);

		p = __symbol_get("module_main");
		if(p)
		{
			p();
		}
		else
		{
			printk("not found module_main\r\n");
		}
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
