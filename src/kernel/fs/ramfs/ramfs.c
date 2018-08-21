/*
 * kernel/fs/ramfs/ramfs.c
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
#include <types.h>
#include <stdarg.h>
#include <malloc.h>
#include <errno.h>
#include <xboot/initcall.h>
#include <fs/fs.h>


#define RAMFS_PAGE_SIZE_MASK		(0x00000fff)

/*
 * file/directory node for ramfs
 */
struct ramfs_node {
	struct ramfs_node * next;		/* next node in the same directory */
	struct ramfs_node * child;		/* first child node */
	enum vnode_type_t type;			/* file or directory */
	u32_t mode;						/* file mode permissions */
	s8_t * name;					/* name (null-terminated) */
	s32_t name_len;					/* length of name not including terminator */
	s8_t * buf;						/* buffer to the file data */
	loff_t buf_len;					/* allocated buffer size */
	loff_t size;					/* file size */
};

static struct ramfs_node * ramfs_allocate_node(char * name, enum vnode_type_t type)
{
	struct ramfs_node * node;

	node = malloc(sizeof(struct ramfs_node));
	if(node == NULL)
		return NULL;
	memset(node, 0, sizeof(struct ramfs_node));

	node->name_len = strlen(name);
	node->name = malloc(node->name_len + 1);
	if(node->name == NULL)
	{
		free(node);
		return NULL;
	}
	strlcpy((char *)node->name, name, node->name_len + 1);
	node->type = type;
	return node;
}

static void ramfs_free_node(struct ramfs_node * node)
{
	free(node->name);
	free(node);
}

static struct ramfs_node * ramfs_add_node(struct ramfs_node * node, char * name, enum vnode_type_t type)
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

static s32_t ramfs_remove_node(struct ramfs_node * dnode, struct ramfs_node * node)
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

static s32_t ramfs_rename_node(struct ramfs_node * node, char * name)
{
	loff_t len;
	char * tmp;

	len = strlen(name);
	tmp = malloc(len + 1);
	if(tmp == NULL)
		return ENOMEM;
	strlcpy(tmp, name, len + 1);
	free(node->name);
	node->name = (s8_t *)tmp;
	node->name_len = len;

	return 0;
}

/*
 * filesystem operations
 */
static s32_t ramfs_mount(struct mount_t * m, char * dev, s32_t flag)
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

static s32_t ramfs_unmount(struct mount_t * m)
{
	ramfs_free_node(m->m_root->v_data);
	m->m_data = NULL;

	return 0;
}

static s32_t ramfs_sync(struct mount_t * m)
{
	return 0;
}

static s32_t ramfs_vget(struct mount_t * m, struct vnode_t * node)
{
	return 0;
}

static s32_t ramfs_statfs(struct mount_t * m, struct statfs * stat)
{
	return -1;
}

/*
 * vnode operations
 */
static s32_t ramfs_open(struct vnode_t * node, s32_t flag)
{
	return 0;
}

static s32_t ramfs_close(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t ramfs_read(struct vnode_t * node, struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	struct ramfs_node * n;
	loff_t off;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	off = fp->f_offset;
	if(off >= (loff_t)node->v_size)
		return 0;

	if(node->v_size - off < size)
		size = node->v_size - off;

	n = node->v_data;
	memcpy(buf, n->buf + off, size);

	fp->f_offset += size;
	*result = size;

	return 0;
}

static s32_t ramfs_write(struct vnode_t * node , struct file_t * fp, void * buf, loff_t size, loff_t * result)
{
	struct ramfs_node * n;
	loff_t file_pos, end_pos;
	void * new_buf;
	loff_t new_size;

	*result = 0;
	if(node->v_type == VDIR)
		return EISDIR;
	if(node->v_type != VREG)
		return EINVAL;

	n = node->v_data;

	/* check if the file position exceeds the end of file. */
	end_pos = node->v_size;
	file_pos = (fp->f_flags & O_APPEND) ? end_pos : fp->f_offset;

	if(file_pos + size > (loff_t)end_pos)
	{
		/* expand the file size before writing to it */
		end_pos = file_pos + size;
		if(end_pos > (loff_t)n->buf_len)
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

static s32_t ramfs_seek(struct vnode_t * node, struct file_t * fp, loff_t off1, loff_t off2)
{
	if(off2 > (loff_t)(node->v_size))
		return -1;

	return 0;
}

static s32_t ramfs_ioctl(struct vnode_t * node, struct file_t * fp, int cmd, void * arg)
{
	return -1;
}

static s32_t ramfs_fsync(struct vnode_t * node, struct file_t * fp)
{
	return 0;
}

static s32_t ramfs_readdir(struct vnode_t * node, struct file_t * fp, struct dirent_t * dir)
{
	struct ramfs_node *n, *dn;
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

		strlcpy((char *)&dir->d_name, (char *)n->name, sizeof(dir->d_name));
	}

	dir->d_fileno = (u32_t)fp->f_offset;
	dir->d_namlen = (u16_t)strlen(dir->d_name);

	fp->f_offset++;

	return 0;
}

static s32_t ramfs_lookup(struct vnode_t * dnode, char * name, struct vnode_t * node)
{
	struct ramfs_node *n, *dn;
	loff_t len;
	bool_t found;

	if(*name == '\0')
		return ENOENT;

	len = strlen(name);
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

static s32_t ramfs_create(struct vnode_t * node, char * name, u32_t mode)
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

static s32_t ramfs_remove(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	struct ramfs_node * n;
	s32_t error;

	error = ramfs_remove_node(dnode->v_data, node->v_data);
	if(error != 0)
		return error;

	n = node->v_data;
	if(n->buf != NULL)
		free(n->buf);
	return 0;
}

static s32_t ramfs_rename(struct vnode_t * dnode1, struct vnode_t * node1, char * name1, struct vnode_t *dnode2, struct vnode_t * node2, char * name2)
{
	struct ramfs_node *n, *old_n;
	s32_t error;

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

static s32_t ramfs_mkdir(struct vnode_t * node, char * name, u32_t mode)
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

static s32_t ramfs_rmdir(struct vnode_t * dnode, struct vnode_t * node, char * name)
{
	return ramfs_remove_node(dnode->v_data, node->v_data);
}

static s32_t ramfs_getattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t ramfs_setattr(struct vnode_t * node, struct vattr_t * attr)
{
	return -1;
}

static s32_t ramfs_inactive(struct vnode_t * node)
{
	return 0;
}

static s32_t ramfs_truncate(struct vnode_t * node, loff_t length)
{
	struct ramfs_node *n;
	void * new_buf;
	loff_t new_size;

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
static struct vnops_t ramfs_vnops = {
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
static struct vfsops_t ramfs_vfsops = {
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
static struct filesystem_t ramfs = {
	.name		= "ramfs",
	.vfsops		= &ramfs_vfsops,
};

static __init void filesystem_ramfs_init(void)
{
	filesystem_register(&ramfs);
}

static __exit void filesystem_ramfs_exit(void)
{
	filesystem_unregister(&ramfs);
}

core_initcall(filesystem_ramfs_init);
core_exitcall(filesystem_ramfs_exit);
