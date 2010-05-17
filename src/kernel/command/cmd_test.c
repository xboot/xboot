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
#include <fb/logo.h>
#include <rtc/rtc.h>
#include <xboot/proc.h>
#include <terminal/terminal.h>
#include <terminal/curses.h>
#include <fs/vfs/vfs.h>
#include <fs/fsapi.h>
#include <mmc/mmc_host.h>
#include <mmc/mmc_card.h>


#if	defined(CONFIG_COMMAND_TEST) && (CONFIG_COMMAND_TEST > 0)

static x_s32 test(x_s32 argc, const x_s8 **argv)
{
	struct mmc_card * card;
	x_u8 buf[512];
	x_s32 i;

	if(argc > 1)
	{
		printk("write mmc\r\n");

		for(i=0; i<512; i++)
		{
			buf[i] = i;
		}

		card = search_mmc_card("mmc0");
		if(!card)
		{
			printk("not found mmc0\r\n");
			return -1;
		}

		if(!card->host->write_sector(card, 32, buf))
		{
			printk("write fail\r\n");
			return -1;
		}
		printk("write successed\r\n");
	}
	else
	{
		printk("read mmc\r\n");

		memset(buf, 0, sizeof(buf));

		card = search_mmc_card("mmc0");
		if(!card)
		{
			printk("not found mmc0\r\n");
			return -1;
		}

		if(!card->host->read_sector(card, 32, buf))
		{
			printk("read fail\r\n");
			return -1;
		}

		for(i=0; i<256; i++)
		{
			if((i % 16) == 0)
				printk("\r\n");
			printk("%02x ",buf[i]);
		}
		printk("\r\n");
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
