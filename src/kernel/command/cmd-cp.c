/*
 * kernel/command/cmd-cp.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
	printf("    cp [-v] <SOURCE> <DIRECTORY>\r\n");
}

static int make_directory(const char * dir)
{
	struct vfs_stat_t st;
	char * s = strdup(dir);
	char * path = s;
	char c;
	int ret = 0;

	do {
		c = 0;
		while(*s)
		{
			if(*s == '/')
			{
				do {
					++s;
				} while(*s == '/');
				c = *s;
				*s = 0;
				break;
			}
			++s;
		}
		if(vfs_stat(path, &st) < 0)
		{
			if(vfs_mkdir(path, 0755) < 0)
			{
				ret = -1;
				break;
			}
		}
		if(!c)
			goto out;
		*s = c;
	} while(1);
out:
	free(path);
	return ret;
}

static int copy_file(const char * src, const char * dst, int verbose)
{
	struct vfs_stat_t st;
	char * buf;
	int sfd, dfd;
	int flags;
	u64_t n;

	sfd = vfs_open(src, O_RDONLY, 0);
	if(sfd < 0)
	{
		printf("could not open %s\r\n", src);
		return -1;
	}

	flags = O_WRONLY | O_CREAT;
	if((vfs_stat(dst, &st) >= 0) && S_ISREG(st.st_mode))
	{
		flags |= O_TRUNC;
	}
	dfd = vfs_open(dst, flags, 0755);
	if(dfd < 0)
	{
		printf("could not open %s\r\n", dst);
		vfs_close(sfd);
		return -1;
	}

	if(!(buf = malloc(SZ_64K)))
	{
		vfs_close(sfd);
		vfs_close(dfd);
		return -1;
	}

	if(verbose)
		printf("'%s' -> '%s'\r\n", src, dst);
	while((n = vfs_read(sfd, buf, SZ_64K)) > 0)
	{
		vfs_write(dfd, buf, n);
	}

	free(buf);
	vfs_close(sfd);
	vfs_close(dfd);

	return 0;
}

static int copy_recursive(const char * src, const char * dst, int verbose)
{
	struct vfs_stat_t st;
	struct vfs_dirent_t dir;
	char * from, * to;
	int fd;
	int ret;

	ret = vfs_stat(src, &st);
	if(ret)
		return ret;

	if(!S_ISDIR(st.st_mode))
		return copy_file(src, dst, verbose);

	ret = make_directory(dst);
	if(ret)
		return ret;

	fd = vfs_opendir(src);
	if(!fd)
		return -1;

	while(vfs_readdir(fd, &dir) >= 0)
	{
		if(!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."))
			continue;
		from = malloc(strlen(src) + strlen(dir.d_name) + 2);
		to = malloc(strlen(dst) + strlen(dir.d_name) + 2);
		if(from && to)
		{
			sprintf(from, "%s/%s", src, dir.d_name);
			sprintf(to, "%s/%s", dst, dir.d_name);
			ret = copy_recursive(from, to, verbose);
			if(ret)
				break;
		}
		if(from)
			free(from);
		if(to)
			free(to);
	}
	vfs_closedir(fd);

	return ret;
}

static int do_cp(int argc, char ** argv)
{
	struct vfs_stat_t st;
	char spath[VFS_MAX_PATH];
	char dpath[VFS_MAX_PATH];
	char tpath[VFS_MAX_PATH];
	char ** v;
	int exist = 0;
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
	if(c < 2)
	{
		usage();
		free(v);
		return -1;
	}
	if((shell_realpath(v[c - 1], dpath) >= 0) && vfs_stat(dpath, &st) >= 0)
	{
		if(S_ISDIR(st.st_mode))
		{
			exist = 1;
		}
		else
		{
			usage();
			free(v);
			return -1;
		}
	}
	if(!exist && make_directory(dpath))
	{
		usage();
		free(v);
		return -1;
	}
	for(i = 0; i < c - 1; i++)
	{
		if(shell_realpath(v[i], spath) >= 0)
		{
			sprintf(tpath, "%s/%s", dpath, basename(v[i]));
			copy_recursive(spath, tpath, verbose);
		}
	}
	free(v);

	return 0;
}

static struct command_t cmd_cp = {
	.name	= "cp",
	.desc	= "copy files and directories",
	.usage	= usage,
	.exec	= do_cp,
};

static __init void cp_cmd_init(void)
{
	register_command(&cmd_cp);
}

static __exit void cp_cmd_exit(void)
{
	unregister_command(&cmd_cp);
}

command_initcall(cp_cmd_init);
command_exitcall(cp_cmd_exit);
