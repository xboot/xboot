/*
 * kernel/fs/fileio.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
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

#include <xboot.h>
#include <errno.h>
#include <malloc.h>
#include <xboot/printk.h>
#include <fs/vfs/fcntl.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/vfs.h>
#include <fs/fileio.h>

/*
 * mount a file system
 */
int mount(const char * dev, const char * dir, const char * fs, u32_t flags)
{
	char dev_path[MAX_PATH];
	char dir_path[MAX_PATH];
	struct stat st;
	int err;

	if((err = vfs_path_conv(dir, dir_path)) != 0)
		return err;

	if(dev != NULL)
	{
		if((err = vfs_path_conv(dev, dev_path)) != 0)
			return err;

		if(stat(dev_path, &st) != 0)
			return EEXIST;

		if(! S_ISBLK(st.st_mode))
			return EACCES;

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
int umount(const char * dir)
{
	char buf[MAX_PATH];
	int err;

	if((err = vfs_path_conv(dir, buf)) != 0)
		return err;

	return sys_umount(buf);
}

/*
 * open a file with flags and mode and return file descriptor.
 */
int open(const char * path, u32_t flags, u32_t mode)
{
	char buf[MAX_PATH];
	struct file * fp;
	int fd;
	int err;

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
loff_t read(int fd, void * buf, loff_t len)
{
	struct file * fp;
	loff_t bytes;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	if(sys_read(fp, buf, len, &bytes) != 0)
		return -1;

	return bytes;
}

/*
 * write to file
 */
loff_t write(int fd, void * buf, loff_t len)
{
	struct file * fp;
	loff_t bytes;

	if(fd < 0)
		return -1;

	if((fp = get_fp(fd)) == NULL)
		return -1;

	if(sys_write(fp, buf, len, &bytes) != 0)
		return -1;

	return bytes;
}

/*
 * seek a offset
 */
loff_t lseek(int fd, loff_t offset, s32_t whence)
{
	struct file * fp;
	loff_t org;

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
int fstat(int fd, struct stat * st)
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
int ioctl(int fd, int cmd, void * arg)
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
int fsync(int fd)
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
int close(int fd)
{
	struct file * fp;
	int err;

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
	int fd;
	int err;

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
int rewinddir(void * dir)
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
int closedir(void * dir)
{
	struct file * fp;
	struct dir * pdir;
	int err;

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
char * getcwd(char * buf, size_t size)
{
	return vfs_getcwd(buf, size);
}

/*
 * change the current working directory to the specified path
 */
int chdir(const char * path)
{
	char buf[MAX_PATH];
	struct file * fp;
	int err;

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
int mkdir(const char * path, u32_t mode)
{
	char buf[MAX_PATH];
	int err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_mkdir(buf, mode);
}

/*
 * remove a empty directories
 */
int rmdir(const char * path)
{
	char buf[MAX_PATH];
	int err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_rmdir(buf);
}

/*
 * get file's status
 */
int stat(const char * path, struct stat * st)
{
	char buf[MAX_PATH];
	int err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_stat(buf, st);
}

/*
 * test for access to a file with permission.
 */
int access(const char * path, u32_t mode)
{
	char buf[MAX_PATH];
	int err;

	if((err = vfs_path_conv(path, buf)) != 0)
		return err;

	return sys_access(buf, mode);
}

/*
 * rename a file or directory
 */
int rename(char * oldpath, char * newpath)
{
	char src[MAX_PATH];
	char dest[MAX_PATH];
	int err;

	if((err = vfs_path_conv(oldpath, src)) != 0)
		return err;

	if((err = vfs_path_conv(newpath, dest)) != 0)
		return err;

	return sys_rename(src, dest);
}

/*
 * remove a file
 */
int unlink(const char * path)
{
	char buf[MAX_PATH];
	int err;

	if((err = vfs_path_conv(path, buf)) != 0)
		return err;

	return sys_unlink(buf);
}

/*
 * create the special node.
 */
int mknod(const char * path, u32_t mode)
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
int chmod(const char * path, u32_t mode)
{
	return -1;
}

/*
 * change the owner and group id of path to the numeric uid and gid
 */
int chown(const char * path, u32_t owner, u32_t group)
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
int ftruncate(int fd, loff_t length)
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
int truncate(const char * path, loff_t length)
{
	char buf[MAX_PATH];
	int err;

	if((err = vfs_path_conv(path, buf)) !=0 )
		return err;

	return sys_truncate(buf, length);
}

ssize_t readv(int fd, const struct iovec * iov, int iovcnt)
{
	ssize_t count = 0;
	ssize_t bytes;

	while(iovcnt-- > 0)
	{
		bytes = read(fd, iov->iov_base, iov->iov_len);
		if(bytes < 0)
			return (-1);

		count += bytes;
		if (bytes != iov->iov_len)
			break;

		iov++;
	}

	return (count);
}

ssize_t writev(int fd, const struct iovec * iov, int iovcnt)
{
	ssize_t count = 0;
	ssize_t bytes;

	while(iovcnt-- > 0)
	{
		bytes = write(fd, iov->iov_base, iov->iov_len);
		if (bytes < 0)
			return (-1);

		count += bytes;
		if (bytes != iov->iov_len)
			break;

		iov++;
	}

	return (count);
}
