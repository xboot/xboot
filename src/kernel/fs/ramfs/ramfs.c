/*
 * kernel/fs/ramfs/ramfs.c
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
#include <stdarg.h>
#include <vsprintf.h>
#include <malloc.h>
#include <error.h>
#include <time/xtime.h>
#include <shell/readline.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/scank.h>
#include <fs/fs.h>


#define RAMFS_PAGE_SIZE_MASK		(0x00000fff)

/*
 * file/directory node for ramfs
 */
struct ramfs_node {
	struct ramfs_node * next;		/* next node in the same directory */
	struct ramfs_node * child;		/* first child node */
	enum vnode_type type;			/* file or directory */
	x_u32 mode;						/* file mode permissions */
	x_s8 * name;					/* name (null-terminated) */
	x_s32 name_len;					/* length of name not including terminator */
	x_s8 * buf;						/* buffer to the file data */
	x_size buf_len;					/* allocated buffer size */
	x_size size;					/* file size */
};

static struct ramfs_node * ramfs_allocate_node(char * name, enum vnode_type type)
{
	struct ramfs_node * node;

	node = malloc(sizeof(struct ramfs_node));
	if(node == NULL)
		return NULL;
	memset(node, 0, sizeof(struct ramfs_node));

	node->name_len = strlen((const x_s8 *)name);
	node->name = malloc(node->name_len + 1);
	if(node->name == NULL)
	{
		free(node);
		return NULL;
	}
	strlcpy(node->name, (const x_s8 *)name, node->name_len + 1);
	node->type = type;
	return node;
}

static void ramfs_free_node(struct ramfs_node * node)
{
	free(node->name);
	free(node);
}

static struct ramfs_node * ramfs_add_node(struct ramfs_node * node, char * name, enum vnode_type type)
{
	struct ramfs_node *n, *prev;

	n = ramfs_allocate_node(name, type);
	if(n == NULL)
		return NULL;

	/* link to the directory list */
	if(node->child == NULL)
	{
		node->child = n;
	}
	else
	{
		prev = node->child;
		while(prev->next != NULL)
			prev = prev->next;
		prev->next = n;
	}

	return n;
}

static x_s32 ramfs_remove_node(struct ramfs_node * dnode, struct ramfs_node * node)
{
	struct ramfs_node * prev;

	if(dnode->child == NULL)
		return EBUSY;

	/* unlink from the directory list */
	if(dnode->child == node)
	{
		dnode->child = node->next;
	}
	else
	{
		for(prev = dnode->child; prev->next != node; prev = prev->next)
		{
			if(prev->next == NULL)
			{
				return ENOENT;
			}
		}
		prev->next = node->next;
	}

	ramfs_free_node(node);

	return 0;
}

static x_s32 ramfs_rename_node(struct ramfs_node * node, char * name)
{
	x_size len;
	x_s8 * tmp;

	len = strlen((const x_s8 *)name);
	tmp = malloc(len + 1);
	if(tmp == NULL)
		return ENOMEM;
	strlcpy(tmp, (const x_s8 *)name, len + 1);
	free(node->name);
	node->name = tmp;
	node->name_len = len;

	return 0;
}

/*
 * filesystem operations
 */
static x_s32 ramfs_mount(struct mount * m, char * dev, x_s32 flag)
{
	struct ramfs_node * node;

	if(dev != NULL)
		return EINVAL;

	/* create a root node */
	node = ramfs_allocate_node("/", VDIR);
	if(node == NULL)
		return ENOMEM;

	m->m_flags = flag & MOUNT_MASK;
	m->m_root->v_data = node;
	m->m_data = NULL;

	return 0;
}

static x_s32 ramfs_unmount(struct mount * m)
{
	ramfs_free_node(m->m_root->v_data);
	m->m_data = NULL;

	return 0;
}

static x_s32 ramfs_sync(struct mount * m)
{
	return 0;
}

static x_s32 ramfs_vget(struct mount * m, struct vnode * node)
{
	return 0;
}

