/*
 * kernel/xfs/archiver-dir.c
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

#include <xfs/archiver.h>

struct mhandle_dir_t {
	char * path;
};

struct fhandle_dir_t {
	int fd;
};

static char * concat(const char * str, ...)
{
	va_list args;
	const char *s;
	int len = strlen(str);
	va_start(args, str);
	while((s = va_arg(args, char *)))
	{
		len += strlen(s);
	}
	va_end(args);
	char * res = malloc(len + 1);
	if(!res)
		return NULL;
	strcpy(res, str);
	va_start(args, str);
	while((s = va_arg(args, char *)))
	{
		strcat(res, s);
	}
	va_end(args);
	return res;
}

static void * dir_mount(const char * path, int * writable)
{
	struct mhandle_dir_t * m;
	struct stat st;

	if((stat(path, &st) != 0) || !S_ISDIR(st.st_mode))
		return NULL;
	m = malloc(sizeof(struct mhandle_dir_t));
	if(!m)
		return NULL;
	m->path = strdup(path);
	if(writable)
		*writable = (access(path, W_OK) == 0) ? 1 : 0;
	return m;
}

static void dir_umount(void * m)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;

	if(mh)
	{
		free(mh->path);
		free(mh);
	}
}

static void dir_walk(void * m, const char * name, xfs_walk_callback_t cb, void * data)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	struct dirent_t * entry;
	char * path = concat(mh->path, "/", name, NULL);
	void * dir;

	if((dir = opendir(path)) == NULL)
	{
		free(path);
		return;
	}
	while((entry = readdir(dir)) != NULL)
	{
		if(strcmp(entry->d_name, ".") == 0)
			continue;
		else if(strcmp(entry->d_name, "..") == 0)
			continue;
		cb(name, entry->d_name, data);
	}
	closedir(dir);
	free(path);
}

static bool_t dir_isdir(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	struct stat st;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = FALSE;

	if((stat(path, &st) == 0) && S_ISDIR(st.st_mode))
		ret = TRUE;
	free(path);
	return ret;
}

static bool_t dir_isfile(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	struct stat st;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = FALSE;

	if((stat(path, &st) == 0) && S_ISREG(st.st_mode))
		ret = TRUE;
	free(path);
	return ret;
}

static bool_t dir_mkdir(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = FALSE;

	if(mkdir(path, (S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) == 0)
		ret = TRUE;
	free(path);
	return ret;
}

static bool_t dir_remove(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	struct stat st;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = FALSE;

	if(stat(path, &st) == 0)
	{
		if(S_ISDIR(st.st_mode))
			ret = (rmdir(path) == 0) ? TRUE : FALSE;
		else if(S_ISREG(st.st_mode))
			ret = (unlink(path) == 0) ? TRUE : FALSE;
	}
	free(path);
	return ret;
}

static void * dir_open(void * m, const char * name, int mode)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	struct fhandle_dir_t * fh;
	char * path = concat(mh->path, "/", name, NULL);
	int fd, flags;

	switch(mode)
	{
	case XFS_OPEN_MODE_READ:
		flags = O_RDONLY;
		break;
	case XFS_OPEN_MODE_WRITE:
		flags = O_WRONLY | O_CREAT | O_TRUNC;
		break;
	case XFS_OPEN_MODE_APPEND:
		flags = O_WRONLY | O_CREAT | O_APPEND;
		break;
	default:
		flags = O_RDONLY;
		break;
	}
	fd = open(path, flags, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
	{
		free(path);
		return NULL;
	}

	fh = malloc(sizeof(struct fhandle_dir_t));
	if(!fh)
	{
		close(fd);
		free(path);
		return NULL;
	}
	fh->fd = fd;
	free(path);
	return ((void *)fh);
}

static s64_t dir_read(void * f, void * buf, s64_t size)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	return read(fh->fd, buf, size);
}

static s64_t dir_write(void * f, void * buf, s64_t size)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	return write(fh->fd, buf, size);
}

static s64_t dir_seek(void * f, s64_t offset)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	s64_t pos;
	pos = lseek(fh->fd, offset, SEEK_SET);
	return (pos >= 0) ? pos : 0;
}

static s64_t dir_length(void * f)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	struct stat st;
	if(fstat(fh->fd, &st) == 0)
		return st.st_size;
	return 0;
}

static void dir_close(void * f)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	close(fh->fd);
	free(fh);
}

static struct xfs_archiver_t archiver_dir = {
	.name		= "",
	.mount		= dir_mount,
	.umount 	= dir_umount,
	.walk		= dir_walk,
	.isdir		= dir_isdir,
	.isfile		= dir_isfile,
	.mkdir		= dir_mkdir,
	.remove		= dir_remove,
	.open		= dir_open,
	.read		= dir_read,
	.write		= dir_write,
	.seek		= dir_seek,
	.length		= dir_length,
	.close		= dir_close,
};

static __init void archiver_dir_init(void)
{
	register_archiver(&archiver_dir);
}

static __exit void archiver_dir_exit(void)
{
	unregister_archiver(&archiver_dir);
}

core_initcall(archiver_dir_init);
core_exitcall(archiver_dir_exit);
