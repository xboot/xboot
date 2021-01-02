/*
 * kernel/vfs/cpio/cpio.c
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

struct cpio_newc_header_t {
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

static bool_t get_next_token(const char * path, const char * prefix, char * result)
{
	const char * p, * q;
	int l;

	if(!path || !prefix || !result)
		return FALSE;

	if(*path == '/')
		path++;

	if(*prefix == '/')
		prefix++;

	l = strlen(prefix);
	if(strncmp(path, prefix, l) != 0)
		return FALSE;

	p = &path[l];
	if(*p == '\0')
		return FALSE;
	if(*p == '/')
		p++;
	if(*p == '\0')
		return FALSE;

	q = strchr(p, '/');
	if(q)
	{
		if (*(q + 1) != '\0')
			return FALSE;
		l = q - p;
	}
	else
	{
		l = strlen(p);
	}
	memcpy(result, p, l);
	result[l] = '\0';

	return TRUE;
}

static bool_t check_path(const char * path, const char * prefix, const char * name)
{
	int l;

	if(!path || !prefix || !name)
		return FALSE;

	if(path[0] == '/')
		path++;

	if(prefix[0] == '/')
		prefix++;

	l = strlen(prefix);
	if(l && (strncmp(path, prefix, l) != 0))
		return FALSE;

	path += l;

	if(path[0] == '/')
		path++;

	if(strcmp(path, name) != 0)
		return FALSE;

	return TRUE;
}

static int cpio_mount(struct vfs_mount_t * m, const char * dev)
{
	struct cpio_newc_header_t header;
	u64_t rd;

	if(dev == NULL)
		return -1;

	if(block_capacity(m->m_dev) <= sizeof(struct cpio_newc_header_t))
		return -1;

	rd = block_read(m->m_dev, (u8_t *)(&header), 0, sizeof(struct cpio_newc_header_t));
	if(rd != sizeof(struct cpio_newc_header_t))
		return -1;

	if(strncmp((const char *)header.c_magic, "070701", 6) != 0)
		return -1;

	m->m_flags |= MOUNT_RO;
	m->m_root->v_data = NULL;
	m->m_data = NULL;

	return 0;
}

static int cpio_unmount(struct vfs_mount_t * m)
{
	m->m_data = NULL;
	return 0;
}

static int cpio_msync(struct vfs_mount_t * m)
{
	return 0;
}

static int cpio_vget(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static int cpio_vput(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static u64_t cpio_read(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
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

static u64_t cpio_write(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	return 0;
}

static int cpio_truncate(struct vfs_node_t * n, s64_t off)
{
	return -1;
}

static int cpio_sync(struct vfs_node_t * n)
{
	return 0;
}

static int cpio_readdir(struct vfs_node_t * dn, s64_t off, struct vfs_dirent_t * d)
{
	struct cpio_newc_header_t header;
	char path[VFS_MAX_PATH];
	char name[VFS_MAX_NAME];
	u32_t size, name_size, mode;
	u64_t toff = 0, rd;
	char buf[9];
	int i = 0;

	while(1)
	{
		rd = block_read(dn->v_mount->m_dev, (u8_t *)&header, toff, sizeof(struct cpio_newc_header_t));
		if(rd != sizeof(struct cpio_newc_header_t))
			return -1;

		if(strncmp((const char *)&header.c_magic, "070701", 6) != 0)
			return -1;

		buf[8] = '\0';

		memcpy(buf, &header.c_filesize, 8);
		size = strtoul((const char *)buf, NULL, 16);

		memcpy(buf, &header.c_namesize, 8);
		name_size = strtoul((const char *)buf, NULL, 16);

		memcpy(buf, &header.c_mode, 8);
		mode = strtoul((const char *)buf, NULL, 16);

		rd = block_read(dn->v_mount->m_dev, (u8_t *)path, toff + sizeof(struct cpio_newc_header_t), name_size);
		if(!rd)
			return -1;

		if((size == 0) && (mode == 0) && (name_size == 11) && (strncmp(path, "TRAILER!!!", 10) == 0))
			return -1;

		toff += sizeof(struct cpio_newc_header_t);
		toff += (((name_size + 1) & ~3) + 2) + size;
		toff = (toff + 3) & ~3;

		if(path[0] == '.')
			continue;

		if(!get_next_token(path, dn->v_path, name))
			continue;

		if(i++ == off)
		{
			toff = 0;
			break;
		}
	}

	if((mode & 00170000) == 0140000)
	{
		d->d_type = VDT_SOCK;
	}
	else if((mode & 00170000) == 0120000)
	{
		d->d_type = VDT_LNK;
	}
	else if ((mode & 00170000) == 0100000)
	{
		d->d_type = VDT_REG;
	}
	else if ((mode & 00170000) == 0060000)
	{
		d->d_type = VDT_BLK;
	}
	else if ((mode & 00170000) == 0040000)
	{
		d->d_type = VDT_DIR;
	}
	else if ((mode & 00170000) == 0020000)
	{
		d->d_type = VDT_CHR;
	}
	else if ((mode & 00170000) == 0010000)
	{
		d->d_type = VDT_FIFO;
	}
	else
	{
		d->d_type = VDT_REG;
	}

	strlcpy(d->d_name, name, sizeof(d->d_name));
	d->d_off = off;
	d->d_reclen = 1;

	return 0;
}

static int cpio_lookup(struct vfs_node_t * dn, const char * name, struct vfs_node_t * n)
{
	struct cpio_newc_header_t header;
	char path[VFS_MAX_PATH];
	u64_t off = 0, rd;
	u32_t size, name_size, mode, mtime;
	u8_t buf[9];

	while(1)
	{
		rd = block_read(dn->v_mount->m_dev, (u8_t *)&header, off, sizeof(struct cpio_newc_header_t));
		if(rd != sizeof(struct cpio_newc_header_t))
			return -1;

		if(strncmp((const char *)header.c_magic, "070701", 6) != 0)
			return -1;

		buf[8] = '\0';

		memcpy(buf, &header.c_filesize, 8);
		size = strtoul((const char *)buf, NULL, 16);

		memcpy(buf, &header.c_namesize, 8);
		name_size = strtoul((const char *)buf, NULL, 16);

		memcpy(buf, &header.c_mode, 8);
		mode = strtoul((const char *)buf, NULL, 16);

		memcpy(buf, &header.c_mtime, 8);
		mtime = strtoul((const char *)buf, NULL, 16);

		rd = block_read(dn->v_mount->m_dev, (u8_t *)path, off + sizeof(struct cpio_newc_header_t), name_size);
		if(!rd)
			return -1;

		if((size == 0) && (mode == 0) && (name_size == 11) && (strncmp(path, "TRAILER!!!", 10) == 0))
			return -1;

		if((path[0] != '.') && check_path(path, dn->v_path, name))
			break;

		off += sizeof(struct cpio_newc_header_t);
		off += (((name_size + 1) & ~3) + 2) + size;
		off = (off + 3) & ~0x3;
	}

	n->v_atime = mtime;
	n->v_mtime = mtime;
	n->v_ctime = mtime;
	n->v_mode = 0;

	if((mode & 00170000) == 0140000)
	{
		n->v_type = VNT_SOCK;
		n->v_mode |= S_IFSOCK;
	}
	else if((mode & 00170000) == 0120000)
	{
		n->v_type = VNT_LNK;
		n->v_mode |= S_IFLNK;
	}
	else if((mode & 00170000) == 0100000)
	{
		n->v_type = VNT_REG;
		n->v_mode |= S_IFREG;
	}
	else if((mode & 00170000) == 0060000)
	{
		n->v_type = VNT_BLK;
		n->v_mode |= S_IFBLK;
	}
	else if((mode & 00170000) == 0040000)
	{
		n->v_type = VNT_DIR;
		n->v_mode |= S_IFDIR;
	}
	else if((mode & 00170000) == 0020000)
	{
		n->v_type = VNT_CHR;
		n->v_mode |= S_IFCHR;
	}
	else if((mode & 00170000) == 0010000)
	{
		n->v_type = VNT_FIFO;
		n->v_mode |= S_IFIFO;
	}
	else
	{
		n->v_type = VNT_REG;
	}

	n->v_mode |= (mode & 00400) ? S_IRUSR : 0;
	n->v_mode |= (mode & 00200) ? S_IWUSR : 0;
	n->v_mode |= (mode & 00100) ? S_IXUSR : 0;
	n->v_mode |= (mode & 00040) ? S_IRGRP : 0;
	n->v_mode |= (mode & 00020) ? S_IWGRP : 0;
	n->v_mode |= (mode & 00010) ? S_IXGRP : 0;
	n->v_mode |= (mode & 00004) ? S_IROTH : 0;
	n->v_mode |= (mode & 00002) ? S_IWOTH : 0;
	n->v_mode |= (mode & 00001) ? S_IXOTH : 0;
	n->v_size = size;

	off += sizeof(struct cpio_newc_header_t);
	off += (((name_size + 1) & ~3) + 2);
	n->v_data = (void *)((unsigned long)off);

	return 0;
}

static int cpio_create(struct vfs_node_t * dn, const char * filename, u32_t mode)
{
	return -1;
}

static int cpio_remove(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	return -1;
}

static int cpio_rename(struct vfs_node_t * sn, const char * sname, struct vfs_node_t * n, struct vfs_node_t * dn, const char * dname)
{
	return -1;
}

static int cpio_mkdir(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	return -1;
}

static int cpio_rmdir(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	return -1;
}

static int cpio_chmod(struct vfs_node_t * n, u32_t mode)
{
	return -1;
}

static struct filesystem_t cpio = {
	.name		= "cpio",

	.mount		= cpio_mount,
	.unmount	= cpio_unmount,
	.msync		= cpio_msync,
	.vget		= cpio_vget,
	.vput		= cpio_vput,

	.read		= cpio_read,
	.write		= cpio_write,
	.truncate	= cpio_truncate,
	.sync		= cpio_sync,
	.readdir	= cpio_readdir,
	.lookup		= cpio_lookup,
	.create		= cpio_create,
	.remove		= cpio_remove,
	.rename		= cpio_rename,
	.mkdir		= cpio_mkdir,
	.rmdir		= cpio_rmdir,
	.chmod		= cpio_chmod,
};

static __init void filesystem_cpio_init(void)
{
	register_filesystem(&cpio);
}

static __exit void filesystem_cpio_exit(void)
{
	unregister_filesystem(&cpio);
}

core_initcall(filesystem_cpio_init);
core_exitcall(filesystem_cpio_exit);
