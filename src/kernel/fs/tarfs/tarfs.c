/*
 * kernel/fs/tarfs/tarfs.c
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

enum {
	FILE_TYPE_NORMAL		= '0',
	FILE_TYPE_HARD_LINK		= '1',
	FILE_TYPE_SYMBOLIC_LINK = '2',
	FILE_TYPE_CHAR_DEVICE	= '3',
	FILE_TYPE_BLOCK_DEVICE	= '4',
	FILE_TYPE_DIRECTORY		= '5',
	FILE_TYPE_FIFO			= '6',
	FILE_TYPE_CONTIGOUS		= '7',
};

struct tar_header
{
	/* file name */
	x_s8 name[100];

	/* file mode */
	x_s8 mode[8];

	/* user id */
	x_s8 uid[8];

	/* group id */
	x_s8 gid[8];

	/* file size in bytes */
	x_s8 size[12];

	/* last modification time */
	x_s8 mtime[12];

	/* checksum for header block */
	x_s8 chksum[8];

	/* file type */
	x_s8 filetype;

	/* name of linked file */
	x_s8 linkname[100];

	/* ustar indicator "ustar" */
	x_s8 magic[6];

	/* ustar version */
	x_s8 version[2];

	/* user name */
	x_s8 uname[32];

	/* group name */
	x_s8 gname[32];

	/* device major number */
	x_s8 devmajor[8];

	/* device minor number */
	x_s8 devminor[8];

	/* filename prefix */
	x_s8 prefix[155];

	/* reserver */
	x_s8 reserver[12];
} __attribute__ ((packed));

/*
 * filesystem operations
 */
static x_s32 tarfs_mount(struct mount * m, char * dev, x_s32 flag)
{
	struct blkdev * blk;
	struct tar_header header;

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev *)m->m_dev;
	if(!blk || !blk->info)
		return ENODEV;

	if(get_blkdev_total_size(blk) <= sizeof(struct tar_header))
		return EINTR;

	if(bio_read(blk, (x_u8 *)(&header), 0, sizeof(struct tar_header)) != sizeof(struct tar_header))
		return EIO;

	/*
	 * check if the device includes valid archive image
	 */
	if(strncmp((const x_s8 *)(header.magic), (const x_s8 *)"ustar", 5) != 0)
		return EINVAL;

	m->m_flags = (flag & MOUNT_MASK) | MOUNT_RDONLY;

	return 0;
}

static x_s32 tarfs_unmount(struct mount * m)
{
	return 0;
}

static x_s32 tarfs_sync(struct mount * m)
{
	return 0;
}

static x_s32 tarfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static x_s32 tarfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static x_s32 tarfs_open(struct vnode * node, x_s32 flag)
{
	return 0;
}

static x_s32 tarfs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 tarfs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
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

static x_s32 tarfs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 tarfs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	if(off2 > (x_off)(node->v_size))
		return -1;

	return 0;
}

