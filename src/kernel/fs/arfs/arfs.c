/*
 * kernel/fs/arfs/arfs.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <types.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <xboot/initcall.h>
#include <block/block.h>
#include <xboot/device.h>
#include <fs/vfs/vfs.h>
#include <fs/fs.h>

struct ar_hdr
{
	/* member file name, sometimes '/' terminated */
	s8_t ar_name[16];

	/* file date, decimal seconds since epoch */
	s8_t ar_date[12];

	 /* user and group id, in ascii decimal */
	s8_t ar_uid[6];
	s8_t ar_gid[6];

	/* file mode, in ascii octal */
	s8_t ar_mode[8];

	/* File size, in ascii decimal.  */
	s8_t ar_size[10];

	/* always contains `\n */
	s8_t ar_fmag[2];
};

/*
 * filesystem operations
 */
static s32_t arfs_mount(struct mount_t * m, char * dev, s32_t flag)
{
	struct block_t * blk;
	u8_t buf[8];

	if(dev == NULL)
		return EINVAL;

	blk = (struct block_t *)m->m_dev;
	if(!blk)
		return EACCES;

	if(block_capacity(blk) <= 8)
		return EINTR;

	if(block_read(blk, buf, 0, 8) != 8)
		return EIO;

	/*
	 * check if the device includes valid archive image
	 */
	if(strncmp((const char *)(&buf[0]), "!<arch>\n", 8) != 0)
		return EINVAL;

	m->m_flags = (flag & MOUNT_MASK) | MOUNT_RDONLY;

	return 0;
}

static s32_t arfs_unmount(struct mount_t * m)
{
	return 0;
}

static s32_t arfs_sync(struct mount_t * m)
{
	return 0;
}

static s32_t arfs_vget(struct mount_t * m, struct vnode_t * node)
{
	return 0;
}

static s32_t arfs_statfs(struct mount_t * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static s32_t arfs_open(struct vnode_t * node, s32_t flag)
{
	return 0;
}

static s32_t arfs_close(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t arfs_read(struct vnode_t * node, struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	struct block_t * dev = (struct block_t *)node->v_mount->m_dev;
	loff_t off;
	loff_t len;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	if(fp->f_offset >= node->v_size)
		return 0;

	if(node->v_size - fp->f_offset < size)
		size = node->v_size - fp->f_offset;

	off = (loff_t)((s32_t)(node->v_data));
	len = block_read(dev, (u8_t *)buf, (off + fp->f_offset), size);

	fp->f_offset += len;
	*result = len;

	return 0;
}

static s32_t arfs_write(struct vnode_t * node , struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	return -1;
}

static s32_t arfs_seek(struct vnode_t * node, struct file_t * fp, loff_t off1, loff_t off2)
{
	if(off2 > (loff_t)(node->v_size))
		return -1;

	return 0;
}

static s32_t arfs_ioctl(struct vnode_t * node, struct file_t * fp, int cmd, void * arg)
{
	return -1;
}

static s32_t arfs_fsync(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t arfs_readdir(struct vnode_t * node, struct file_t * fp, struct dirent_t * dir)
{
	struct block_t * dev = (struct block_t *)node->v_mount->m_dev;
	struct ar_hdr header;
	loff_t off = 8;
	loff_t size;
	s8_t * p;
	s32_t i = 0;

	if(fp->f_offset == 0)
	{
		dir->d_type = DT_DIR;
		strlcpy((char *)&dir->d_name, ".", sizeof(dir->d_name));
	}
	else if(fp->f_offset == 1)
	{
		dir->d_type = DT_DIR;
		strlcpy((char *)&dir->d_name, "..", sizeof(dir->d_name));
	}
	else
	{
		while(1)
		{
			memset(&header, 0, sizeof(struct ar_hdr));
			block_read(dev, (u8_t *)(&header), off, sizeof(struct ar_hdr));

			if(strncmp((const char *)header.ar_fmag, "`\n", 2) != 0)
				return ENOENT;

			size = strtoll((const char *)(header.ar_size), NULL, 0);
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
		strlcpy((char *)&dir->d_name, (const char *)(header.ar_name), sizeof(dir->d_name));
	}

	dir->d_fileno = (u32_t)fp->f_offset;
	dir->d_namlen = (u16_t)strlen(dir->d_name);
	fp->f_offset++;

	return 0;
}

static s32_t arfs_lookup(struct vnode_t * dnode, char * name, struct vnode_t * node)
{
	struct block_t * dev = (struct block_t *)node->v_mount->m_dev;
	struct ar_hdr header;
	loff_t off = 8;
	loff_t size;
	s8_t * p;

	while(1)
	{
		memset(&header, 0, sizeof(struct ar_hdr));
		block_read(dev, (u8_t *)(&header), off, sizeof(struct ar_hdr));

		if(strncmp((const char *)header.ar_fmag, "`\n", 2) != 0)
			return ENOENT;

		size = strtoll((const char *)(header.ar_size), NULL, 0);
		if(size <= 0)
			return ENOENT;

		if((p = memchr((const void *)(header.ar_name), '/', 16)) != NULL)
			*p = '\0';

		if(strncmp((const char *)name, (const char *)(header.ar_name), 16) == 0)
			break;

		off += (sizeof(struct ar_hdr) + size);
		off += (off % 2);
	}

	node->v_type = VREG;
	node->v_size = size;
	node->v_data = (void *)((s32_t)(off + sizeof(struct ar_hdr)));
	node->v_mode = S_IRUSR | S_IRGRP | S_IROTH;

	return 0;
}

static s32_t arfs_create(struct vnode_t * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t arfs_remove(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	return -1;
}

static s32_t arfs_rename(struct vnode_t * dnode1, struct vnode_t * node1, char * name1, struct vnode_t *dnode2, struct vnode_t * node2, char * name2)
{
	return -1;
}

static s32_t arfs_mkdir(struct vnode_t * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t arfs_rmdir(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	return -1;
}

static s32_t arfs_getattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t arfs_setattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t arfs_inactive(struct vnode_t * node)
{
	return -1;
}

static s32_t arfs_truncate(struct vnode_t * node, loff_t length)
{
	return -1;
}

/*
 * arfs vnode operations
 */
static struct vnops_t arfs_vnops = {
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
static struct vfsops_t arfs_vfsops = {
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
static struct filesystem_t arfs = {
	.name		= "arfs",
	.vfsops		= &arfs_vfsops,
};

static __init void filesystem_arfs_init(void)
{
	filesystem_register(&arfs);
}

static __exit void filesystem_arfs_exit(void)
{
	filesystem_unregister(&arfs);
}

core_initcall(filesystem_arfs_init);
core_exitcall(filesystem_arfs_exit);
