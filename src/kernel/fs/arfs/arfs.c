/*
 * kernel/fs/arfs/arfs.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <error.h>
#include <time/xtime.h>
#include <shell/readline.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/scank.h>
#include <xboot/chrdev.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <fs/vfs/vfs.h>
#include <fs/fs.h>

struct ar_hdr
{
	/* member file name, sometimes '/' terminated */
	x_s8 ar_name[16];

	/* file date, decimal seconds since epoch */
	x_s8 ar_date[12];

	 /* user and group id, in ascii decimal */
	x_s8 ar_uid[6];
	x_s8 ar_gid[6];

	/* file mode, in ascii octal */
	x_s8 ar_mode[8];

	/* File size, in ascii decimal.  */
	x_s8 ar_size[10];

	/* always contains `\n */
	x_s8 ar_fmag[2];
};

/*
 * filesystem operations
 */
static x_s32 arfs_mount(struct mount * m, char * dev, x_s32 flag)
{
	struct blkdev * blk;
	x_u8 buf[8];

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev *)m->m_dev;
	if(!blk || !blk->info)
		return ENODEV;

	if(get_blkdev_total_size(blk) <= 8)
		return EINTR;

	if(bio_read(blk, buf, 0, 8) != 8)
		return EIO;

	/*
	 * check if the device includes valid archive image
	 */
	if(strncmp((const x_s8 *)(&buf[0]), (const x_s8 *)"!<arch>\n", 8) != 0)
		return EINVAL;

	m->m_flags = (flag & MOUNT_MASK) | MOUNT_RDONLY;

	return 0;
}

static x_s32 arfs_unmount(struct mount * m)
{
	return 0;
}

static x_s32 arfs_sync(struct mount * m)
{
	return 0;
}

static x_s32 arfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static x_s32 arfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static x_s32 arfs_open(struct vnode * node, x_s32 flag)
{
	return 0;
}

static x_s32 arfs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 arfs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	x_off off;
	x_size len;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	if(fp->f_offset >= node->v_size)
		return 0;

	if(node->v_size - fp->f_offset < size)
		size = node->v_size - fp->f_offset;

	off = (x_off)((x_s32)(node->v_data));
	len = bio_read(dev, (x_u8 *)buf, (off + fp->f_offset), size);

	fp->f_offset += len;
	*result = len;

	return 0;
}

static x_s32 arfs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 arfs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	if(off2 > (x_off)(node->v_size))
		return -1;

	return 0;
}

static x_s32 arfs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 arfs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 arfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct ar_hdr header;
	x_off off = 8;
	x_size size;
	x_s8 * p;
	x_s32 i = 0;

	if(fp->f_offset == 0)
	{
		dir->d_type = DT_DIR;
		strlcpy((x_s8 *)&dir->d_name, (const x_s8 *)".", sizeof(dir->d_name));
	}
	else if(fp->f_offset == 1)
	{
		dir->d_type = DT_DIR;
		strlcpy((x_s8 *)&dir->d_name, (const x_s8 *)"..", sizeof(dir->d_name));
	}
	else
	{
		while(1)
		{
			memset(&header, 0, sizeof(struct ar_hdr));
			bio_read(dev, (x_u8 *)(&header), off, sizeof(struct ar_hdr));

			if(strncmp((const x_s8 *)header.ar_fmag, (const x_s8 *)"`\n", 2) != 0)
				return ENOENT;

			size = simple_strtos64((const x_s8 *)(header.ar_size), NULL, 0);
			if(size <= 0)
				return ENOENT;

			if(i++ == fp->f_offset - 2)
				break;

			off += (sizeof(struct ar_hdr) + size);
			off += (off % 2);
		}

		dir->d_type = DT_REG;
		if((p = memchr((const void *)(header.ar_name), '/', 16)) != NULL)
			*p = '\0';
		strlcpy((x_s8 *)&dir->d_name, (const x_s8 *)(header.ar_name), sizeof(dir->d_name));
	}

	dir->d_fileno = (x_u32)fp->f_offset;
	dir->d_namlen = (x_u16)strlen((const x_s8 *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static x_s32 arfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct ar_hdr header;
	x_off off = 8;
	x_size size;
	x_s8 * p;

	while(1)
	{
		memset(&header, 0, sizeof(struct ar_hdr));
		bio_read(dev, (x_u8 *)(&header), off, sizeof(struct ar_hdr));

		if(strncmp((const x_s8 *)header.ar_fmag, (const x_s8 *)"`\n", 2) != 0)
			return ENOENT;

		size = simple_strtos64((const x_s8 *)(header.ar_size), NULL, 0);
		if(size <= 0)
			return ENOENT;

		if((p = memchr((const void *)(header.ar_name), '/', 16)) != NULL)
			*p = '\0';

		if(strncmp((const x_s8 *)name, (const x_s8 *)(header.ar_name), 16) == 0)
			break;

		off += (sizeof(struct ar_hdr) + size);
		off += (off % 2);
	}

	node->v_type = VREG;
	node->v_size = size;
	node->v_data = (void *)((x_s32)(off + sizeof(struct ar_hdr)));
	node->v_mode = S_IRUSR | S_IRGRP | S_IROTH;

	return 0;
}

static x_s32 arfs_create(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 arfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 arfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static x_s32 arfs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 arfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 arfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 arfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 arfs_inactive(struct vnode * node)
{
	return -1;
}

static x_s32 arfs_truncate(struct vnode * node, x_off length)
{
	return -1;
}

/*
 * arfs vnode operations
 */
static struct vnops arfs_vnops = {
	.vop_open 		= arfs_open,
	.vop_close		= arfs_close,
	.vop_read		= arfs_read,
	.vop_write		= arfs_write,
	.vop_seek		= arfs_seek,
	.vop_ioctl		= arfs_ioctl,
	.vop_fsync		= arfs_fsync,
	.vop_readdir	= arfs_readdir,
	.vop_lookup		= arfs_lookup,
	.vop_create		= arfs_create,
	.vop_remove		= arfs_remove,
	.vop_rename		= arfs_rename,
	.vop_mkdir		= arfs_mkdir,
	.vop_rmdir		= arfs_rmdir,
	.vop_getattr	= arfs_getattr,
	.vop_setattr	= arfs_setattr,
	.vop_inactive	= arfs_inactive,
	.vop_truncate	= arfs_truncate,
};

/*
 * file system operations
 */
static struct vfsops arfs_vfsops = {
	.vfs_mount		= arfs_mount,
	.vfs_unmount	= arfs_unmount,
	.vfs_sync		= arfs_sync,
	.vfs_vget		= arfs_vget,
	.vfs_statfs		= arfs_statfs,
	.vfs_vnops		= &arfs_vnops,
};

/*
 * arfs filesystem
 */
static struct filesystem arfs = {
	.name		= "arfs",
	.vfsops		= &arfs_vfsops,
};

static __init void filesystem_arfs_init(void)
{
	if(!filesystem_register(&arfs))
		LOG_E("register 'arfs' filesystem fail");
}

static __exit void filesystem_arfs_exit(void)
{
	if(!filesystem_unregister(&arfs))
		LOG_E("unregister 'arfs' filesystem fail");
}

module_init(filesystem_arfs_init, LEVEL_POSTCORE);
module_exit(filesystem_arfs_exit, LEVEL_POSTCORE);