static x_s32 tarfs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 tarfs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 tarfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct tar_header header;
	x_s8 path[MAX_PATH];
	x_s8 *p, *q;
	x_off off = 0;
	x_size size;
	x_s32 pos, i = 0;

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
			memset(&header, 0, sizeof(struct tar_header));
			bio_read(dev, (x_u8 *)(&header), off, sizeof(struct tar_header));

			if(strncmp((const x_s8 *)(header.magic), (const x_s8 *)"ustar", 5) != 0)
				return ENOENT;

			size = simple_strtos64((const x_s8 *)(header.size), NULL, 0);
			if(size < 0)
				return ENOENT;

			if(size == 0)
				off += sizeof(struct tar_header);
			else
				off += sizeof(struct tar_header) + (((size + 512) >> 9) << 9);

			if(header.name[0] != '/')
				strcpy(path, (const x_s8 *)("/"));
			strlcat(path, (const x_s8 *)(header.name), sizeof(path));
			pos = strspn(path, (const x_s8 *)node->v_path);
			if(pos <= 0)
				continue;
			p = &path[pos - 1];
			if(*p == '/')
				p++;
			if(*p == 0)
				continue;
			q = strchr(p, '/');
			if(q)
				*q = 0;
			if(*(q+1) != 0)
				continue;

			if(i++ == fp->f_offset - 2)
			{
				off = 0;
				break;
			}
		}

		if(header.filetype == FILE_TYPE_DIRECTORY)
			dir->d_type = DT_DIR;
		else
			dir->d_type = DT_REG;

		strlcpy((x_s8 *)&dir->d_name, p, sizeof(header.name));
	}

	dir->d_fileno = (x_u32)fp->f_offset;
	dir->d_namlen = (x_u16)strlen((const x_s8 *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static x_s32 tarfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
//	printk("path:%s,%s\r\n", name, dnode->v_path);

	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct tar_header header;
	x_s8 *p, path[MAX_PATH];
	x_off off = 0;
	x_size size;

	strcpy(path, (const x_s8 *)dnode->v_path);
	if(strcmp(path, "/") != 0)
		strlcat(path, (const x_s8 *)"/", sizeof(path));
	strlcat(path, (const x_s8 *)name, sizeof(path));
	p = path;
	if(*p == '/')
		p++;

	while(1)
	{
		memset(&header, 0, sizeof(struct tar_header));
		bio_read(dev, (x_u8 *)(&header), off, sizeof(struct tar_header));

		if(strncmp((const x_s8 *)(header.magic), (const x_s8 *)"ustar", 5) != 0)
			return ENOENT;

		size = simple_strtos64((const x_s8 *)(header.size), NULL, 0);
		if(size < 0)
			return ENOENT;

		if(size == 0)
			off += sizeof(struct tar_header);
		else
			off += sizeof(struct tar_header) + (((size + 512) >> 9) << 9);

		//printk("xx:%s\r\n", header.name);
		if(strncmp((const x_s8 *)p, (const x_s8 *)(header.name), strlen((const x_s8 *)p) - 1) == 0)
			break;
	}

	if(header.filetype == FILE_TYPE_DIRECTORY)
		node->v_type = VDIR;
	else
		node->v_type = VREG;

	node->v_size = size;
	node->v_data = (void *)((x_s32)off);
	node->v_mode = S_IRUSR | S_IRGRP | S_IROTH;

	return 0;
}

static x_s32 tarfs_create(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 tarfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 tarfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static x_s32 tarfs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 tarfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 tarfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 tarfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 tarfs_inactive(struct vnode * node)
{
	return -1;
}

static x_s32 tarfs_truncate(struct vnode * node, x_off length)
{
	return -1;
}

/*
 * tarfs vnode operations
 */
static struct vnops tarfs_vnops = {
	.vop_open 		= tarfs_open,
	.vop_close		= tarfs_close,
	.vop_read		= tarfs_read,
	.vop_write		= tarfs_write,
	.vop_seek		= tarfs_seek,
	.vop_ioctl		= tarfs_ioctl,
	.vop_fsync		= tarfs_fsync,
	.vop_readdir	= tarfs_readdir,
	.vop_lookup		= tarfs_lookup,
	.vop_create		= tarfs_create,
	.vop_remove		= tarfs_remove,
	.vop_rename		= tarfs_rename,
	.vop_mkdir		= tarfs_mkdir,
	.vop_rmdir		= tarfs_rmdir,
	.vop_getattr	= tarfs_getattr,
	.vop_setattr	= tarfs_setattr,
	.vop_inactive	= tarfs_inactive,
	.vop_truncate	= tarfs_truncate,
};

/*
 * file system operations
 */
static struct vfsops tarfs_vfsops = {
	.vfs_mount		= tarfs_mount,
	.vfs_unmount	= tarfs_unmount,
	.vfs_sync		= tarfs_sync,
	.vfs_vget		= tarfs_vget,
	.vfs_statfs		= tarfs_statfs,
	.vfs_vnops		= &tarfs_vnops,
};

/*
 * tarfs filesystem
 */
static struct filesystem tarfs = {
	.name		= "tarfs",
	.vfsops		= &tarfs_vfsops,
};

static __init void filesystem_tarfs_init(void)
{
	if(!filesystem_register(&tarfs))
		LOG_E("register 'tarfs' filesystem fail");
}

static __exit void filesystem_tarfs_exit(void)
{
	if(!filesystem_unregister(&tarfs))
		LOG_E("unregister 'tarfs' filesystem fail");
}

module_init(filesystem_tarfs_init, LEVEL_POSTCORE);
module_exit(filesystem_tarfs_exit, LEVEL_POSTCORE);
