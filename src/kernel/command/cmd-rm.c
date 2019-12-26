/*
 * kernel/command/cmd-rm.c
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
	printf("    rm [-v] <DIRECTORY>...\r\n");
}

int remove_tree(const char * path, int verbose)
{
	struct vfs_stat_t st;
	struct vfs_dirent_t dir;
	char * buf;
	int fd, plen, len;
	int r = -1, r2 = -1;

	if((fd = vfs_opendir(path)) >= 0)
	{
		plen = strlen(path);
		r = 0;
		while(!r && (vfs_readdir(fd, &dir) >= 0))
		{
			if(!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."))
				continue;
			len = plen + strlen(dir.d_name) + 2;
			if((buf = malloc(len)))
			{
				snprintf(buf, len, "%s/%s", path, dir.d_name);
				if(!vfs_stat(buf, &st))
				{
					if(S_ISDIR(st.st_mode))
					{
						r2 = remove_tree(buf, verbose);
					}
					else
					{
						r2 = vfs_unlink(buf);
						if(r2 < 0)
							printf("cannot remove: '%s'\r\n", buf);
						else if(verbose)
							printf("removed: '%s'\r\n", buf);
					}
				}
				free(buf);
			}
			r = r2;
		}
		vfs_closedir(fd);
	}
	if(!r)
	{
		r = vfs_rmdir(path);
		if(r < 0)
			printf("cannot remove directory: '%s'\r\n", path);
		else if(verbose)
			printf("removed directory: '%s'\r\n", path);
	}
	return r;
}

static int do_rm(int argc, char ** argv)
{
	struct vfs_stat_t st;
	char fpath[VFS_MAX_PATH];
	char ** v;
	int verbose = 0;
	int c = 0;
	int i;

	if(!(v = malloc(sizeof(char *) * argc)))
		return -1;

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-v") == 0)
			verbose = 1;
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
		if(shell_realpath(v[i], fpath) < 0)
			continue;
		if(vfs_stat(fpath, &st) >= 0)
		{
			if(S_ISDIR(st.st_mode))
			{
				remove_tree(fpath, verbose);
			}
			else
			{
				if(vfs_unlink(fpath) < 0)
					printf("cannot remove: '%s'\r\n", fpath);
				else if(verbose)
					printf("removed: '%s'\r\n", fpath);
			}
		}
	}
	free(v);

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
