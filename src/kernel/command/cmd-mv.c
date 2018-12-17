/*
 * kernel/command/cmd-mv.c
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
	printf("    mv SOURCE DEST\r\n");
}

static int do_mv(int argc, char ** argv)
{
	char path[VFS_MAX_PATH];
	char * src, * dest, * p;
	struct vfs_stat_t st1, st2;
	int rc;

	src = argv[1];
	dest = argv[2];

	if(argc != 3)
	{
		usage();
		return -1;
	}

	if(vfs_stat(src, &st1) != 0)
	{
		printf("mv: cannot access %s: No such file or directory\r\n", src);
		return -1;
	}

	if(!S_ISREG(st1.st_mode))
	{
		printf("mv: invalid file type\r\n");
		return -1;
	}

	rc = vfs_stat(dest, &st2);
	if(!rc && S_ISDIR(st2.st_mode))
	{
		p = strrchr(src, '/');
		p = p ? p + 1 : src;
		strlcpy(path, dest, sizeof(path));
		if(strcmp(dest, "/"))
			strlcat(path, "/", sizeof(path));
		strlcat(path, p, sizeof(path));
		dest = path;
	}

    if(vfs_rename(src, dest) != 0)
    {
    	printf("mv: failed to move file %s to %s\r\n", src, dest);
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
