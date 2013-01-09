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

static void mouse_callback(struct event_t * event, void * data)
{
	if(event)
	{
		printk("event->type = %d\r\n", event->type);
		printk("event->timestamp = %d\r\n", event->timestamp);
		printk("event->btndown = %d\r\n", event->e.mouse_raw.btndown);
		printk("event->btnup = %d\r\n", event->e.mouse_raw.btnup);
		printk("event->xrel = %d\r\n", event->e.mouse_raw.xrel);
		printk("event->yrel = %d\r\n", event->e.mouse_raw.yrel);
		printk("event->zrel = %d\r\n", event->e.mouse_raw.zrel);
	}
}

static int test(int argc, char ** argv)
{
	event_base_add_watcher(runtime_get()->__event_base, EVENT_TYPE_MOUSE_RAW, mouse_callback, 0);

	while(1)
	{
		event_dispatch();
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

command_initcall(test_cmd_init);
command_exitcall(test_cmd_exit);

#endif
