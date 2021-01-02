/*
 * kernel/vfs/tar/tar.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <vfs/vfs.h>

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

struct tar_header_t
{
	/* File name */
	int8_t name[100];

	/* File mode */
	int8_t mode[8];

	/* User id */
	int8_t uid[8];

	/* Group id */
	int8_t gid[8];

	/* File size in bytes */
	int8_t size[12];

	/* Last modification time */
	int8_t mtime[12];

	/* Checksum for header block */
	int8_t chksum[8];

	/* File type */
	int8_t filetype;

	/* Link filename */
	int8_t linkname[100];

	/* Magic indicator "ustar" */
	int8_t magic[6];

	/* Version */
	int8_t version[2];

	/* User name */
	int8_t uname[32];

	/* Group name */
	int8_t gname[32];

	/* Device major number */
	int8_t devmajor[8];

	/* Device minor number */
	int8_t devminor[8];

	/* Filename prefix */
	int8_t prefix[155];

	/* Reserver */
	int8_t reserver[12];
} __attribute__ ((packed));

static bool_t get_next_token(const char * path, const char * perfix, char * result)
{
	char full_path[VFS_MAX_PATH];
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
	char path1[VFS_MAX_PATH];
	char path2[VFS_MAX_PATH];
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

static int tar_mount(struct vfs_mount_t * m, const char * dev)
{
	struct tar_header_t header;
	u64_t rd;

	if(dev == NULL)
		return -1;

	if(block_capacity(m->m_dev) <= sizeof(struct tar_header_t))
		return -1;

	rd = block_read(m->m_dev, (u8_t *)(&header), 0, sizeof(struct tar_header_t));
	if(rd != sizeof(struct tar_header_t))
		return -1;

	if(strncmp((const char *)(header.magic), "ustar", 5) != 0)
		return -1;

	m->m_flags |= MOUNT_RO;
	m->m_root->v_data = NULL;
	m->m_data = NULL;

	return 0;
}

static int tar_unmount(struct vfs_mount_t * m)
{
	m->m_data = NULL;
	return 0;
}

static int tar_msync(struct vfs_mount_t * m)
{
	return 0;
}

static int tar_vget(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static int tar_vput(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static u64_t tar_read(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	u64_t toff;
	u64_t sz = 0;

	if(n->v_type != VNT_REG)
		return 0;

	if(off >= n->v_size)
		return 0;

	sz = len;
	if((n->v_size - off) < sz)
		sz = n->v_size - off;

	toff = (u64_t)((unsigned long)(n->v_data));
	sz = block_read(n->v_mount->m_dev, (u8_t *)buf, (toff + off), sz);

	return sz;
}

static u64_t tar_write(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	return 0;
}

static int tar_truncate(struct vfs_node_t * n, s64_t off)
{
	return -1;
}

static int tar_sync(struct vfs_node_t * n)
{
	return 0;
}

static int tar_readdir(struct vfs_node_t * dn, s64_t off, struct vfs_dirent_t * d)
{
	struct tar_header_t header;
	char name[VFS_MAX_NAME];
	u64_t toff = 0, size, rd;
	int i = 0;

	while(1)
	{
		rd = block_read(dn->v_mount->m_dev, (u8_t *)&header, toff, sizeof(struct tar_header_t));
		if(rd != sizeof(struct tar_header_t))
			return -1;

		if(strncmp((const char *)(header.magic), "ustar", 5) != 0)
			return -1;

		size = strtoull((const char *)(header.size), NULL, 0);
		if(size < 0)
			return -1;

		if(size == 0)
			toff += sizeof(struct tar_header_t);
		else
			toff += sizeof(struct tar_header_t) + (((size + 512) >> 9) << 9);

		if(!get_next_token((const char *)header.name, dn->v_path, name))
			continue;

		if(i++ == off)
		{
			toff = 0;
			break;
		}
	}

	switch(header.filetype)
	{
	case FILE_TYPE_NORMAL:
		d->d_type = VDT_REG;
		break;
	case FILE_TYPE_HARD_LINK:
		d->d_type = VDT_LNK;
		break;
	case FILE_TYPE_SYMBOLIC_LINK:
		d->d_type = VDT_LNK;
		break;
	case FILE_TYPE_CHAR_DEVICE:
		d->d_type = VDT_CHR;
		break;
	case FILE_TYPE_BLOCK_DEVICE:
		d->d_type = VDT_BLK;
		break;
	case FILE_TYPE_DIRECTORY:
		d->d_type = VDT_DIR;
		break;
	case FILE_TYPE_FIFO:
		d->d_type = VDT_FIFO;
		break;
	case FILE_TYPE_CONTIGOUS:
		d->d_type = VDT_SOCK;
		break;
	default:
		d->d_type = VDT_REG;
		break;
	}
	strlcpy(d->d_name, name, sizeof(d->d_name));
	d->d_off = off;
	d->d_reclen = 1;

	return 0;
}

static int tar_lookup(struct vfs_node_t * dn, const char * name, struct vfs_node_t * n)
{
	struct tar_header_t header;
	u64_t off = 0, size, rd;
	u64_t mtime;
	u32_t mode;
	char buf[9];

	while(1)
	{
		rd = block_read(dn->v_mount->m_dev, (u8_t *)&header, off, sizeof(struct tar_header_t));
		if(rd != sizeof(struct tar_header_t))
			return -1;

		if(strncmp((const char *)(header.magic), "ustar", 5) != 0)
			return -1;

		size = strtoull((const char *)(header.size), NULL, 0);
		if(size < 0)
			return -1;

		if(check_path((const char *)(header.name), dn->v_path, name))
			break;

		if(size == 0)
			off += sizeof(struct tar_header_t);
		else
			off += sizeof(struct tar_header_t) + (((size + 512) >> 9) << 9);
	}

	mtime = strtoull((const char *)(header.mtime), NULL, 0);
	n->v_atime = mtime;
	n->v_mtime = mtime;
	n->v_ctime = mtime;
	n->v_mode = 0;

	switch(header.filetype)
	{
	case FILE_TYPE_NORMAL:
		n->v_type = VNT_REG;
		n->v_mode |= S_IFREG;
		break;
	case FILE_TYPE_HARD_LINK:
		n->v_type = VNT_LNK;
		n->v_mode |= S_IFLNK;
		break;
	case FILE_TYPE_SYMBOLIC_LINK:
		n->v_type = VNT_LNK;
		n->v_mode |= S_IFLNK;
		break;
	case FILE_TYPE_CHAR_DEVICE:
		n->v_type = VNT_CHR;
		n->v_mode |= S_IFCHR;
		break;
	case FILE_TYPE_BLOCK_DEVICE:
		n->v_type = VNT_BLK;
		n->v_mode |= S_IFBLK;
		break;
	case FILE_TYPE_DIRECTORY:
		n->v_type = VNT_DIR;
		n->v_mode |= S_IFDIR;
		break;
	case FILE_TYPE_FIFO:
		n->v_type = VNT_FIFO;
		n->v_mode |= S_IFIFO;
		break;
	case FILE_TYPE_CONTIGOUS:
		n->v_type = VNT_SOCK;
		n->v_mode |= S_IFSOCK;
		break;
	default:
		n->v_type = VNT_REG;
		n->v_mode |= S_IFREG;
		break;
	}

	buf[8] = '\0';
	memcpy(buf, (const char *)(header.mode), 8);
	mode = strtoul(buf, NULL, 8);

	if(mode & 00400)
		n->v_mode |= S_IRUSR;
	if(mode & 00200)
		n->v_mode |= S_IWUSR;
	if(mode & 00100)
		n->v_mode |= S_IXUSR;
	if(mode & 00040)
		n->v_mode |= S_IRGRP;
	if(mode & 00020)
		n->v_mode |= S_IWGRP;
	if(mode & 00010)
		n->v_mode |= S_IXGRP;
	if(mode & 00004)
		n->v_mode |= S_IROTH;
	if(mode & 00002)
		n->v_mode |= S_IWOTH;
	if(mode & 00001)
		n->v_mode |= S_IXOTH;

	n->v_size = size;
	n->v_data = (void *)((unsigned long)(off + sizeof(struct tar_header_t)));

	return 0;
}

static int tar_create(struct vfs_node_t * dn, const char * filename, u32_t mode)
{
	return -1;
}

static int tar_remove(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	return -1;
}

static int tar_rename(struct vfs_node_t * sn, const char * sname, struct vfs_node_t * n, struct vfs_node_t * dn, const char * dname)
{
	return -1;
}

static int tar_mkdir(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	return -1;
}

static int tar_rmdir(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	return -1;
}

static int tar_chmod(struct vfs_node_t * n, u32_t mode)
{
	return -1;
}

static struct filesystem_t tar = {
	.name		= "tar",

	.mount		= tar_mount,
	.unmount	= tar_unmount,
	.msync		= tar_msync,
	.vget		= tar_vget,
	.vput		= tar_vput,

	.read		= tar_read,
	.write		= tar_write,
	.truncate	= tar_truncate,
	.sync		= tar_sync,
	.readdir	= tar_readdir,
	.lookup		= tar_lookup,
	.create		= tar_create,
	.remove		= tar_remove,
	.rename		= tar_rename,
	.mkdir		= tar_mkdir,
	.rmdir		= tar_rmdir,
	.chmod		= tar_chmod,
};

static __init void filesystem_tar_init(void)
{
	register_filesystem(&tar);
}

static __exit void filesystem_tar_exit(void)
{
	unregister_filesystem(&tar);
}

core_initcall(filesystem_tar_init);
core_exitcall(filesystem_tar_exit);
