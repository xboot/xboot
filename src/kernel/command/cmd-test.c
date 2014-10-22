/*
 * kernel/command/cmd-test.c
 */

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <fifo.h>
#include <byteorder.h>
#include <io.h>
#include <math.h>
#include <time/timer.h>
#include <time/tick.h>
#include <xboot/machine.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <xboot/module.h>
#include <shell/exec.h>
#include <fb/fb.h>
#include <rtc/rtc.h>
#include <input/input.h>
#include <console/console.h>
#include <block/loop.h>
#include <command/command.h>
#include <fs/vfs/vfs.h>
#include <fs/fileio.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>

#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)

static int test(int argc, char ** argv)
{
	return 0;
}

static struct command_t test_cmd = {
	.name		= "test",
	.func		= test,
	.desc		= "test command for debug\r\n",
	.usage		= "test [arg ...]\r\n",
	.help		= "    test command for debug software by programmer.\r\n"
};

static __init void test_cmd_init(void)
{
	if(command_register(&test_cmd))
		LOG("Register command 'test'");
	else
		LOG("Failed to register command 'test'");
}

static __exit void test_cmd_exit(void)
{
	if(command_unregister(&test_cmd))
		LOG("Unegister command 'test'");
	else
		LOG("Failed to unregister command 'test'");
}

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);

#endif
