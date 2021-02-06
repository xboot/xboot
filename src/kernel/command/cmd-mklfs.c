/*
 * kernel/command/cmd-mklfs.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

extern int lfs_format_block(struct block_t * blk);

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    mklfs <block device>\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_BLOCK], head)
	{
		printf("    %s\r\n", pos->name);
	}
}

static int do_mklfs(int argc, char ** argv)
{
	struct block_t * blk;

	if(argc != 2)
	{
		usage();
		return -1;
	}
	blk = search_block(argv[1]);
	if(!blk)
	{
		printf("Can't find block device '%s'\r\n", argv[1]);
		return -1;
	}
	if(!lfs_format_block(blk))
	{
		printf("Format failed.\r\n");
		return -1;
	}
	printf("Format complete.\r\n");

	return 0;
}

static struct command_t cmd_mklfs = {
	.name	= "mklfs",
	.desc	= "format lfs filesystem on block device",
	.usage	= usage,
	.exec	= do_mklfs,
};

static __init void mklfs_cmd_init(void)
{
	register_command(&cmd_mklfs);
}

static __exit void mklfs_cmd_exit(void)
{
	unregister_command(&cmd_mklfs);
}

command_initcall(mklfs_cmd_init);
command_exitcall(mklfs_cmd_exit);
