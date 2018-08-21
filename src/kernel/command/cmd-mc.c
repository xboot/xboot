/*
 * kernel/command/cmd-md.c
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

#include <shell/ctrlc.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mc <src> <dst> <size>\r\n");
}

static int do_mc(int argc, char ** argv)
{
	u32_t src, dst, size = 0;
	u32_t i;

	if(argc != 4)
	{
		usage();
		return -1;
	}

	src = strtoul((const char *)argv[1], NULL, 0);
	dst = strtoul((const char *)argv[2], NULL, 0);
	size = strtoul((const char *)argv[3], NULL, 0);

	for(i = 0; i < size; i++)
	{
		*((u8_t *)(dst+i)) = *((u8_t *)(src+i));

		if(ctrlc())
			return -1;
	}

	return 0;
}

static struct command_t cmd_mc = {
	.name	= "mc",
	.desc	= "memory copy",
	.usage	= usage,
	.exec	= do_mc,
};

static __init void mc_cmd_init(void)
{
	register_command(&cmd_mc);
}

static __exit void mc_cmd_exit(void)
{
	unregister_command(&cmd_mc);
}

command_initcall(mc_cmd_init);
command_exitcall(mc_cmd_exit);
