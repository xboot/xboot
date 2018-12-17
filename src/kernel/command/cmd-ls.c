/*
 * kernel/command/cmd-ls.c
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

enum {
	LSFLAG_DOT	= (1 << 0),
	LSFLAG_LONG	= (1 << 1),
};
static const char rwx[8][4] = { "---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx" };

static void ls(const char * path, int flag)
{
	struct vfs_stat_t st;
	struct vfs_dirent_t dir;
	struct slist_t * sl, * e;
	char buf[VFS_MAX_PATH];
	int isdir;
	int fd;

	if(vfs_stat(path, &st) < 0)
	{
		printf("ls: cannot access %s: No such file or directory\r\n", path);
		return;
	}

	sl = slist_alloc();
	if(S_ISDIR(st.st_mode))
	{
		if((fd = vfs_opendir(path)) >= 0)
		{
			while(vfs_readdir(fd, &dir) >= 0)
			{
				if(!(flag & LSFLAG_DOT) && dir.d_name && (dir.d_name[0] == '.'))
					continue;
				slist_add(sl, NULL, "%s", dir.d_name);
			}
			vfs_closedir(fd);
		}
		isdir = 1;
	}
	else
	{
		slist_add(sl, NULL, "%s", path);
		isdir = 0;
	}
	slist_sort(sl);

	if(flag & LSFLAG_LONG)
	{
		slist_for_each_entry(e, sl)
		{
			if(isdir)
				snprintf(buf, sizeof(buf), "%s/%s", path, e->key);
			else
				snprintf(buf, sizeof(buf), "%s", e->key);
			if(vfs_stat(buf, &st) < 0)
				continue;

		    if(S_ISDIR(st.st_mode))
		    	printf("d");
		    if(S_ISCHR(st.st_mode))
		    	printf("c");
		    if(S_ISBLK(st.st_mode))
		    	printf("b");
		    if(S_ISREG(st.st_mode))
		    	printf("-");
		    if(S_ISLNK(st.st_mode))
		    	printf("l");
		    if(S_ISFIFO(st.st_mode))
		    	printf("p");
		    if(S_ISSOCK(st.st_mode))
		    	printf("s");

		    printf("%s%s%s", rwx[(st.st_mode & S_IRWXU) >> 6], rwx[(st.st_mode & S_IRWXG) >> 3], rwx[(st.st_mode & S_IRWXO) >> 0]);
			if((st.st_uid == 0) && (st.st_gid == 0))
				printf(" %s %s", "root", "root");
			else
				printf(" %4d %4d", st.st_uid, st.st_gid);
			printf(" %8lld %s\r\n", st.st_size, e->key);
		}
	}
	else
	{
		int len = 0, n, i, l;

		slist_for_each_entry(e, sl)
		{
			l = strlen(e->key) + 4;
			if(l > len)
				len = l;
		}

		if(!len)
			return;

		n = 80 / (len + 1);
		if(n == 0)
			n = 1;

		i = 0;
		slist_for_each_entry(e, sl)
		{
			if(!(++i % n))
				printf("%s\r\n", e->key);
			else
				printf("%-*s", len, e->key);
		}
		if(i % n)
			printf("\r\n");
	}
	slist_free(sl);
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    ls [-l] [-a] [FILE]...\r\n");
}

static int do_ls(int argc, char ** argv)
{
	char fpath[VFS_MAX_PATH];
	char ** v;
	int flag = 0;
	int c = 0;
	int i;

	if(!(v = malloc(sizeof(char *) * argc)))
		return -1;

	for(i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-l") == 0)
			flag |= LSFLAG_LONG;
		else if(strcmp(argv[i], "-a") == 0)
			flag |= LSFLAG_DOT;
		else
			v[c++] = argv[i];
	}

	if(c == 0)
		v[c++] = ".";

	for(i = 0; i < c; i++)
	{
		if(shell_realpath(v[i], fpath) >= 0)
		{
			ls(fpath, flag);
		}
	}
	free(v);

	return 0;
}

static struct command_t cmd_ls = {
	.name	= "ls",
	.desc	= "list directory contents",
	.usage	= usage,
	.exec	= do_ls,
};

static __init void ls_cmd_init(void)
{
	register_command(&cmd_ls);
}

static __exit void ls_cmd_exit(void)
{
	unregister_command(&cmd_ls);
}

command_initcall(ls_cmd_init);
command_exitcall(ls_cmd_exit);
