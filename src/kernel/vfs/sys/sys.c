/*
 * kernel/vfs/sys/sys.c
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

static int sys_mount(struct vfs_mount_t * m, const char * dev, u32_t flags)
{
	if(dev)
		return -1;

	m->m_flags = flags & MOUNT_MASK;
	m->m_root->v_data = (void *)kobj_get_root();
	m->m_data = NULL;
	return 0;
}

static int sys_unmount(struct vfs_mount_t * m)
{
	m->m_data = NULL;
	return 0;
}

static int sys_msync(struct vfs_mount_t * m)
{
	return 0;
}

static int sys_vget(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static int sys_vput(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	return 0;
}

static u64_t sys_read(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	struct kobj_t * kobj;

	if(n->v_type != VNT_REG)
		return 0;

	kobj = n->v_data;
	if(off == 0)
	{
		if(kobj && kobj->read)
			return kobj->read(kobj, buf, len);
	}
	return 0;
}

static u64_t sys_write(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	struct kobj_t * kobj;

	if(n->v_type != VNT_REG)
		return 0;

	kobj = n->v_data;
	if(off == 0)
	{
		if(kobj && kobj->write)
			return kobj->write(kobj, buf, len);
	}
	return 0;
}

static int sys_truncate(struct vfs_node_t * n, s64_t off)
{
	return -1;
}

static int sys_sync(struct vfs_node_t * n)
{
	return 0;
}

static int sys_readdir(struct vfs_node_t * dn, s64_t off, struct vfs_dirent_t * d)
{
	struct kobj_t * kobj, * obj;
	struct list_head * pos;
	int i;

	kobj = dn->v_data;
	if(list_empty(&kobj->children))
		return -1;

	pos = (&kobj->children)->next;
	for(i = 0; i != off; i++)
	{
		pos = pos->next;
		if(pos == (&kobj->children))
			return -1;
	}

	obj = list_entry(pos, struct kobj_t, entry);
	if(obj->type == KOBJ_TYPE_DIR)
		d->d_type = VDT_DIR;
	else
		d->d_type = VDT_REG;
	strlcpy(d->d_name, obj->name, sizeof(d->d_name));
	d->d_off = off;
	d->d_reclen = 1;

	return 0;
}

static int sys_lookup(struct vfs_node_t * dn, const char * name, struct vfs_node_t * n)
{
	struct kobj_t * kobj, * obj;

	if(*name == '\0')
		return -1;

	kobj = dn->v_data;
	obj = kobj_search(kobj, name);
	if(!obj)
		return -1;

	n->v_atime = 0;
	n->v_mtime = 0;
	n->v_ctime = 0;
	n->v_mode = 0;
	n->v_size = 0;
	n->v_data = (void *)obj;

	if(obj->type == KOBJ_TYPE_DIR)
	{
		n->v_type = VNT_DIR;
		n->v_mode |= S_IFDIR;
		n->v_mode |= S_IRWXU | S_IRWXG | S_IRWXO;
	}
	else
	{
		n->v_type = VNT_REG;
		n->v_mode |= S_IFREG;
		if(obj->read)
			n->v_mode |= (S_IRUSR | S_IRGRP | S_IROTH);
		if(obj->write)
			n->v_mode |= (S_IWUSR | S_IWGRP | S_IWOTH);
	}
	return 0;
}

static int sys_create(struct vfs_node_t * dn, const char * filename, u32_t mode)
{
	return -1;
}

static int sys_remove(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	return -1;
}

static int sys_rename(struct vfs_node_t * sn, const char * sname, struct vfs_node_t * n, struct vfs_node_t * dn, const char * dname)
{
	return -1;
}

static int sys_mkdir(struct vfs_node_t * dn, const char * name, u32_t mode)
{
	return -1;
}

static int sys_rmdir(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	return -1;
}

static int sys_chmod(struct vfs_node_t * n, u32_t mode)
{
	return -1;
}

static struct filesystem_t sys = {
	.name		= "sys",

	.mount		= sys_mount,
	.unmount	= sys_unmount,
	.msync		= sys_msync,
	.vget		= sys_vget,
	.vput		= sys_vput,

	.read		= sys_read,
	.write		= sys_write,
	.truncate	= sys_truncate,
	.sync		= sys_sync,
	.readdir	= sys_readdir,
	.lookup		= sys_lookup,
	.create		= sys_create,
	.remove		= sys_remove,
	.rename		= sys_rename,
	.mkdir		= sys_mkdir,
	.rmdir		= sys_rmdir,
	.chmod		= sys_chmod,
};

static __init void filesystem_sys_init(void)
{
	register_filesystem(&sys);
}

static __exit void filesystem_sys_exit(void)
{
	unregister_filesystem(&sys);
}

core_initcall(filesystem_sys_init);
core_exitcall(filesystem_sys_exit);
