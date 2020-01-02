/*
 * kernel/vfs/ram/ram.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
	struct list_head entry;
	struct list_head children;
	enum vfs_node_type_t type;
	char * name;
	u32_t mode;
	char * buf;
	u64_t buflen;
	u64_t size;
};

static struct ram_node_t * ram_node_alloc(const char * name, enum vfs_node_type_t type)
{
	struct ram_node_t * rn;

	rn = malloc(sizeof(struct ram_node_t));
	if(!rn)
		return NULL;

	rn->name = strdup(name);
	if(!rn->name)
	{
		free(rn);
		return NULL;
	}
	init_list_head(&rn->entry);
	init_list_head(&rn->children);
	rn->type = type;
	rn->mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	rn->buf = NULL;
	rn->buflen = 0;
	rn->size = 0;

	return rn;
}

static void ram_node_free(struct ram_node_t * rn)
{
	if(rn->name)
		free(rn->name);
	if(rn->buf)
	{
		free(rn->buf);
		rn->buf = NULL;
		rn->buflen = 0;
	}
	free(rn);
}

static struct ram_node_t * ram_node_add(struct ram_node_t * rn, const char * name, enum vfs_node_type_t type)
{
	struct ram_node_t * n;

	n = ram_node_alloc(name, type);
	if(!n)
		return NULL;
	list_add_tail(&n->entry, &rn->children);
	return n;
}

static int ram_node_remove(struct ram_node_t * drn, struct ram_node_t * rn)
{
	struct ram_node_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(drn->children), entry)
	{
		if(pos == rn)
		{
			list_del(&pos->entry);
			ram_node_free(pos);
			return 0;
		}
	}
	return -1;
}

static int ramfs_rename_node(struct ram_node_t * rn, const char * name)
{
	if(rn->name)
		free(rn->name);
	rn->name = strdup(name);
	if(!rn->name)
		return -1;
	return 0;
}

static int ram_mount(struct vfs_mount_t * m, const char * dev)
{
	struct ram_node_t * rn;

	if(dev)
		return -1;
	rn = ram_node_alloc("/", VNT_DIR);
	if(!rn)
		return -1;
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
		if(epos > rn->buflen)
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
			rn->buflen = nsize;
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
			rn->buflen = 0;
		}
	}
	else if(off > rn->buflen)
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
		rn->buflen = nsize;
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
	struct ram_node_t * pos, * n;
	struct ram_node_t * drn;
	s64_t i = 0;

	drn = dn->v_data;
	list_for_each_entry_safe(pos, n, &(drn->children), entry)
	{
		if(i++ == off)
		{
			if(pos->type == VNT_DIR)
				d->d_type = VDT_DIR;
			else
				d->d_type = VDT_REG;
			strlcpy(d->d_name, pos->name, sizeof(d->d_name));
			d->d_off = off;
			d->d_reclen = 1;
			return 0;
		}
	}
	return -1;
}

static int ram_lookup(struct vfs_node_t * dn, const char * name, struct vfs_node_t * n)
{
	struct ram_node_t * pos, * tn;
	struct ram_node_t * drn;

	if(!name || (*name == '\0'))
		return -1;

	drn = dn->v_data;
	list_for_each_entry_safe(pos, tn, &(drn->children), entry)
	{
		if(strcmp(name, pos->name) == 0)
		{
			n->v_atime = 0;
			n->v_mtime = 0;
			n->v_ctime = 0;
			n->v_mode = 0;
			if(pos->type == VNT_DIR)
				n->v_mode |= S_IFDIR;
			else
				n->v_mode |= S_IFREG;
			n->v_mode |= pos->mode & (S_IRWXU | S_IRWXG | S_IRWXO);
			n->v_type = pos->type;
			n->v_size = pos->size;
			n->v_data = (void *)pos;
			return 0;
		}
	}
	return -1;
}

static int ram_create(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	struct ram_node_t * rn;

	if(!S_ISREG(mode))
		return -1;
	rn = ram_node_add(dn->v_data, name, VNT_REG);
	if(!rn)
		return -1;
	rn->mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	return 0;
}

static int ram_remove(struct vfs_node_t * dn, struct vfs_node_t * n, const char * name)
{
	return ram_node_remove(dn->v_data, n->v_data);
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
			rn->buflen = orn->buflen;
			rn->size = orn->size;
			orn->buf = NULL;
			orn->buflen = 0;
			orn->size = 0;
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
	rn->mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	rn->size = 0;
	return 0;
}

static int ram_rmdir(struct vfs_node_t * dn, struct vfs_node_t * n, const char * name)
{
	return ram_node_remove(dn->v_data, n->v_data);
}

static int ram_chmod(struct vfs_node_t * n, u32_t mode)
{
	struct ram_node_t * rn;

	rn = n->v_data;
	rn->mode = mode & (S_IRWXU | S_IRWXG | S_IRWXO);
	return 0;
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
