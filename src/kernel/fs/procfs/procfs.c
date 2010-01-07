/*
 * kernel/fs/procfs/procfs.c
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <debug.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <error.h>
#include <time/xtime.h>
#include <shell/readline.h>
#include <xboot/proc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/scank.h>
#include <fs/fs.h>

extern struct proc_list * proc_list;

/*
 * filesystem operations
 */
static x_s32 procfs_mount(struct mount * m, char * dev, x_s32 flag, void * data)
{
	m->m_flags |= MOUNT_RDONLY | MOUNT_NODEV;
	m->m_data = (void *)proc_list;

	return 0;
}

static x_s32 procfs_unmount(struct mount * m)
{
	m->m_data = 0;
	return 0;
}

static x_s32 procfs_sync(struct mount * m)
{
	return 0;
}

static x_s32 procfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static x_s32 procfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static x_s32 procfs_open(struct vnode * node, x_s32 flag)
{
	struct proc * proc;
	x_s8 * path;

	path = (x_s8 *)node->v_path;
	if(!strcmp(path, (const x_s8 *)"/"))
		return 0;

	if(*path == '/')
		path++;

	proc = proc_search((char *)path);
	if(proc == NULL)
		return -1;

	node->v_data = (void *)proc;

	return 0;
}

static x_s32 procfs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 procfs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
{
	struct proc * proc = (struct proc *)(node->v_data);
	x_size len;

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

static x_s32 procfs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	return -1;
}

static x_s32 procfs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	return -1;
}

static x_s32 procfs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 procfs_fsync(struct vnode * node, struct file * fp)
{
	return -1;
}

static x_s32 procfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct proc_list * list;
	struct list_head * pos;
	x_s32 i;

	if(fp->f_offset == 0)
	{
		dir->d_type = DT_DIR;
		strlcpy((x_s8 *)&dir->d_name, (const x_s8 *)".", sizeof(dir->d_name));
	}
	else if(fp->f_offset == 1)
	{
		dir->d_type = DT_DIR;
		strlcpy((x_s8 *)&dir->d_name, (const x_s8 *)"..", sizeof(dir->d_name));
	}
	else
	{
		pos = (&proc_list->entry)->next;
		for(i = 0; i != (fp->f_offset - 2); i++)
		{
			pos = pos->next;
			if(pos == (&proc_list->entry))
				return EINVAL;
		}

		list = list_entry(pos, struct proc_list, entry);
		dir->d_type = DT_REG;
		strlcpy((x_s8 *)&dir->d_name, (const x_s8 *)list->proc->name, sizeof(dir->d_name));
	}

	dir->d_fileno = (x_u32)fp->f_offset;
	dir->d_namlen = (x_u16)strlen((const x_s8 *)dir->d_name);
	fp->f_offset++;

	return 0;
}

static x_s32 procfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
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

static x_s32 procfs_create(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 procfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 procfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	return -1;
}

static x_s32 procfs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	return -1;
}

static x_s32 procfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return -1;
}

static x_s32 procfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 procfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 procfs_inactive(struct vnode * node)
{
	return -1;
}

static x_s32 procfs_truncate(struct vnode * node, x_off length)
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
		DEBUG_E("register 'procfs' filesystem fail");
}

static __exit void filesystem_procfs_exit(void)
{
	if(!filesystem_unregister(&procfs))
		DEBUG_E("unregister 'procfs' filesystem fail");
}

module_init(filesystem_procfs_init, LEVEL_POSTCORE);
module_exit(filesystem_procfs_exit, LEVEL_POSTCORE);
