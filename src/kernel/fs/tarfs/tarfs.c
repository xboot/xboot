/*
 * kernel/fs/tarfs/tarfs.c
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
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
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
	s8_t name[100];

	/* file mode */
	s8_t mode[8];

	/* user id */
	s8_t uid[8];

	/* group id */
	s8_t gid[8];

	/* file size in bytes */
	s8_t size[12];

	/* last modification time */
	s8_t mtime[12];

	/* checksum for header block */
	s8_t chksum[8];

	/* file type */
	s8_t filetype;

	/* name of linked file */
	s8_t linkname[100];

	/* ustar indicator "ustar" */
	s8_t magic[6];

	/* ustar version */
	s8_t version[2];

	/* user name */
	s8_t uname[32];

	/* group name */
	s8_t gname[32];

	/* device major number */
	s8_t devmajor[8];

	/* device minor number */
	s8_t devminor[8];

	/* filename prefix */
	s8_t prefix[155];

	/* reserver */
	s8_t reserver[12];
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
static s32_t tarfs_mount(struct mount * m, char * dev, s32_t flag)
{
	struct blkdev * blk;
	struct tar_header header;

	if(dev == NULL)
		return EINVAL;

	blk = (struct blkdev *)m->m_dev;
	if(!blk)
		return EACCES;

	if(get_blkdev_total_size(blk) <= sizeof(struct tar_header))
		return EINTR;

	if(bio_read(blk, (u8_t *)(&header), 0, sizeof(struct tar_header)) != sizeof(struct tar_header))
		return EIO;

	/*
	 * check if the device includes valid archive image
	 */
	if(strncmp((const char *)(header.magic), (const char *)"ustar", 5) != 0)
		return EINVAL;

	m->m_flags = (flag & MOUNT_MASK) | MOUNT_RDONLY;
	m->m_root->v_data = 0;
	m->m_data = NULL;

	return 0;
}

static s32_t tarfs_unmount(struct mount * m)
{
	m->m_data = NULL;
	return 0;
}

static s32_t tarfs_sync(struct mount * m)
{
	return 0;
}

static s32_t tarfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static s32_t tarfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static s32_t tarfs_open(struct vnode * node, s32_t flag)
{
	return 0;
}

static s32_t tarfs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static s32_t tarfs_read(struct vnode * node, struct file * fp, void * buf, loff_t size, loff_t * result)
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

static s32_t tarfs_write(struct vnode * node , struct file * fp, void * buf, loff_t size, loff_t * result)
{
	return -1;
}

static s32_t tarfs_seek(struct vnode * node, struct file * fp, loff_t off1, loff_t off2)
{
	if(off2 > (loff_t)(node->v_size))
		return -1;

	return 0;
}

static s32_t tarfs_ioctl(struct vnode * node, struct file * fp, int cmd, void * arg)
{
	return -1;
}

static s32_t tarfs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static s32_t tarfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct tar_header header;
	char name[MAX_NAME];
	loff_t off = 0;
	loff_t size;
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
			bio_read(dev, (u8_t *)(&header), off, sizeof(struct tar_header));

			if(strncmp((const char *)(header.magic), (const char *)"ustar", 5) != 0)
				return ENOENT;

			size = strtoll((const char *)(header.size), NULL, 0);
			if(size < 0)
				return ENOENT;

			if(size == 0)
				off += sizeof(struct tar_header);
			else
				off += sizeof(struct tar_header) + (((size + 512) >> 9) << 9);

			if(!get_next_token((const char *)header.name, (const char *)node->v_path, name))
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
		strlcpy((char *)&dir->d_name, name, sizeof(name));
	}

	dir->d_fileno = (u32_t)fp->f_offset;
	dir->d_namlen = (u16_t)strlen((const char *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static s32_t tarfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	struct blkdev * dev = (struct blkdev *)node->v_mount->m_dev;
	struct tar_header header;
	loff_t off = 0;
	loff_t size;
	u32_t mode;
	s8_t buf[9];

	while(1)
	{
		bio_read(dev, (u8_t *)(&header), off, sizeof(struct tar_header));

		if(strncmp((const char *)(header.magic), (const char *)"ustar", 5) != 0)
			return ENOENT;

		size = strtoll((const char *)(header.size), NULL, 0);
		if(size < 0)
			return ENOENT;

		if(check_path((const char *)(header.name), (const char *)(dnode->v_path), (const char *)name))
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

	buf[8] = '\0';
	memcpy(buf, (const s8_t *)(header.mode), 8);
	mode = strtoul((const char *)buf, NULL, 8);

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
	node->v_data = (void *)((s32_t)(off+ sizeof(struct tar_header)));

	return 0;
}

static s32_t tarfs_create(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t tarfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t tarfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static s32_t tarfs_mkdir(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t tarfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t tarfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t tarfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t tarfs_inactive(struct vnode * node)
{
	return -1;
}

static s32_t tarfs_truncate(struct vnode * node, loff_t length)
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

fs_initcall(filesystem_tarfs_init);
fs_exitcall(filesystem_tarfs_exit);