static x_s32 ramfs_statfs(struct mount * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static x_s32 ramfs_open(struct vnode * node, x_s32 flag)
{
	return 0;
}

static x_s32 ramfs_close(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 ramfs_read(struct vnode * node, struct file * fp, void * buf, x_size size, x_size * result)
{
	struct ramfs_node * n;
	x_off off;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	off = fp->f_offset;
	if(off >= (x_off)node->v_size)
		return 0;

	if(node->v_size - off < size)
		size = node->v_size - off;

	n = node->v_data;
	memcpy(buf, n->buf + off, size);

	fp->f_offset += size;
	*result = size;

	return 0;
}

static x_s32 ramfs_write(struct vnode * node , struct file * fp, void * buf, x_size size, x_size * result)
{
	struct ramfs_node * n;
	x_off file_pos, end_pos;
	void * new_buf;
	x_size new_size;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	n = node->v_data;

	/* check if the file position exceeds the end of file. */
	end_pos = node->v_size;
	file_pos = (fp->f_flags & O_APPEND) ? end_pos : fp->f_offset;

	if(file_pos + size > (x_size)end_pos)
	{
		/* expand the file size before writing to it */
		end_pos = file_pos + size;
		if(end_pos > (x_off)n->buf_len)
		{
			/* round page */
			new_size = (((end_pos) + RAMFS_PAGE_SIZE_MASK) & ~RAMFS_PAGE_SIZE_MASK);
			new_buf = malloc(new_size);
			if(new_buf == NULL)
				return ENOMEM;
			if(n->size != 0)
			{
				memcpy(new_buf, n->buf, node->v_size);
				free(n->buf);
			}
			n->buf = new_buf;
			n->buf_len = new_size;
		}
		n->size = end_pos;
		node->v_size = end_pos;
	}

	memcpy(n->buf + file_pos, buf, size);
	fp->f_offset += size;
	*result = size;

	return 0;
}

static x_s32 ramfs_seek(struct vnode * node, struct file * fp, x_off off1, x_off off2)
{
	return 0;
}

static x_s32 ramfs_ioctl(struct vnode * node, struct file * fp, x_u32 cmd, void * arg)
{
	return -1;
}

static x_s32 ramfs_fsync(struct vnode * node, struct file * fp)
{
	return 0;
}

static x_s32 ramfs_readdir(struct vnode * node, struct file * fp, struct dirent * dir)
{
	struct ramfs_node *n, *dn;
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
		dn = node->v_data;
		n = dn->child;
		if(n == NULL)
			return ENOENT;

		for(i = 0; i != (fp->f_offset - 2); i++)
		{
			n = n->next;
			if(n == NULL)
				return ENOENT;
		}
		if(n->type == VDIR)
			dir->d_type = DT_DIR;
		else
			dir->d_type = DT_REG;

		strlcpy((x_s8 *)&dir->d_name, n->name, sizeof(dir->d_name));
	}

	dir->d_fileno = (x_u32)fp->f_offset;
	dir->d_namlen = (x_u16)strlen((const x_s8 *)dir->d_name);

	fp->f_offset++;

	return 0;
}

static x_s32 ramfs_lookup(struct vnode * dnode, char * name, struct vnode * node)
{
	struct ramfs_node *n, *dn;
	x_size len;
	x_bool found;

	if(*name == '\0')
		return ENOENT;

	len = strlen((const x_s8 *)name);
	dn = dnode->v_data;
	found = FALSE;
	for(n = dn->child; n != NULL; n = n->next)
	{
		if(n->name_len == len && memcmp(name, n->name, len) == 0)
		{
			found = TRUE;
			break;
		}
	}

	if(found == FALSE)
		return ENOENT;

	node->v_data = n;
	node->v_mode = n->mode & (S_IRWXU|S_IRWXG|S_IRWXO);
	node->v_type = n->type;
	node->v_size = n->size;

	return 0;
}

static x_s32 ramfs_create(struct vnode * node, char * name, x_u32 mode)
{
	struct ramfs_node * n;

	if(!S_ISREG(mode))
		return EINVAL;

	n = ramfs_add_node(node->v_data, name, VREG);
	if(n == NULL)
		return ENOMEM;

	n->mode = mode & (S_IRWXU|S_IRWXG|S_IRWXO);

	return 0;
}

static x_s32 ramfs_remove(struct vnode * dnode, struct vnode * node, char * name)
{
	struct ramfs_node * n;
	x_s32 error;

	error = ramfs_remove_node(dnode->v_data, node->v_data);
	if(error != 0)
		return error;

	n = node->v_data;
	if(n->buf != NULL)
		free(n->buf);
	return 0;
}

static x_s32 ramfs_rename(struct vnode * dnode1, struct vnode * node1, char * name1, struct vnode *dnode2, struct vnode * node2, char * name2)
{
	struct ramfs_node *n, *old_n;
	x_s32 error;

	if(node2)
	{
		/* remove destination file, first */
		error = ramfs_remove_node(dnode2->v_data, node2->v_data);
		if(error != 0)
			return error;
	}

	/* same directory */
	if(dnode1 == dnode2)
	{
		/* change the name of existing file */
		error = ramfs_rename_node(node1->v_data, name2);
		if(error)
			return error;
	}
	else
	{
		/* create new file or directory */
		old_n = node1->v_data;
		n = ramfs_add_node(dnode2->v_data, name2, VREG);
		if(n == NULL)
			return ENOMEM;

		if(node1->v_type == VREG)
		{
			/* copy file data */
			n->buf = old_n->buf;
			n->size = old_n->size;
			n->buf_len = old_n->buf_len;
		}

		/* remove source file */
		ramfs_remove_node(dnode1->v_data, node1->v_data);
	}

	return 0;
}

static x_s32 ramfs_mkdir(struct vnode * node, char * name, x_u32 mode)
{
	struct ramfs_node *n;

	if(!S_ISDIR(mode))
		return EINVAL;

	n = ramfs_add_node(node->v_data, name, VDIR);
	if(n == NULL)
		return ENOMEM;

	n->mode = mode & (S_IRWXU|S_IRWXG|S_IRWXO);
	n->size = 0;

	return 0;
}

static x_s32 ramfs_rmdir(struct vnode * dnode, struct vnode * node, char * name)
{
	return ramfs_remove_node(dnode->v_data, node->v_data);
}

static x_s32 ramfs_getattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 ramfs_setattr(struct vnode * node, struct vattr * attr)
{
	return -1;
}

static x_s32 ramfs_inactive(struct vnode * node)
{
	return 0;
}

static x_s32 ramfs_truncate(struct vnode * node, x_off length)
{
	struct ramfs_node *n;
	void * new_buf;
	x_size new_size;

	n = node->v_data;

	if(length == 0)
	{
		if(n->buf != NULL)
		{
			free(n->buf);
			n->buf = NULL;
			n->buf_len = 0;
		}
	}
	else if(length > n->buf_len)
	{
		/* round page */
		new_size = (((length) + RAMFS_PAGE_SIZE_MASK) & ~RAMFS_PAGE_SIZE_MASK);
		new_buf = malloc(new_size);
		if(new_buf == NULL)
			return ENOMEM;
		if(n->size != 0)
		{
			memcpy(new_buf, n->buf, node->v_size);
			free(n->buf);
		}
		n->buf = new_buf;
		n->buf_len = new_size;
	}
	n->size = length;
	node->v_size = length;

	return 0;
}

/*
 * ramfs vnode operations
 */
static struct vnops ramfs_vnops = {
	.vop_open 		= ramfs_open,
	.vop_close		= ramfs_close,
	.vop_read		= ramfs_read,
	.vop_write		= ramfs_write,
	.vop_seek		= ramfs_seek,
	.vop_ioctl		= ramfs_ioctl,
	.vop_fsync		= ramfs_fsync,
	.vop_readdir	= ramfs_readdir,
	.vop_lookup		= ramfs_lookup,
	.vop_create		= ramfs_create,
	.vop_remove		= ramfs_remove,
	.vop_rename		= ramfs_rename,
	.vop_mkdir		= ramfs_mkdir,
	.vop_rmdir		= ramfs_rmdir,
	.vop_getattr	= ramfs_getattr,
	.vop_setattr	= ramfs_setattr,
	.vop_inactive	= ramfs_inactive,
	.vop_truncate	= ramfs_truncate,
};

/*
 * file system operations
 */
static struct vfsops ramfs_vfsops = {
	.vfs_mount		= ramfs_mount,
	.vfs_unmount	= ramfs_unmount,
	.vfs_sync		= ramfs_sync,
	.vfs_vget		= ramfs_vget,
	.vfs_statfs		= ramfs_statfs,
	.vfs_vnops		= &ramfs_vnops,
};

/*
 * ramfs filesystem
 */
static struct filesystem ramfs = {
	.name		= "ramfs",
	.vfsops		= &ramfs_vfsops,
};

static __init void filesystem_ramfs_init(void)
{
	if(!filesystem_register(&ramfs))
		LOG_E("register 'ramfs' filesystem fail");
}

static __exit void filesystem_ramfs_exit(void)
{
	if(!filesystem_unregister(&ramfs))
		LOG_E("unregister 'ramfs' filesystem fail");
}

module_init(filesystem_ramfs_init, LEVEL_POSTCORE);
module_exit(filesystem_ramfs_exit, LEVEL_POSTCORE);
