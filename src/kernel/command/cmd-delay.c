/*
 * kernel/command/cmd-delay.c
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

#include <xboot.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    delay [millisecond]\r\n");
}

static int do_delay(int argc, char ** argv)
{
	u32_t ms = 1000;

	if(argc > 1)
		ms = strtoul(argv[1], NULL, 0);
	mdelay(ms);

	return 0;
}

static struct command_t cmd_delay = {
	.name	= "delay",
	.desc	= "delay for a specified time",
	.usage	= usage,
	.exec	= do_delay,
};

static __init void delay_cmd_init(void)
{
	register_command(&cmd_delay);
}

static __exit void delay_cmd_exit(void)
{
	unregister_command(&cmd_delay);
}

command_initcall(delay_cmd_init);
command_exitcall(delay_cmd_exit);
