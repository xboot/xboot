/*
 * kernel/command/cmd-tree.c
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

static void usage(void)
{
	printf("usage:\r\n");
	printf("    tree <directory> ...\r\n");
}

static void showtree(const char * path, int depth)
{
	struct vfs_stat_t st;
	struct vfs_dirent_t dir;
	char * buf;
	int fd, len;
	int i, l;

	if(((len = strlen(path)) > 0) && ((fd = vfs_opendir(path)) >= 0))
	{
		if(depth == 0)
			printf("%s\r\n", path);
		while(vfs_readdir(fd, &dir) >= 0)
		{
			if(!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."))
				continue;
			l = len + strlen(dir.d_name) + 2;
			if((buf = malloc(l)))
			{
				snprintf(buf, l, "%s%s%s", path, (path[len - 1] == '/') ? "" : "/", dir.d_name);
				if(!vfs_stat(buf, &st))
				{
					for(i = 0; i < depth; i++)
						printf("|   ");
					printf("|-- %s\r\n", dir.d_name);
					if(S_ISDIR(st.st_mode))
						showtree(buf, depth + 1);
				}
				free(buf);
			}
		}
		vfs_closedir(fd);
	}
}

static int do_tree(int argc, char ** argv)
{
	struct vfs_stat_t st;
	char fpath[VFS_MAX_PATH];
	int i;

	if(argc == 1)
	{
		usage();
		return -1;
	}
	for(i = 1; i < argc; i++)
	{
		if(shell_realpath(argv[i], fpath) < 0)
			continue;
		if((vfs_stat(fpath, &st) >= 0) && S_ISDIR(st.st_mode))
			showtree(fpath, 0);
	}
	return 0;
}

static struct command_t cmd_tree = {
	.name	= "tree",
	.desc	= "list contents of directories in a tree-like format",
	.usage	= usage,
	.exec	= do_tree,
};

static __init void tree_cmd_init(void)
{
	register_command(&cmd_tree);
}

static __exit void tree_cmd_exit(void)
{
	unregister_command(&cmd_tree);
}

command_initcall(tree_cmd_init);
command_exitcall(tree_cmd_exit);
