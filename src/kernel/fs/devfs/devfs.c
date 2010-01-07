/*
 * kernel/fs/devfs/devfs.c
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
#include <types.h>
#include <debug.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <error.h>
#include <time/xtime.h>
#include <shell/readline.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/scank.h>
#include <fs/fs.h>


/*
 * filesystem operations
 */
static x_s32 devfs_mount(struct mount * m, char * dev, x_s32 flag, void * data)
{
	return 0;
}

static x_s32 devfs_unmount(struct mount * m)
{
	return 0;
}

static x_s32 devfs_sync(struct mount * m)
{
	return 0;
}

static x_s32 devfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static x_s32 devfs_statfs(struct mount * m, struct statfs * stat)
{
	return 0;
}

/*
 * vnode operations
 */
static x_s32 devfs_open(struct vnode * node, x_s32 flag)
{
	return -1;
}

static x_s32 devfs_close(struct vnode * node, struct file * fp)
{
	return -1;
}

static x_s32 devfs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 devfs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 devfs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	return -1;
}

static x_s32 devfs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 devfs_fsync(struct vnode * node, struct file * fp)
{
	return -1;
}

static x_s32 devfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	return -1;
}

static x_s32 devfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	return -1;
}

static x_s32 devfs_create(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 devfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 devfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static x_s32 devfs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 devfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 devfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 devfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 devfs_inactive(struct vnode * node)
{
	return -1;
}

static x_s32 devfs_truncate(struct vnode * node, x_off length)
{
	return -1;
}

/*
 * devfs vnode operations
 */
static struct vnops devfs_vnops = {
	.vop_open 		= devfs_open,
	.vop_close		= devfs_close,
	.vop_read		= devfs_read,
	.vop_write		= devfs_write,
	.vop_seek		= devfs_seek,
	.vop_ioctl		= devfs_ioctl,
	.vop_fsync		= devfs_fsync,
	.vop_readdir	= devfs_readdir,
	.vop_lookup		= devfs_lookup,
	.vop_create		= devfs_create,
	.vop_remove		= devfs_remove,
	.vop_rename		= devfs_rename,
	.vop_mkdir		= devfs_mkdir,
	.vop_rmdir		= devfs_rmdir,
	.vop_getattr	= devfs_getattr,
	.vop_setattr	= devfs_setattr,
	.vop_inactive	= devfs_inactive,
	.vop_truncate	= devfs_truncate,
};

/*
 * file system operations
 */
static struct vfsops devfs_vfsops = {
	.vfs_mount		= devfs_mount,
	.vfs_unmount	= devfs_unmount,
	.vfs_sync		= devfs_sync,
	.vfs_vget		= devfs_vget,
	.vfs_statfs		= devfs_statfs,
	.vfs_vnops		= &devfs_vnops,
};

/*
 * devfs filesystem
 */
static struct filesystem devfs = {
	.name		= "devfs",
	.vfsops		= &devfs_vfsops,
};

static __init void filesystem_devfs_init(void)
{
	if(!filesystem_register(&devfs))
		DEBUG_E("register 'devfs' filesystem fail");
}

static __exit void filesystem_devfs_exit(void)
{
	if(!filesystem_unregister(&devfs))
		DEBUG_E("unregister 'devfs' filesystem fail");
}

module_init(filesystem_devfs_init, LEVEL_POSTCORE);
module_exit(filesystem_devfs_exit, LEVEL_POSTCORE);
