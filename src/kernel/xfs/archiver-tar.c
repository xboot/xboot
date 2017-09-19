/*
 * kernel/xfs/archiver-tar.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xfs/archiver.h>

struct mhandle_tar_t {
	char * path;
};

struct fhandle_tar_t {
	int fd;
};

static void * tar_mount(const char * path, int * writable)
{
	return NULL;
}

static void tar_umount(void * m)
{
}

static void tar_walk(void * m, const char * name, xfs_walk_callback_t cb, void * data)
{
}

static bool_t tar_isdir(void * m, const char * name)
{
	return FALSE;
}

static bool_t tar_isfile(void * m, const char * name)
{
	return FALSE;
}

static bool_t tar_mkdir(void * m, const char * name)
{
	return FALSE;
}

static bool_t tar_remove(void * m, const char * name)
{
	return FALSE;
}

static void * tar_open(void * m, const char * name, int mode)
{
	return NULL;
}

static s64_t tar_read(void * f, void * buf, s64_t size)
{
	return 0;
}

static s64_t tar_write(void * f, void * buf, s64_t size)
{
	return 0;
}

static s64_t tar_seek(void * f, s64_t offset)
{
	return 0;
}

static s64_t tar_length(void * f)
{
	return 0;
}

static void tar_close(void * f)
{
}

static struct xfs_archiver_t archiver_tar = {
	.name		= "tar",
	.mount		= tar_mount,
	.umount 	= tar_umount,
	.walk		= tar_walk,
	.isdir		= tar_isdir,
	.isfile		= tar_isfile,
	.mkdir		= tar_mkdir,
	.remove		= tar_remove,
	.open		= tar_open,
	.read		= tar_read,
	.write		= tar_write,
	.seek		= tar_seek,
	.length		= tar_length,
	.close		= tar_close,
};

static __init void archiver_tar_init(void)
{
	register_archiver(&archiver_tar);
}

static __exit void archiver_tar_exit(void)
{
	unregister_archiver(&archiver_tar);
}

core_initcall(archiver_tar_init);
core_exitcall(archiver_tar_exit);
