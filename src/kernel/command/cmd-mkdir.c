/*
 * kernel/command/cmd-mkdir.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
	printf("    mkdir [-p] DIRECTORY...\r\n");
}

static int build(char * path)
{
	struct vfs_stat_t st;
	char fpath[VFS_MAX_PATH];
	char * p;

	p = path;
	if(p[0] == '/')
		++p;

	for(;; ++p)
	{
		if(p[0] == '\0' || (p[0] == '/' && p[1] == '\0'))
			break;
		if(p[0] != '/')
			continue;
		*p = '\0';
		if(shell_realpath(path, fpath) < 0)
			return -1;
		if(vfs_stat(fpath, &st) < 0)
		{
			if(vfs_mkdir(fpath, 0755) < 0)
				return -1;
		}
		*p = '/';
	}
	return 0;
}

static int do_mkdir(int argc, char ** argv)
{
	char fpath[VFS_MAX_PATH];
	char ** v;
	int pflag = 0;
	int c = 0;
	int ret = 0;
	int i;

	if(!(v = malloc(sizeof(char *) * argc)))
		return -1;

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-p") )
			pflag = 1;
		else
			v[c++] = argv[i];
	}

	if(c == 0)
	{
		usage();
		free(v);
		return -1;
	}

	for(i = 0; i < c; i++)
	{
		if(pflag && build(v[i]))
		{
			ret = -1;
			continue;
		}
		if(shell_realpath(v[i], fpath) < 0)
		{
			ret = -1;
			continue;
		}
		if(vfs_mkdir(fpath, 0755) < 0)
		{
			ret = -1;
			printf("mkdir: failed to create directory %s\r\n", fpath);
		}
	}
	free(v);

	return ret;
}

static struct command_t cmd_mkdir = {
	.name	= "mkdir",
	.desc	= "make directories",
	.usage	= usage,
	.exec	= do_mkdir,
};

static __init void mkdir_cmd_init(void)
{
	register_command(&cmd_mkdir);
}

static __exit void mkdir_cmd_exit(void)
{
	unregister_command(&cmd_mkdir);
}

command_initcall(mkdir_cmd_init);
command_exitcall(mkdir_cmd_exit);
