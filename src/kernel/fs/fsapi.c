/*
 * kernel/fs/fsapi.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <error.h>
#include <malloc.h>
#include <fs/vfs/fcntl.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/vfs.h>
#include <fs/fsapi.h>


/*
 * mount a file system
 */
s32_t mount(const char * dev, const char * dir, const char * fs, u32_t flags)
{
	char dev_path[MAX_PATH];
	char dir_path[MAX_PATH];
	struct stat st;
	s32_t err;

	if((err = vfs_path_conv(dir, dir_path)) != 0)
		return err;

	if(dev != NULL)
	{
		if((err = vfs_path_conv(dev, dev_path)) != 0)
			return err;

		if(stat(dev_path, &st) != 0)
			return EEXIST;

		if(! S_ISBLK(st.st_mode))
			return ENODEV;

		return sys_mount(dev_path, dir_path, (char *)fs, flags);
	}
	else
		return sys_mount(NULL, dir_path, (char *)fs, flags);
}

/*
 * flush file system buffers.
 */
void sync(void)
{
	sys_sync();
}

/*
 * unmount file systems
 */
s32_t umount(const char * dir)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(dir, buf)) != 0)
		return err;

	return sys_umount(buf);
}

/*
 * open a file with flags and mode and return file descriptor.
 */
s32_t open(const char * path, u32_t flags, u32_t mode)
{
	char buf[MAX_PATH];
	struct file * fp;
	s32_t fd;
	s32_t err;

	if((fd = fd_alloc(0)) < 0)
		return -1;

	if(vfs_path_conv(path, buf) !=0 )
	{
		fd_free(fd);
		return -1;
	}

	if((err = sys_open(buf, flags, mode, &fp)) != 0)
	{
		fd_free(fd);
		return err;
	}

	set_fp(fd, fp);
	return fd;
}

/*
 * read from file
 */
s32_t read(s32_t fd, void * buf, x_size len)
{
	struct file * fp;
	x_size bytes;
	s32_t err;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	if((err = sys_read(fp, buf, len, &bytes)) != 0)
		return err;

	return bytes;
}

/*
 * write to file
 */
s32_t write(s32_t fd, void * buf, x_size len)
{
	struct file * fp;
	x_size bytes;
	s32_t err;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	if((err = sys_write(fp, buf, len, &bytes)) != 0)
		return err;

	return bytes;
}

/*
 * seek a offset
 */
x_off lseek(s32_t fd, x_off offset, s32_t whence)
{
	struct file * fp;
	x_off org;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	if(sys_lseek(fp, offset, whence, &org) != 0)
		return -1;

	return org;
}

/*
 * stat a file by file descriptor
 */
s32_t fstat(s32_t fd, struct stat * st)
{
	struct file * fp;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	return sys_fstat(fp, st);
}

/*
 * input and output control
 */
s32_t ioctl(s32_t fd, u32_t cmd, void * arg)
{
	struct file * fp;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	return sys_ioctl(fp, cmd, arg);
}

/*
 * flush file system buffers by file descriptor.
 */
s32_t fsync(s32_t fd)
{
	struct file * fp;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	return sys_fsync(fp);
}

/*
 * close a file by file descriptor
 */
s32_t close(s32_t fd)
{
	struct file * fp;
	s32_t err;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	if((err = sys_close(fp)) != 0)
		return err;

	fd_free(fd);
	return 0;
}

/*
 * open a directory
 */
void * opendir(const char * name)
{
	char buf[MAX_PATH];
	struct dir * dir;
	struct file * fp;
	s32_t fd;
	s32_t err;

	if((dir = malloc(sizeof(struct dir))) == NULL)
		return NULL;

	/* find empty slot for file descriptor. */
	if((fd = fd_alloc(0)) < 0)
	{
		free(dir);
		return NULL;
	}

	if((err = vfs_path_conv(name, buf)) !=0 )
	{
		free(dir);
		fd_free(fd);
		return NULL;
	}

	if((err = sys_opendir(buf, &fp)) != 0)
	{
		free(dir);
		fd_free(fd);
		return NULL;
	}

	set_fp(fd, fp);
	dir->fd = fd;

	return (void *)dir;
}

