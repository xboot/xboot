/*
 * kernel/command/cmd-mv.c
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
	printf("    mv <SRC> <DST>\r\n");
}

static int do_mv(int argc, char ** argv)
{
	struct vfs_stat_t st1, st2;
	char spath[VFS_MAX_PATH];
	char dpath[VFS_MAX_PATH];
	char path[VFS_MAX_PATH];
	char * p;

	if(argc != 3)
	{
		usage();
		return -1;
	}

	if(shell_realpath(argv[1], spath) < 0)
	{
		printf("mv: %s: Can not convert to realpath\r\n", argv[1]);
		return -1;
	}

	if(shell_realpath(argv[2], dpath) < 0)
	{
		printf("mv: %s: Can not convert to realpath\r\n", argv[2]);
		return -1;
	}

	if(vfs_stat(spath, &st1) != 0)
	{
		printf("mv: cannot access %s: No such file or directory\r\n", spath);
		return -1;
	}

	if(!S_ISREG(st1.st_mode) && !S_ISDIR(st1.st_mode))
	{
		printf("mv: invalid type\r\n");
		return -1;
	}

	if(!vfs_stat(dpath, &st2) && S_ISDIR(st2.st_mode))
	{
		p = strrchr(spath, '/');
		p = p ? p + 1 : spath;
		strlcpy(path, dpath, sizeof(path));
		if(strcmp(dpath, "/"))
			strlcat(path, "/", sizeof(path));
		strlcat(path, p, sizeof(path));
		strlcpy(dpath, path, sizeof(path));
	}

    if(vfs_rename(spath, dpath) != 0)
    {
    	printf("mv: failed to move file or directory %s to %s\r\n", spath, dpath);
    	return -1;
    }
	return 0;
}

static struct command_t cmd_mv = {
	.name	= "mv",
	.desc	= "move (rename) files",
	.usage	= usage,
	.exec	= do_mv,
};

static __init void mv_cmd_init(void)
{
	register_command(&cmd_mv);
}

static __exit void mv_cmd_exit(void)
{
	unregister_command(&cmd_mv);
}

command_initcall(mv_cmd_init);
command_exitcall(mv_cmd_exit);
