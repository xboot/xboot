/*
 * kernel/fs/sysfs/sysfs.c
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
#include <fs/fs.h>

static s32_t sysfs_mount(struct mount_t * m, char * dev, s32_t flag)
{
	if(dev != NULL)
		return EINVAL;

	m->m_flags = flag & MOUNT_MASK;
	m->m_root->v_data = (void *)kobj_get_root();
	m->m_data = NULL;

	return 0;
}

static s32_t sysfs_unmount(struct mount_t * m)
{
	m->m_data = NULL;

	return 0;
}

static s32_t sysfs_sync(struct mount_t * m)
{
	return 0;
}

static s32_t sysfs_vget(struct mount_t * m, struct vnode_t * node)
{
	return 0;
}

static s32_t sysfs_statfs(struct mount_t * m, struct statfs * stat)
{
	return -1;
}

static s32_t sysfs_open(struct vnode_t * node, s32_t flag)
{
	return 0;
}

static s32_t sysfs_close(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t sysfs_read(struct vnode_t * node, struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	struct kobj_t * kobj;
	loff_t len = 0;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	kobj = node->v_data;
	if(fp->f_offset == 0)
	{
		if(kobj && kobj->read)
			len = kobj->read(kobj, buf, size);
	}
	fp->f_offset += len;
	*result = len;

	return 0;
}

static s32_t sysfs_write(struct vnode_t * node , struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	struct kobj_t * kobj;
	loff_t len = 0;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	kobj = node->v_data;
	if(fp->f_offset == 0)
	{
		if(kobj && kobj->write)
			len = kobj->write(kobj, buf, size);
	}
	fp->f_offset += len;
	*result = len;

	return 0;
}

static s32_t sysfs_seek(struct vnode_t * node, struct file_t * fp, loff_t off1, loff_t off2)
{
	return -1;
}

static s32_t sysfs_ioctl(struct vnode_t * node, struct file_t * fp, int cmd, void * arg)
{
	return -1;
}

static s32_t sysfs_fsync(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t sysfs_readdir(struct vnode_t * node, struct file_t * fp, struct dirent_t * dir)
{
	struct kobj_t * kobj, * obj;
	struct list_head * pos;
	s32_t i;

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
		kobj = node->v_data;
		if(list_empty(&kobj->children))
			return ENOENT;

		pos = (&kobj->children)->next;
		for(i = 0; i != (fp->f_offset - 2); i++)
		{
			pos = pos->next;
			if(pos == (&kobj->children))
				return EINVAL;
		}

		obj = list_entry(pos, struct kobj_t, entry);
		if(obj->type == KOBJ_TYPE_DIR)
			dir->d_type = DT_DIR;
		else
			dir->d_type = DT_REG;
		strlcpy((char *)&dir->d_name, obj->name, sizeof(dir->d_name));
	}

	dir->d_fileno = (u32_t)fp->f_offset;
	dir->d_namlen = (u16_t)strlen(dir->d_name);
	fp->f_offset++;

	return 0;
}

static s32_t sysfs_lookup(struct vnode_t * dnode, char * name, struct vnode_t * node)
{
	struct kobj_t * kobj, * obj;

	if(*name == '\0')
		return ENOENT;

	kobj = dnode->v_data;
	obj = kobj_search(kobj, name);
	if(!obj)
		return ENOENT;

	node->v_data = (void *)obj;
	if(obj->type == KOBJ_TYPE_DIR)
	{
		node->v_mode = S_IRWXU | S_IRWXG | S_IRWXO;
		node->v_type = VDIR;
		node->v_size = 0;
	}
	else
	{
		node->v_mode = 0;
		if(obj->read)
			node->v_mode |= (S_IRUSR | S_IRGRP | S_IROTH);
		if(obj->write)
			node->v_mode |= (S_IWUSR | S_IWGRP | S_IWOTH);
		node->v_type = VREG;
		node->v_size = 0;
	}

	return 0;
}

static s32_t sysfs_create(struct vnode_t * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t sysfs_remove(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	return -1;
}

static s32_t sysfs_rename(struct vnode_t * dnode1, struct vnode_t * node1, char * name1, struct vnode_t *dnode2, struct vnode_t * node2, char * name2)
{
	return -1;
}

static s32_t sysfs_mkdir(struct vnode_t * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t sysfs_rmdir(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	return -1;
}

static s32_t sysfs_getattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t sysfs_setattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t sysfs_inactive(struct vnode_t * node)
{
	return -1;
}

static s32_t sysfs_truncate(struct vnode_t * node, loff_t length)
{
	return -1;
}

static struct vnops_t sysfs_vnops = {
	.vop_open 		= sysfs_open,
	.vop_close		= sysfs_close,
	.vop_read		= sysfs_read,
	.vop_write		= sysfs_write,
	.vop_seek		= sysfs_seek,
	.vop_ioctl		= sysfs_ioctl,
	.vop_fsync		= sysfs_fsync,
	.vop_readdir	= sysfs_readdir,
	.vop_lookup		= sysfs_lookup,
	.vop_create		= sysfs_create,
	.vop_remove		= sysfs_remove,
	.vop_rename		= sysfs_rename,
	.vop_mkdir		= sysfs_mkdir,
	.vop_rmdir		= sysfs_rmdir,
	.vop_getattr	= sysfs_getattr,
	.vop_setattr	= sysfs_setattr,
	.vop_inactive	= sysfs_inactive,
	.vop_truncate	= sysfs_truncate,
};

static struct vfsops_t sysfs_vfsops = {
	.vfs_mount		= sysfs_mount,
	.vfs_unmount	= sysfs_unmount,
	.vfs_sync		= sysfs_sync,
	.vfs_vget		= sysfs_vget,
	.vfs_statfs		= sysfs_statfs,
	.vfs_vnops		= &sysfs_vnops,
};

static struct filesystem_t sysfs = {
	.name		= "sysfs",
	.vfsops		= &sysfs_vfsops,
};

static __init void filesystem_sysfs_init(void)
{
	filesystem_register(&sysfs);
}

static __exit void filesystem_sysfs_exit(void)
{
	filesystem_unregister(&sysfs);
}

core_initcall(filesystem_sysfs_init);
core_exitcall(filesystem_sysfs_exit);
