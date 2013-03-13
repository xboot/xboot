/*
 * kernel/fs/devfs/devfs.c
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
#include <types.h>
#include <stdarg.h>
#include <malloc.h>
#include <errno.h>
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/chrdev.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <fs/vfs/vfs.h>
#include <fs/fs.h>

extern struct device_list * device_list;

/*
 * filesystem operations
 */
static s32_t devfs_mount(struct mount * m, char * dev, s32_t flag)
{
	if(dev != NULL)
		return EINVAL;

	m->m_flags = flag & MOUNT_MASK;
	m->m_data = (void *)device_list;

	return 0;
}

static s32_t devfs_unmount(struct mount * m)
{
	m->m_data = NULL;

	return 0;
}

static s32_t devfs_sync(struct mount * m)
{
	return 0;
}

static s32_t devfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static s32_t devfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static s32_t devfs_open(struct vnode * node, s32_t flag)
{
	struct device * dev;
	struct chrdev * chr;
	struct blkdev * blk;
	s8_t * path;

	path = (s8_t *)node->v_path;
	if(!strcmp((const char *)path, (const char *)"/"))
		return 0;

	if(*path == '/')
		path++;

	dev = search_device((const char *)path);
	if(dev == NULL)
		return -1;

	if(dev->type == CHAR_DEVICE)
	{
		chr = (struct chrdev *)(dev->priv);

		if(chr->open(chr) != 0)
			return -1;
	}
	else if(dev->type == BLOCK_DEVICE)
	{
		blk = (struct blkdev *)(dev->priv);

		if(blk->open(blk) != 0)
			return -1;
	}
	else
	{
		return -1;
	}

	/*
	 * store private data
	 */
	node->v_data = (void *)dev;

	return 0;
}

static s32_t devfs_close(struct vnode * node, struct file * fp)
{
	struct device * dev;
	struct chrdev * chr;
	struct blkdev * blk;
	s8_t * path;

	path = (s8_t *)node->v_path;
	if(!strcmp((const char *)path, (const char *)"/"))
		return 0;

	dev = (struct device *)node->v_data;
	if(dev == NULL)
		return -1;

	if(dev->type == CHAR_DEVICE)
	{
		chr = (struct chrdev *)(dev->priv);
		return(chr->close(chr));
	}
	else if(dev->type == BLOCK_DEVICE)
	{
		blk = (struct blkdev *)(dev->priv);
		return(blk->close(blk));
	}

	return -1;
}

static s32_t devfs_read(struct vnode * node, struct file * fp, void * buf, loff_t size, loff_t * result)
{
	struct device * dev = (struct device *)(node->v_data);
	struct chrdev * chr;
	struct blkdev * blk;
	loff_t len;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;

	if(node->v_type == VCHR)
	{
		chr = (struct chrdev *)(dev->priv);

		len = chr->read(chr, buf, size);
		fp->f_offset = 0;
		*result = len;

		return 0;
	}
	else if(node->v_type == VBLK)
	{
		blk = (struct blkdev *)(dev->priv);

		len = bio_read(blk, buf, fp->f_offset, size);
		fp->f_offset += len;
		*result = len;

		return 0;
	}

	return -1;
}

static s32_t devfs_write(struct vnode * node , struct file * fp, void * buf, loff_t size, loff_t * result)
{
	struct device * dev = (struct device *)(node->v_data);
	struct chrdev * chr;
	struct blkdev * blk;
	loff_t len;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;

	if(node->v_type == VCHR)
	{
		chr = (struct chrdev *)(dev->priv);

		len = chr->write(chr, buf, size);
		fp->f_offset = 0;
		*result = len;

		return 0;
	}
	else if(node->v_type == VBLK)
	{
		blk = (struct blkdev *)(dev->priv);

		len = bio_write(blk, buf, fp->f_offset, size);
		fp->f_offset += len;
		*result = len;

		return 0;
	}

	return -1;
}

static s32_t devfs_seek(struct vnode * node, struct file * fp, loff_t off1, loff_t off2)
{
	if(node->v_type == VBLK)
	{
		if((off2 < 0) || (off2 > (loff_t)node->v_size))
			return -1;
		else
			return 0;
	}

	return -1;
}

static s32_t devfs_ioctl(struct vnode * node, struct file * fp, int cmd, void * arg)
{
	struct device * dev = (struct device *)(node->v_data);
	struct chrdev * chr;
	struct blkdev * blk;

	if(node->v_type == VDIR)
		return EISDIR;

	if(node->v_type == VCHR)
	{
		chr = (struct chrdev *)(dev->priv);
		return(chr->ioctl(chr, cmd, arg));
	}
	else if(node->v_type == VBLK)
	{
		blk = (struct blkdev *)(dev->priv);
		return(blk->ioctl(blk, cmd, arg));
	}

	return -1;
}

static s32_t devfs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static s32_t devfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct device_list * plist = (struct device_list *)node->v_mount->m_data;
	struct device_list * list;
	struct list_head * pos;
	s32_t i;

	if(fp->f_offset == 0)
	{
		dir->d_type = DT_DIR;
		strlcpy((char *)&dir->d_name, (const char *)".", sizeof(dir->d_name));
	}
	else if(fp->f_offset == 1)
	{
		dir->d_type = DT_DIR;
		strlcpy((char *)&dir->d_name, (const char *)"..", sizeof(dir->d_name));
	}
	else
	{
		pos = (&plist->entry)->next;
		for(i = 0; i != (fp->f_offset - 2); i++)
		{
			pos = pos->next;
			if(pos == (&plist->entry))
				return EINVAL;
		}

		list = list_entry(pos, struct device_list, entry);
		if(list->device->type == CHAR_DEVICE)
		{
			dir->d_type = DT_CHR;
		}
		else if(list->device->type == BLOCK_DEVICE)
		{
			dir->d_type = DT_BLK;
		}
		else
		{
			dir->d_type = DT_UNKNOWN;
		}

		strlcpy((char *)&dir->d_name, (const char *)list->device->name, sizeof(dir->d_name));
	}

	dir->d_fileno = (u32_t)fp->f_offset;
	dir->d_namlen = (u16_t)strlen((const char *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static s32_t devfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	struct device * dev;
	struct chrdev * chr;
	struct blkdev * blk;

	dev = search_device(name);
	if(dev == NULL)
		return -1;

	if(dev->type == CHAR_DEVICE)
	{
		chr = (struct chrdev *)(dev->priv);

		node->v_type = VCHR;
		node->v_size = 0;
	}
	else if(dev->type == BLOCK_DEVICE)
	{
		blk = (struct blkdev *)(dev->priv);

		node->v_type = VBLK;
		node->v_size = get_blkdev_total_size(blk);
	}
	else
	{
		return -1;
	}

	node->v_data = (void *)dev;
	node->v_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

	return 0;
}

static s32_t devfs_create(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t devfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t devfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static s32_t devfs_mkdir(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t devfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t devfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t devfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t devfs_inactive(struct vnode * node)
{
	return -1;
}

static s32_t devfs_truncate(struct vnode * node, loff_t length)
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
		LOG_E("register 'devfs' filesystem fail");
}

static __exit void filesystem_devfs_exit(void)
{
	if(!filesystem_unregister(&devfs))
		LOG_E("unregister 'devfs' filesystem fail");
}

fs_initcall(filesystem_devfs_init);
fs_exitcall(filesystem_devfs_exit);
