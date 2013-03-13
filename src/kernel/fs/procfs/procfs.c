/*
 * kernel/fs/procfs/procfs.c
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
#include <malloc.h>
#include <errno.h>
#include <time/xtime.h>
#include <xboot/log.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <fs/fs.h>

extern struct proc_list * proc_list;

/*
 * filesystem operations
 */
static s32_t procfs_mount(struct mount * m, char * dev, s32_t flag)
{
	if(dev != NULL)
		return EINVAL;

	m->m_flags = (flag & MOUNT_MASK) | MOUNT_RDONLY;
	m->m_data = (void *)proc_list;

	return 0;
}

static s32_t procfs_unmount(struct mount * m)
{
	m->m_data = NULL;

	return 0;
}

static s32_t procfs_sync(struct mount * m)
{
	return 0;
}

static s32_t procfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static s32_t procfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static s32_t procfs_open(struct vnode * node, s32_t flag)
{
	struct proc * proc;
	char * path;

	path = node->v_path;
	if(!strcmp(path, "/"))
		return 0;

	if(*path == '/')
		path++;

	proc = proc_search((char *)path);
	if(proc == NULL)
		return -1;

	node->v_data = (void *)proc;

	return 0;
}

static s32_t procfs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static s32_t procfs_read(struct vnode * node, struct file * fp, void * buf, loff_t size, loff_t * result)
{
	struct proc * proc = (struct proc *)(node->v_data);
	loff_t len;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	len = proc->read(buf, fp->f_offset, size);
	fp->f_offset += len;
	*result = len;

	return 0;
}

static s32_t procfs_write(struct vnode * node , struct file * fp, void * buf, loff_t size, loff_t * result)
{
	return -1;
}

static s32_t procfs_seek(struct vnode * node, struct file * fp, loff_t off1, loff_t off2)
{
	return -1;
}

static s32_t procfs_ioctl(struct vnode * node, struct file * fp, int cmd, void * arg)
{
	return -1;
}

static s32_t procfs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static s32_t procfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct proc_list * plist = (struct proc_list *)node->v_mount->m_data;
	struct proc_list * list;
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
		pos = (&plist->entry)->next;
		for(i = 0; i != (fp->f_offset - 2); i++)
		{
			pos = pos->next;
			if(pos == (&plist->entry))
				return EINVAL;
		}

		list = list_entry(pos, struct proc_list, entry);
		dir->d_type = DT_REG;
		strlcpy((char *)&dir->d_name, list->proc->name, sizeof(dir->d_name));
	}

	dir->d_fileno = (u32_t)fp->f_offset;
	dir->d_namlen = (u16_t)strlen(dir->d_name);
	fp->f_offset++;

	return 0;
}

static s32_t procfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	struct proc * proc;

	proc = proc_search(name);
	if(proc == NULL)
		return -1;

	node->v_data = (void *)proc;
	node->v_mode = S_IRUSR | S_IRGRP | S_IROTH;
	node->v_type = VREG;
	node->v_size = 0;

	return 0;
}

static s32_t procfs_create(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t procfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t procfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static s32_t procfs_mkdir(struct vnode * node, char * name, u32_t mode)
{
	return -1;
}

static s32_t procfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static s32_t procfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t procfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static s32_t procfs_inactive(struct vnode * node)
{
	return -1;
}

static s32_t procfs_truncate(struct vnode * node, loff_t length)
{
	return -1;
}

/*
 * procfs vnode operations
 */
static struct vnops procfs_vnops = {
	.vop_open 		= procfs_open,
	.vop_close		= procfs_close,
	.vop_read		= procfs_read,
	.vop_write		= procfs_write,
	.vop_seek		= procfs_seek,
	.vop_ioctl		= procfs_ioctl,
	.vop_fsync		= procfs_fsync,
	.vop_readdir	= procfs_readdir,
	.vop_lookup		= procfs_lookup,
	.vop_create		= procfs_create,
	.vop_remove		= procfs_remove,
	.vop_rename		= procfs_rename,
	.vop_mkdir		= procfs_mkdir,
	.vop_rmdir		= procfs_rmdir,
	.vop_getattr	= procfs_getattr,
	.vop_setattr	= procfs_setattr,
	.vop_inactive	= procfs_inactive,
	.vop_truncate	= procfs_truncate,
};

/*
 * file system operations
 */
static struct vfsops procfs_vfsops = {
	.vfs_mount		= procfs_mount,
	.vfs_unmount	= procfs_unmount,
	.vfs_sync		= procfs_sync,
	.vfs_vget		= procfs_vget,
	.vfs_statfs		= procfs_statfs,
	.vfs_vnops		= &procfs_vnops,
};

/*
 * procfs filesystem
 */
static struct filesystem procfs = {
	.name		= "procfs",
	.vfsops		= &procfs_vfsops,
};

static __init void filesystem_procfs_init(void)
{
	if(!filesystem_register(&procfs))
		LOG_E("register 'procfs' filesystem fail");
}

static __exit void filesystem_procfs_exit(void)
{
	if(!filesystem_unregister(&procfs))
		LOG_E("unregister 'procfs' filesystem fail");
}

fs_initcall(filesystem_procfs_init);
fs_exitcall(filesystem_procfs_exit);
