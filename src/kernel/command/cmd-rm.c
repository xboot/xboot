/*
 * kernel/command/cmd-rm.c
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

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    rm [OPTION] FILE...\r\n");
}

static int do_rm(int argc, char ** argv)
{
	s32_t i;
	s32_t ret;
	struct stat st;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
	    if(stat((const char*)argv[i], &st) == 0)
	    {
	        if(S_ISDIR(st.st_mode))
	            ret = rmdir((const char*)argv[i]);
	        else
	            ret = unlink((const char*)argv[i]);
			if(ret != 0)
				printf("rm: cannot remove %s: No such file or directory\r\n", argv[i]);
	    }
	    else
	    	printf("rm: cannot stat file or directory %s\r\n", argv[i]);
	}

	return 0;
}

static struct command_t cmd_rm = {
	.name	= "rm",
	.desc	= "remove files or directories",
	.usage	= usage,
	.exec	= do_rm,
};

static __init void rm_cmd_init(void)
{
	register_command(&cmd_rm);
}

static __exit void rm_cmd_exit(void)
{
	unregister_command(&cmd_rm);
}

command_initcall(rm_cmd_init);
command_exitcall(rm_cmd_exit);
