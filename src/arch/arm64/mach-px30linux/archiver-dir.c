/*
 * archiver-dir.c
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

#include <sandbox.h>
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

	if(!sandbox_file_isdir(path))
		return NULL;
	m = malloc(sizeof(struct mhandle_dir_t));
	if(!m)
		return NULL;
	m->path = strdup(path);
	if(writable)
		*writable = sandbox_file_access(path, "rw") ? 1 : 0;
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
	char * path = concat(mh->path, "/", name, NULL);
	sandbox_file_walk(path, cb, name, data);
	free(path);
}

static bool_t dir_isdir(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = sandbox_file_isdir(path) ? TRUE : FALSE;
	free(path);
	return ret;
}

static bool_t dir_isfile(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = sandbox_file_isfile(path) ? TRUE : FALSE;
	free(path);
	return ret;
}

static bool_t dir_mkdir(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = sandbox_file_mkdir(path) ? TRUE : FALSE;
	free(path);
	return ret;
}

static bool_t dir_remove(void * m, const char * name)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	char * path = concat(mh->path, "/", name, NULL);
	bool_t ret = sandbox_file_remove(path) ? TRUE : FALSE;
	free(path);
	return ret;
}

static void * dir_open(void * m, const char * name, int mode)
{
	struct mhandle_dir_t * mh = (struct mhandle_dir_t *)m;
	struct fhandle_dir_t * fh;
	char * path = concat(mh->path, "/", name, NULL);
	int fd;

	switch(mode)
	{
	case XFS_OPEN_MODE_READ:
		fd = sandbox_file_open(path, "r");
		break;
	case XFS_OPEN_MODE_WRITE:
		fd = sandbox_file_open(path, "r+w");
		break;
	case XFS_OPEN_MODE_APPEND:
		fd = sandbox_file_open(path, "r+a");
		break;
	default:
		fd = sandbox_file_open(path, "r");
		break;
	}
	if(fd < 0)
	{
		free(path);
		return NULL;
	}

	fh = malloc(sizeof(struct fhandle_dir_t));
	if(!fh)
	{
		sandbox_file_close(fd);
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
	return sandbox_file_read(fh->fd, buf, size);
}

static s64_t dir_write(void * f, void * buf, s64_t size)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	return sandbox_file_write(fh->fd, buf, size);
}

static s64_t dir_seek(void * f, s64_t offset)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	return sandbox_file_seek(fh->fd, offset);
}

static s64_t dir_tell(void * f)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	return sandbox_file_tell(fh->fd);
}

static s64_t dir_length(void * f)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	return sandbox_file_length(fh->fd);
}

static void dir_close(void * f)
{
	struct fhandle_dir_t * fh = (struct fhandle_dir_t *)f;
	sandbox_file_close(fh->fd);
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
	.tell		= dir_tell,
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