/*
 * read a directory
 */
struct dirent * readdir(void * dir)
{
	struct dir * pdir;
	struct file * fp;

	if(!dir)
		return NULL;

	pdir = (struct dir *)dir;
	if((fp = get_fp(pdir->fd)) == NULL)
		return NULL;

	if(sys_readdir(fp, &pdir->entry) == 0)
		return &pdir->entry;
	return NULL;
}

/*
 * rewind a directory
 */
s32_t rewinddir(void * dir)
{
	struct dir * pdir;
	struct file * fp;

	if(!dir)
		return -1;

	pdir = (struct dir *)dir;
	if((fp = get_fp(pdir->fd)) == NULL)
		return -1;

	return sys_rewinddir(fp);
}

/*
 * close a directory
 */
s32_t closedir(void * dir)
{
	struct file * fp;
	struct dir * pdir;
	s32_t err;

	if(!dir)
		return -1;

	pdir = (struct dir *)dir;
	if((fp = get_fp(pdir->fd)) == NULL)
		return -1;

	if((err = sys_closedir(fp)) != 0)
		return err;

	fd_free(pdir->fd);
	free(dir);

	return 0;
}

/*
 * get the current working directory
 */
char * getcwd(char * buf, s32_t size)
{
	return vfs_getcwd(buf, size);
}

/*
 * change the current working directory to the specified path
 */
s32_t chdir(const char * path)
{
	char buf[MAX_PATH];
	struct file * fp;
	s32_t err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	/* check if directory exits */
	if((err = sys_opendir(buf, &fp)) != 0)
		return err;

	/* new fp for current work directory */
	if(vfs_getcwdfp())
		sys_closedir(vfs_getcwdfp());
	vfs_setcwdfp(fp);

	/* set current work directory */
	vfs_setcwd(buf);

	return 0;
}

/*
 * create a directory with mode
 */
s32_t mkdir(const char * path, u32_t mode)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_mkdir(buf, mode);
}

/*
 * remove a empty directories
 */
s32_t rmdir(const char * path)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_rmdir(buf);
}

/*
 * get file's status
 */
s32_t stat(const char * path, struct stat * st)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_stat(buf, st);
}

/*
 * test for access to a file with permission.
 */
s32_t access(const char * path, u32_t mode)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(path, buf)) != 0)
		return err;

	return sys_access(buf, mode);
}

/*
 * rename a file or directory
 */
s32_t rename(char * oldpath, char * newpath)
{
	char src[MAX_PATH];
	char dest[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(oldpath, src)) != 0)
		return err;

	if((err = vfs_path_conv(newpath, dest)) != 0)
		return err;

	return sys_rename(src, dest);
}

/*
 * remove a file
 */
s32_t unlink(const char * path)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(path, buf)) != 0)
		return err;

	return sys_unlink(buf);
}

/*
 * create the special node.
 */
s32_t mknod(const char * path, u32_t mode)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_mknod(buf, mode);
}

/*
 * change the access permissions of a file
 */
s32_t chmod(const char * path, u32_t mode)
{
	return -1;
}

/*
 * change the owner and group id of path to the numeric uid and gid
 */
s32_t chown(const char * path, u32_t owner, u32_t group)
{
	return -1;
}

/*
 * set the current numeric umask and return the previous umask.
 */
u32_t umask(u32_t mode)
{
	return -1;
}

/*
 * truncate a file to a specified length by file descriptor
 */
s32_t ftruncate(s32_t fd, x_off length)
{
	struct file * fp;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	return sys_ftruncate(fp, length);
}

/*
 * truncate a file to a specified length by file path
 */
s32_t truncate(const char * path, x_off length)
{
	char buf[MAX_PATH];
	s32_t err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_truncate(buf, length);
}
