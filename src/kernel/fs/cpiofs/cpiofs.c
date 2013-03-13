/*
 * kernel/fs/cpiofs/cpiofs.c
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
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <byteorder.h>
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/chrdev.h>
#include <xboot/blkdev.h>
#include <xboot/device.h>
#include <fs/vfs/vfs.h>
#include <fs/fs.h>

struct cpio_newc_header {
	u8_t c_magic[6];
	u8_t c_ino[8];
	u8_t c_mode[8];
	u8_t c_uid[8];
	u8_t c_gid[8];
	u8_t c_nlink[8];
	u8_t c_mtime[8];
	u8_t c_filesize[8];
	u8_t c_devmajor[8];
	u8_t c_devminor[8];
	u8_t c_rdevmajor[8];
	u8_t c_rdevminor[8];
	u8_t c_namesize[8];
	u8_t c_check[8];
} __attribute__ ((packed));

static bool_t get_next_token(const char * path, const char * perfix, char * result)
{
	char full_path[MAX_PATH];
	char *p, *q;
	s32_t l;

	if(!path || !perfix || !result)
		return FALSE;

	full_path[0] = '\0';

	if(path[0] != '/')
		strcpy(full_path, (const char *)("/"));
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

static bool_t check_path(const char * path, const char * perfix, const char * name)
{
	char path1[MAX_PATH];
	char path2[MAX_PATH];
	char *p;
	s32_t l;

	if(!path || !perfix || !name)
		return FALSE;

	path1[0] = path2[0] = '\0';

	if(path[0] != '/')
		strcpy(path1, (const char *)("/"));
	strlcat(path1, path, sizeof(path1));

	if(perfix[0] != '/')
		strcpy(path2, (const char *)("/"));
	strlcat(path2, perfix, sizeof(path2));

	if(path2[strlen(path2) - 1] != '/')
		strlcat(path2, (const char *)"/", sizeof(path2));
	strlcat(path2, (const char *)name, sizeof(path2));

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
static s32_t cpiofs_mount(struct mount * m, char * dev, s32_t flag)
{
	struct blkdev * blk;
	struct cpio_newc_header header;

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev *)m->m_dev;
	if(!blk)
		return EACCES;

	if(get_blkdev_total_size(blk) <= sizeof(struct cpio_newc_header))
		return EINTR;

	if(bio_read(blk, (u8_t *)(&header), 0, sizeof(struct cpio_newc_header)) != sizeof(struct cpio_newc_header))
		return EIO;

	if(strncmp((const char *)(header.c_magic), (const char *)"070701", 6) != 0)
		return EINVAL;

	m->m_flags = (flag & MOUNT_MASK) | MOUNT_RDONLY;
	m->m_root->v_data = 0;
	m->m_data = NULL;

	return 0;
}

static s32_t cpiofs_unmount(struct mount * m)
{
	m->m_data = NULL;
	return 0;
}

static s32_t cpiofs_sync(struct mount * m)
{
	return 0;
}

static s32_t cpiofs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static s32_t cpiofs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static s32_t cpiofs_open(struct vnode * node, s32_t flag)
{
	return 0;
}

static s32_t cpiofs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static s32_t cpiofs_read(struct vnode * node, struct file * fp, void * buf, loff_t size, loff_t * result)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
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
	len = bio_read(dev, (u8_t *)buf, (off + fp->f_offset), size);

	fp->f_offset += len;
	*result = len;

	return 0;
}

static s32_t cpiofs_write(struct vnode * node , struct file * fp, void * buf, loff_t size, loff_t * result)
{
	return -1;
}

static s32_t cpiofs_seek(struct vnode * node, struct file * fp, loff_t off1, loff_t off2)
{
	if(off2 > (loff_t)(node->v_size))
		return -1;

	return 0;
}

static s32_t cpiofs_ioctl(struct vnode * node, struct file * fp, int cmd, void * arg)
{
	return -1;
}

static s32_t cpiofs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static s32_t cpiofs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct cpio_newc_header header;
	char path[MAX_PATH];
	char name[MAX_NAME];
	u32_t size, name_size, mode;
	loff_t off = 0;
	char buf[9];
	s32_t i = 0;

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
		while(1)
		{
			bio_read(dev, (u8_t *)(&header), off, sizeof(struct cpio_newc_header));

			if(strncmp((const char *)(header.c_magic), (const char *)"070701", 6) != 0)
				return ENOENT;

			buf[8] = '\0';

			memcpy(buf, (const s8_t *)(header.c_filesize), 8);
			size = strtoul((const char *)buf, NULL, 16);

			memcpy(buf, (const s8_t *)(header.c_namesize), 8);
			name_size = strtoul((const char *)buf, NULL, 16);

			memcpy(buf, (const s8_t *)(header.c_mode), 8);
			mode = strtoul((const char *)buf, NULL, 16);

			bio_read(dev, (u8_t *)path, off + sizeof(struct cpio_newc_header), (loff_t)name_size);

			if( (size == 0) && (mode == 0) && (name_size == 11) && (strncmp(path, (const char *)"TRAILER!!!", 10) == 0) )
				return ENOENT;

			off = off + sizeof(struct cpio_newc_header) + (((name_size + 1) & ~3) + 2) + size;
			off = (off + 3) & ~3;

			if(!get_next_token(path, (const char *)node->v_path, name))
				continue;

			if(i++ == fp->f_offset - 2)
			{
				off = 0;
				break;
			}
		}

		if(mode & 0040000)
			dir->d_type = DT_DIR;
		else
			dir->d_type = DT_REG;
		strlcpy((char *)&dir->d_name, name, sizeof(name));
	}

	dir->d_fileno = (u32_t)fp->f_offset;
	dir->d_namlen = (u16_t)strlen((const char *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static s32_t cpiofs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct cpio_newc_header header;
	char path[MAX_PATH];
	u32_t size, name_size, mode;
	loff_t off = 0;
	s8_t buf[9];

	while(1)
	{
		bio_read(dev, (u8_t *)(&header), off, sizeof(struct cpio_newc_header));

		if(strncmp((const char *)(header.c_magic), (const char *)"070701", 6) != 0)
			return ENOENT;

		buf[8] = '\0';

		memcpy(buf, (const s8_t *)(header.c_filesize), 8);
		size = strtoul((const char *)buf, NULL, 16);

		memcpy(buf, (const s8_t *)(header.c_namesize), 8);
		name_size = strtoul((const char *)buf, NULL, 16);

		memcpy(buf, (const s8_t *)(header.c_mode), 8);
		mode = strtoul((const char *)buf, NULL, 16);

		bio_read(dev, (u8_t *)path, off + sizeof(struct cpio_newc_header), (loff_t)name_size);

		if( (size == 0) && (mode == 0) && (name_size == 11) && (strncmp(path, (const char *)"TRAILER!!!", 10) == 0) )
			return ENOENT;

		if(check_path(path, (const char *)(dnode->v_path), (const char *)name))
			break;

		off = off + sizeof(struct cpio_newc_header) + (((name_size + 1) & ~3) + 2) + size;
		off = (off + 3) & ~3;
	}

	if((mode & 00170000) == 0140000)
		node->v_type = VSOCK;
	else if((mode & 00170000) == 0120000)
		node->v_type = VLNK;
	else if((mode & 00170000) == 0100000)
		node->v_type = VREG;
	else if((mode & 00170000) == 0060000)
		node->v_type = VBLK;
	else if((mode & 00170000) == 0040000)
		node->v_type = VDIR;
	else if((mode & 00170000) == 0020000)
		node->v_type = VCHR;
	else if((mode & 00170000) == 0010000)
		node->v_type = VFIFO;
	else
		node->v_type = VREG;

	node->v_mode = 0;
	if(mode & 00400)
		node->v_mode |= S_IRUSR;
	if(mode & 00200)
		node->v_mode |= S_IWUSR;
	if(mode & 00100)
		node->v_mode |= S_IXUSR;
	if(mode & 00040)
		node->v_mode |= S_IRGRP;
	if(mode & 00020)
		node->v_mode |= S_IWGRP;
	if(mode & 00010)
		node->v_mode |= S_IXGRP;
	if(mode & 00004)
		node->v_mode |= S_IROTH;
	if(mode & 00002)
		node->v_mode |= S_IWOTH;
	if(mode & 00001)
		node->v_mode |= S_IXOTH;

	node->v_size = size;
	node->v_data = (void *)((s32_t)(off + sizeof(struct cpio_newc_header) + (((name_size + 1) & ~3) + 2)));

	return 0;
}

static s32_t cpiofs_create(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t cpiofs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t cpiofs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static s32_t cpiofs_mkdir(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t cpiofs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t cpiofs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t cpiofs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t cpiofs_inactive(struct vnode * node)
{
	return -1;
}

static s32_t cpiofs_truncate(struct vnode * node, loff_t length)
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

fs_initcall(filesystem_cpiofs_init);
fs_exitcall(filesystem_cpiofs_exit);
