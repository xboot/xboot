/*
 * kernel/vfs/ram/ram.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

struct ram_node_t {
	struct ram_node_t * next;
	struct ram_node_t * child;
	enum vfs_node_type_t type;
	u32_t mode;
	char * name;
	int name_len;
	char * buf;
	u64_t buf_len;
	u64_t size;
};

static struct ram_node_t * ram_node_alloc(const char * name, enum vfs_node_type_t type)
{
	struct ram_node_t * rn;

	rn = malloc(sizeof(struct ram_node_t));
	if(!rn)
		return NULL;
	memset(rn, 0, sizeof(struct ram_node_t));

	rn->name_len = strlen(name);
	rn->name = malloc(rn->name_len + 1);
	if(!rn->name)
	{
		free(rn);
		return NULL;
	}
	strlcpy(rn->name, name, rn->name_len + 1);
	rn->type = type;
	return rn;
}

static void ram_node_free(struct ram_node_t * rn)
{
	free(rn->name);
	free(rn);
}

static struct ram_node_t * ram_node_add(struct ram_node_t * rn, const char * name, enum vfs_node_type_t type)
{
	struct ram_node_t * n, * prev;

	n = ram_node_alloc(name, type);
	if(!n)
		return NULL;

	if(rn->child == NULL)
	{
		rn->child = n;
	}
	else
	{
		prev = rn->child;
		while(prev->next != NULL)
			prev = prev->next;
		prev->next = n;
	}
	return n;
}

static int ram_node_remove(struct ram_node_t * drn, struct ram_node_t * rn)
{
	struct ram_node_t * prev;

	if(drn->child == NULL)
		return -1;

	if(drn->child == rn)
	{
		drn->child = rn->next;
	}
	else
	{
		for(prev = drn->child; prev->next != rn; prev = prev->next)
		{
			if(prev->next == NULL)
			{
				return -1;
			}
		}
		prev->next = rn->next;
	}
	ram_node_free(rn);

	return 0;
}

static int ramfs_rename_node(struct ram_node_t * rn, const char * name)
{
	char * tmp;
	int len;

	len = strlen(name);
	tmp = malloc(len + 1);
	if(!tmp)
		return -1;
	strlcpy(tmp, name, len + 1);
	free(rn->name);
	rn->name = tmp;
	rn->name_len = len;

	return 0;
}

static int ram_mount(struct vfs_mount_t * m, const char * dev, u32_t flags)
{
	struct ram_node_t * rn;

	if(dev)
		return -1;

	rn = ram_node_alloc("/", VNT_DIR);
	if(!rn)
		return -1;

	m->m_flags = flags & MOUNT_MASK;
	m->m_root->v_data = (void *)rn;
	m->m_data = NULL;

	return 0;
}

static int ram_unmount(struct vfs_mount_t * m)
{
	ram_node_free(m->m_root->v_data);
	m->m_data = NULL;
	return 0;
}

static int ram_msync(struct vfs_mount_t * m)
{
	return 0;
}

static int ram_vget(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static int ram_vput(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static u64_t ram_read(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	struct ram_node_t * rn;
	u64_t sz;

	if(n->v_type != VNT_REG)
		return 0;

	if(off >= n->v_size)
		return 0;

	sz = len;
	if((n->v_size - off) < sz)
		sz = n->v_size - off;

	rn = n->v_data;
	memcpy(buf, rn->buf + off, sz);

	return sz;
}

static u64_t ram_write(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	struct ram_node_t * rn;
	void * nbuf;
	u64_t nsize;
	u64_t epos;

	if(n->v_type != VNT_REG)
		return 0;

	rn = n->v_data;
	epos = n->v_size;

	if(off + len > epos)
	{
		epos = off + len;
		if(epos > rn->buf_len)
		{
			nsize = (epos + 0xfff) & ~0xfff;
			nbuf = malloc(nsize);
			if(!nbuf)
				return -1;
			if(rn->size != 0)
			{
				memcpy(nbuf, rn->buf, n->v_size);
				free(rn->buf);
			}
			rn->buf = nbuf;
			rn->buf_len = nsize;
		}
		rn->size = epos;
		n->v_size = epos;
	}
	memcpy(rn->buf + off, buf, len);

	return len;
}

static int ram_truncate(struct vfs_node_t * n, s64_t off)
{
	struct ram_node_t * rn;
	void * nbuf;
	u64_t nsize;

	rn = n->v_data;

	if(off == 0)
	{
		if(rn->buf != NULL)
		{
			free(rn->buf);
			rn->buf = NULL;
			rn->buf_len = 0;
		}
	}
	else if(off > rn->buf_len)
	{
		nsize = (off + 0xfff) & ~0xfff;
		nbuf = malloc(nsize);
		if(!nbuf)
			return -1;
		if(rn->size != 0)
		{
			memcpy(nbuf, rn->buf, n->v_size);
			free(rn->buf);
		}
		rn->buf = nbuf;
		rn->buf_len = nsize;
	}
	rn->size = off;
	n->v_size = off;

	return 0;
}

static int ram_sync(struct vfs_node_t * n)
{
	return 0;
}

static int ram_readdir(struct vfs_node_t * dn, s64_t off, struct vfs_dirent_t * d)
{
	struct ram_node_t * drn, * rn;
	int i;

	drn = dn->v_data;
	rn = drn->child;
	if(!rn)
		return -1;

	for(i = 0; i != off; i++)
	{
		rn = rn->next;
		if(!rn)
			return -1;
	}
	if(rn->type == VNT_DIR)
		d->d_type = VDT_DIR;
	else
		d->d_type = VDT_REG;
	strlcpy(d->d_name, rn->name, sizeof(d->d_name));
	d->d_off = off;
	d->d_reclen = 1;

	return 0;
}

static int ram_lookup(struct vfs_node_t * dn, const char * name, struct vfs_node_t * n)
{
	struct ram_node_t * drn, * rn;
	int len;
	int found = 0;

	if(*name == '\0')
		return -1;

	len = strlen(name);
	drn = dn->v_data;

	for(rn = drn ? drn->child : NULL; rn != NULL; rn = rn->next)
	{
		if((rn->name_len == len) && (memcmp(name, rn->name, len) == 0))
		{
			found = 1;
			break;
		}
	}
	if(found == 0)
		return -1;

	n->v_atime = 0;
	n->v_mtime = 0;
	n->v_ctime = 0;

	n->v_mode = 0;
	if(rn->type == VNT_DIR)
		n->v_mode |= S_IFDIR;
	else
		n->v_mode |= S_IFREG;
	n->v_mode |= rn->mode & (S_IRWXU | S_IRWXG | S_IRWXO);

	n->v_type = rn->type;
	n->v_size = rn->size;
	n->v_data = (void *)rn;

	return 0;
}

static int ram_create(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	struct ram_node_t * rn;

	if(!S_ISREG(mode))
		return -1;

	rn = ram_node_add(dn->v_data, name, VNT_REG);
	if(!rn)
		return -1;

	rn->mode = mode & (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	return 0;
}

static int ram_remove(struct vfs_node_t * dn, struct vfs_node_t * n, const char * name)
{
	struct ram_node_t * rn;

	if(ram_node_remove(dn->v_data, n->v_data) < 0)
		return -1;

	rn = n->v_data;
	if(rn->buf)
		free(rn->buf);
	return 0;
}

static int ram_rename(struct vfs_node_t * sn, const char * sname, struct vfs_node_t * n, struct vfs_node_t * dn, const char * dname)
{
	struct ram_node_t * rn, * orn;

	if(sn == dn)
	{
		if(ramfs_rename_node(n->v_data, dname) < 0)
			return -1;
	}
	else
	{
		orn = n->v_data;
		rn = ram_node_add(dn->v_data, dname, VNT_REG);
		if(!rn)
			return -1;

		if(n->v_type == VNT_REG)
		{
			rn->buf = orn->buf;
			rn->size = orn->size;
			rn->buf_len = orn->buf_len;
		}
		ram_node_remove(sn->v_data, n->v_data);
	}
	return 0;
}

static int ram_mkdir(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	struct ram_node_t * rn;

	if(!S_ISDIR(mode))
		return -1;

	rn = ram_node_add(dn->v_data, name, VNT_DIR);
	if(!rn)
		return -1;

	rn->mode = mode & (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	rn->size = 0;
	return 0;
}

static int ram_rmdir(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	return ram_node_remove(dn->v_data, n->v_data);
}

static int ram_chmod(struct vfs_node_t * n, u32_t mode)
{
	return -1;
}

static struct filesystem_t ram = {
	.name		= "ram",

	.mount		= ram_mount,
	.unmount	= ram_unmount,
	.msync		= ram_msync,
	.vget		= ram_vget,
	.vput		= ram_vput,

	.read		= ram_read,
	.write		= ram_write,
	.truncate	= ram_truncate,
	.sync		= ram_sync,
	.readdir	= ram_readdir,
	.lookup		= ram_lookup,
	.create		= ram_create,
	.remove		= ram_remove,
	.rename		= ram_rename,
	.mkdir		= ram_mkdir,
	.rmdir		= ram_rmdir,
	.chmod		= ram_chmod,
};

static __init void filesystem_ram_init(void)
{
	register_filesystem(&ram);
}

static __exit void filesystem_ram_exit(void)
{
	unregister_filesystem(&ram);
}

core_initcall(filesystem_ram_init);
core_exitcall(filesystem_ram_exit);
