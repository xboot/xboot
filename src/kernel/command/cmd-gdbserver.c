/*
 * kernel/command/cmd-gdbserver.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <uart/uart.h>
#include <xboot/gdbstub.h>
#include <command/command.h>

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    gdbserver <device>\r\n");
	printf("    gdbserver --kill\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_UART], head)
	{
		printf("    %s\r\n", pos->name);
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
	.desc	= "remote server for gnu debugger",
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
