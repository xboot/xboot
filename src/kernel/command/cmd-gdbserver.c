/*
 * kernel/command/cmd-gdbserver.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <uart/uart.h>
#include <xboot/gdbstub.h>
#include <command/command.h>

static void usage(void)
{
	struct device_list_t * pos, * n;

	printf("usage:\r\n");
	printf("    gdbserver <device>\r\n");
	printf("    gdbserver --kill\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &(__device_list.entry), entry)
	{
		if(pos->device->type == DEVICE_TYPE_UART)
			printf("    %s\r\n", pos->device->name);
	}
}

static int do_gdbserver(int argc, char ** argv)
{
	int s;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	if(strcmp(argv[1], "--kill") == 0)
	{
		gdbserver_stop();
		return 0;
	}

	printf("Start gdbserver on device '%s'\r\n", argv[1]);
	if((s = gdbserver_start(argv[1])) < 0)
	{
		if(s == -1)
		{
			printf("This machine don't support gdbserver\r\n", argv[1]);
			return -1;
		}
		else if(s == -2)
		{
			printf("Can not start gdbserver on device '%s'\r\n", argv[1]);
			usage();
			return -1;
		}
	}

	return 0;
}

static struct command_t cmd_gdbserver = {
	.name	= "gdbserver",
	.desc	= "remote server for the gnu debugger",
	.usage	= usage,
	.exec	= do_gdbserver,
};

static __init void gdbserver_cmd_init(void)
{
	register_command(&cmd_gdbserver);
}

static __exit void gdbserver_cmd_exit(void)
{
	unregister_command(&cmd_gdbserver);
}

command_initcall(gdbserver_cmd_init);
command_exitcall(gdbserver_cmd_exit);
