/*
 * kernel/fs/cpiofs/cpiofs.c
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
#include <byteorder.h>
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

struct cpio_header {
	x_u16 magic;
	x_u16 dev;
	x_u16 ino;
	x_u16 mode;
	x_u16 uid;
	x_u16 gid;
	x_u16 nlink;
	x_u16 rdev;
	x_u16 mtime[2];
	x_u16 namesize;
	x_u16 filesize[2];
} __attribute__ ((packed));

static x_bool get_next_token(const x_s8 * path, const x_s8 * perfix, x_s8 * result)
{
	x_s8 full_path[MAX_PATH];
	x_s8 *p, *q;
	x_s32 l;

	if(!path || !perfix || !result)
		return FALSE;

	full_path[0] = '\0';

	if(path[0] != '/')
		strcpy(full_path, (const x_s8 *)("/"));
	strlcat(full_path, path, sizeof(full_path));

	l = strlen(perfix);
	if(memcmp(full_path, perfix, l) != 0)
		return FALSE;

	p = &full_path[l];
	if(*p == '\0')
		return FALSE;
	if(*p == '/')
		p++;
	if(*p == '\0')
		return FALSE;

	q = strchr(p, '/');
	if(q)
	{
		if(*(q+1) != '\0')
			return FALSE;
		*q = 0;
	}

	strcpy(result, p);

	return TRUE;
}

static x_bool check_path(const x_s8 * path, const x_s8 * perfix, const x_s8 * name)
{
	x_s8 path1[MAX_PATH];
	x_s8 path2[MAX_PATH];
	x_s8 *p;
	x_s32 l;

	if(!path || !perfix || !name)
		return FALSE;

	path1[0] = path2[0] = '\0';

	if(path[0] != '/')
		strcpy(path1, (const x_s8 *)("/"));
	strlcat(path1, path, sizeof(path1));

	if(perfix[0] != '/')
		strcpy(path2, (const x_s8 *)("/"));
	strlcat(path2, perfix, sizeof(path2));

	if(path2[strlen(path2) - 1] != '/')
		strlcat(path2, (const x_s8 *)"/", sizeof(path2));
	strlcat(path2, (const x_s8 *)name, sizeof(path2));

	l = strlen(path2);
	if(memcmp(path1, path2, l) != 0)
		return FALSE;

	p = &path1[l];
	if(*p == '\0')
		return TRUE;
	if(*p == '/')
		p++;
	if(*p == '\0')
		return TRUE;

	return FALSE;
}

/*
 * filesystem operations
 */
static x_s32 cpiofs_mount(struct mount * m, char * dev, x_s32 flag)
{
	struct blkdev * blk;
	struct cpio_header header;

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev *)m->m_dev;
	if(!blk || !blk->info)
		return ENODEV;

	if(get_blkdev_total_size(blk) <= sizeof(struct cpio_header))
		return EINTR;

	if(bio_read(blk, (x_u8 *)(&header), 0, sizeof(struct cpio_header)) != sizeof(struct cpio_header))
		return EIO;

	header.magic = cpu_to_le16(header.magic & 0xffff);
	if(header.magic != 070707)
		return EINVAL;

	m->m_flags = (flag & MOUNT_MASK) | MOUNT_RDONLY;
	m->m_root->v_data = 0;
	m->m_data = NULL;

	return 0;
}

static x_s32 cpiofs_unmount(struct mount * m)
{
	m->m_data = NULL;
	return 0;
}

static x_s32 cpiofs_sync(struct mount * m)
{
	return 0;
}

static x_s32 cpiofs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static x_s32 cpiofs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static x_s32 cpiofs_open(struct vnode * node, x_s32 flag)
{
	return 0;
}

static x_s32 cpiofs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 cpiofs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
{
/*
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

	off = (x_off)(sizeof(struct tar_header) + (x_s32)(node->v_data));
	len = bio_read(dev, (x_u8 *)buf, (off + fp->f_offset), size);

	fp->f_offset += len;
	*result = len;
*/

	return 0;
}

static x_s32 cpiofs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 cpiofs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	if(off2 > (x_off)(node->v_size))
		return -1;

	return 0;
}

static x_s32 cpiofs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 cpiofs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 cpiofs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct cpio_header header;
	x_s8 name[MAX_NAME];
	x_off off = 0;
	x_size size;
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
			bio_read(dev, (x_u8 *)(&header), off, sizeof(struct cpio_header));

			header.magic = cpu_to_le16(header.magic & 0xffff);
			if(header.magic != 070707)
				return ENOENT;

			header.filesize[0] = cpu_to_le16(header.filesize[0] & 0xffff);
			header.filesize[1] = cpu_to_le16(header.filesize[1] & 0xffff);
			size = ((x_u32)(header.filesize[0]) << 16) | ((x_u32)header.filesize[1]);

			header.namesize = cpu_to_le16(header.namesize & 0xffff);
			if(header.namesize & 0x1)
				header.namesize++;
			bio_read(dev, (x_u8 *)name, off + sizeof(struct cpio_header), (x_size)header.namesize);

			header.mode = cpu_to_le16(header.mode & 0xffff);
			if( (size == 0) && (header.mode == 0) && (header.namesize == 11 + 1) && (memcmp(name, "TRAILER!!!", 11) == 0) )
				return ENOENT;

			printk("%Ld,%Ld\r\n",size, header.namesize);
			printk("%s\r\n", name);

			if(size & 0x1)
				off += sizeof(struct cpio_header) + header.namesize + size + 1;
			else
				off += sizeof(struct cpio_header) + header.namesize + size;

			/*
			if(size == 0)
				off += sizeof(struct tar_header);
			else
				off += sizeof(struct tar_header) + (((size + 512) >> 9) << 9);
http://www.mkssoftware.com/docs/man4/cpio.4.asp
			if(!get_next_token((const x_s8 *)header.name, (const x_s8 *)node->v_path, name))
				continue;
			*/

			if(i++ == fp->f_offset - 2)
			{
				off = 0;
				break;
			}
		}

