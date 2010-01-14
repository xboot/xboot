/*
 * kernel/fs/fsapi.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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
x_s32 mount(const char * dev, const char * dir, const char * fs, x_u32 flags)
{
	char dev_path[MAX_PATH];
	char dir_path[MAX_PATH];
	struct stat st;
	x_s32 err;

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
x_s32 umount(const char * dir)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(dir, buf)) != 0)
		return err;

	return sys_umount(buf);
}

/*
 * open a file with flags and mode and return file descriptor.
 */
x_s32 open(const char * path, x_u32 flags, x_u32 mode)
{
	char buf[MAX_PATH];
	struct file * fp;
	x_s32 fd;
	x_s32 err;

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
x_s32 read(x_s32 fd, void * buf, x_size len)
{
	struct file * fp;
	x_size bytes;
	x_s32 err;

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
x_s32 write(x_s32 fd, void * buf, x_size len)
{
	struct file * fp;
	x_size bytes;
	x_s32 err;

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
x_off lseek(x_s32 fd, x_off offset, x_s32 whence)
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
x_s32 fstat(x_s32 fd, struct stat * st)
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
x_s32 ioctl(x_s32 fd, x_u32 cmd, void * arg)
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
x_s32 fsync(x_s32 fd)
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
x_s32 close(x_s32 fd)
{
	struct file * fp;
	x_s32 err;

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
	x_s32 fd;
	x_s32 err;

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
x_s32 rewinddir(void * dir)
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
x_s32 closedir(void * dir)
{
	struct file * fp;
	struct dir * pdir;
	x_s32 err;

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
char * getcwd(char * buf, x_s32 size)
{
	return vfs_getcwd(buf, size);
}

/*
 * change the current working directory to the specified path
 */
x_s32 chdir(const char * path)
{
	char buf[MAX_PATH];
	struct file * fp;
	x_s32 err;

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
x_s32 mkdir(const char * path, x_u32 mode)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_mkdir(buf, mode);
}

/*
 * remove a empty directories
 */
x_s32 rmdir(const char * path)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_rmdir(buf);
}

/*
 * get file's status
 */
x_s32 stat(const char * path, struct stat * st)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_stat(buf, st);
}

/*
 * test for access to a file with permission.
 */
x_s32 access(const char * path, x_u32 mode)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(path, buf)) != 0)
		return err;

	return sys_access(buf, mode);
}

/*
 * rename a file or directory
 */
x_s32 rename(char * oldpath, char * newpath)
{
	char src[MAX_PATH];
	char dest[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(oldpath, src)) != 0)
		return err;

	if((err = vfs_path_conv(newpath, dest)) != 0)
		return err;

	return sys_rename(src, dest);
}

/*
 * remove a file
 */
x_s32 unlink(const char * path)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(path, buf)) != 0)
		return err;

	return sys_unlink(buf);
}

/*
 * create the special node.
 */
x_s32 mknod(const char * path, x_u32 mode)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_mknod(buf, mode);
}

/*
 * change the access permissions of a file
 */
x_s32 chmod(const char * path, x_u32 mode)
{
	return -1;
}

/*
 * change the owner and group id of path to the numeric uid and gid
 */
x_s32 chown(const char * path, x_u32 owner, x_u32 group)
{
	return -1;
}

/*
 * set the current numeric umask and return the previous umask.
 */
x_u32 umask(x_u32 mode)
{
	return -1;
}

/*
 * truncate a file to a specified length by file descriptor
 */
x_s32 ftruncate(x_s32 fd, x_off length)
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
x_s32 truncate(const char * path, x_off length)
{
	char buf[MAX_PATH];
	x_s32 err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_truncate(buf, length);
}
