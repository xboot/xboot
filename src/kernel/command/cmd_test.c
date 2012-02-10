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

static int test(int argc, char ** argv)
{
	FILE *f = tmpfile();
	char obuf[128], ibuf[sizeof obuf];
	char *line;
	size_t linesz;

	if (!f)
	{
		printk("tmpfile\r\n");
		return 1;
	}

	if (fputs("line\n", f) == EOF)
	{
		printk("fputs");
		return 1;
	}

	memset(obuf, 'z', sizeof obuf);
	memset(ibuf, 'y', sizeof ibuf);

	if (fwrite(obuf, sizeof obuf, 1, f) != 1)
	{
		printk("fwrite");
		return 1;
	}

	rewind(f);

	line = NULL;
	linesz = 0;
/*	if (getline(&line, &linesz, f) != 5)
	{
		printk("getline");
		return 1;
	}
	if (strcmp(line, "line\n"))
	{
		puts("Lines differ.  Test FAILED!");
		return 1;
	}*/

	if (fread(ibuf, sizeof ibuf, 1, f) != 1)
	{
		printk("fread");
		return 1;
	}

	if (memcmp(ibuf, obuf, sizeof ibuf))
	{
		printk("Buffers differ.  Test FAILED!");
		return 1;
	}

	printk ("Test succeeded.");

	/*
	FILE * t;

	t = fopen("/romdisk/boot/readme", "r");
	fclose(t);

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