/*		if(header.filetype == FILE_TYPE_DIRECTORY)
			dir->d_type = DT_DIR;
		else
			dir->d_type = DT_REG;
*/
		strlcpy((x_s8 *)&dir->d_name, name, sizeof(name));
	}

	dir->d_fileno = (x_u32)fp->f_offset;
	dir->d_namlen = (x_u16)strlen((const x_s8 *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static x_s32 cpiofs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
/*
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct tar_header header;
	x_off off = 0;
	x_size size;

	while(1)
	{
		bio_read(dev, (x_u8 *)(&header), off, sizeof(struct tar_header));

		if(strncmp((const x_s8 *)(header.magic), (const x_s8 *)"ustar", 5) != 0)
			return ENOENT;

		size = simple_strtos64((const x_s8 *)(header.size), NULL, 0);
		if(size < 0)
			return ENOENT;

		if(check_path((const x_s8 *)(header.name), (const x_s8 *)(dnode->v_path), (const x_s8 *)name))
			break;

		if(size == 0)
			off += sizeof(struct tar_header);
		else
			off += sizeof(struct tar_header) + (((size + 512) >> 9) << 9);
	}

	switch(header.filetype)
	{
	case FILE_TYPE_NORMAL:
		node->v_type = VREG;
		break;

	case FILE_TYPE_HARD_LINK:
	case FILE_TYPE_SYMBOLIC_LINK:
		node->v_type = VLNK;
		break;

	case FILE_TYPE_CHAR_DEVICE:
		node->v_type = VCHR;
		break;

	case FILE_TYPE_BLOCK_DEVICE:
		node->v_type = VBLK;
		break;

	case FILE_TYPE_DIRECTORY:
		node->v_type = VDIR;
		break;

	case FILE_TYPE_FIFO:
		node->v_type = VFIFO;
		break;

	case FILE_TYPE_CONTIGOUS:
		node->v_type = VSOCK;
		break;

	default:
		node->v_type = VREG;
		break;
	}

	node->v_size = size;
	node->v_data = (void *)((x_s32)off);
	node->v_mode = S_IRUSR | S_IRGRP | S_IROTH;
*/

	return 0;
}

static x_s32 cpiofs_create(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 cpiofs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 cpiofs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static x_s32 cpiofs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 cpiofs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 cpiofs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 cpiofs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 cpiofs_inactive(struct vnode * node)
{
	return -1;
}

static x_s32 cpiofs_truncate(struct vnode * node, x_off length)
{
	return -1;
}

/*
 * cpiofs vnode operations
 */
static struct vnops cpiofs_vnops = {
	.vop_open 		= cpiofs_open,
	.vop_close		= cpiofs_close,
	.vop_read		= cpiofs_read,
	.vop_write		= cpiofs_write,
	.vop_seek		= cpiofs_seek,
	.vop_ioctl		= cpiofs_ioctl,
	.vop_fsync		= cpiofs_fsync,
	.vop_readdir	= cpiofs_readdir,
	.vop_lookup		= cpiofs_lookup,
	.vop_create		= cpiofs_create,
	.vop_remove		= cpiofs_remove,
	.vop_rename		= cpiofs_rename,
	.vop_mkdir		= cpiofs_mkdir,
	.vop_rmdir		= cpiofs_rmdir,
	.vop_getattr	= cpiofs_getattr,
	.vop_setattr	= cpiofs_setattr,
	.vop_inactive	= cpiofs_inactive,
	.vop_truncate	= cpiofs_truncate,
};

/*
 * file system operations
 */
static struct vfsops cpiofs_vfsops = {
	.vfs_mount		= cpiofs_mount,
	.vfs_unmount	= cpiofs_unmount,
	.vfs_sync		= cpiofs_sync,
	.vfs_vget		= cpiofs_vget,
	.vfs_statfs		= cpiofs_statfs,
	.vfs_vnops		= &cpiofs_vnops,
};

/*
 * cpiofs filesystem
 */
static struct filesystem cpiofs = {
	.name		= "cpiofs",
	.vfsops		= &cpiofs_vfsops,
};

static __init void filesystem_cpiofs_init(void)
{
	if(!filesystem_register(&cpiofs))
		LOG_E("register 'cpiofs' filesystem fail");
}

static __exit void filesystem_cpiofs_exit(void)
{
	if(!filesystem_unregister(&cpiofs))
		LOG_E("unregister 'cpiofs' filesystem fail");
}

module_init(filesystem_cpiofs_init, LEVEL_POSTCORE);
module_exit(filesystem_cpiofs_exit, LEVEL_POSTCORE);
