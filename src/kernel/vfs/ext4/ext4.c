/*
 * kernel/vfs/ext4/ext4.c
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

#include <vfs/ext4/ext4-control.h>
#include <vfs/ext4/ext4-node.h>
#include <vfs/ext4/ext4.h>

int ext4fs_mount(struct vfs_mount_t * m, const char * dev)
{
	int rc;
	u16_t rootmode;
	struct ext4fs_control_t *ctrl;
	struct ext4fs_node_t *root;

	if(dev == NULL)
		return -1;

	ctrl = calloc(1, sizeof(struct ext4fs_control_t));
	if(!ctrl)
	{
		return -1;
	}

	/* Setup control info */
	rc = ext4fs_control_init(ctrl, m->m_dev);
	if(rc)
	{
		goto fail;
	}

	/* Setup root node */
	root = m->m_root->v_data;
	rc = ext4fs_node_init(root);
	if(rc)
	{
		goto fail;
	}
	rc = ext4fs_node_load(ctrl, 2, root);
	if(rc)
	{
		goto fail;
	}

	rootmode = le16_to_cpu(root->inode.mode);

	m->m_root->v_mode = 0;

	switch(rootmode & EXT2_S_IFMASK)
	{
	case EXT2_S_IFSOCK:
		m->m_root->v_type = VNT_SOCK;
		m->m_root->v_mode |= S_IFSOCK;
		break;
	case EXT2_S_IFLNK:
		m->m_root->v_type = VNT_LNK;
		m->m_root->v_mode |= S_IFLNK;
		break;
	case EXT2_S_IFREG:
		m->m_root->v_type = VNT_REG;
		m->m_root->v_mode |= S_IFREG;
		break;
	case EXT2_S_IFBLK:
		m->m_root->v_type = VNT_BLK;
		m->m_root->v_mode |= S_IFBLK;
		break;
	case EXT2_S_IFDIR:
		m->m_root->v_type = VNT_DIR;
		m->m_root->v_mode |= S_IFDIR;
		break;
	case EXT2_S_IFCHR:
		m->m_root->v_type = VNT_CHR;
		m->m_root->v_mode |= S_IFCHR;
		break;
	case EXT2_S_IFIFO:
		m->m_root->v_type = VNT_FIFO;
		m->m_root->v_mode |= S_IFIFO;
		break;
	default:
		m->m_root->v_type = VNT_UNK;
		break;
	};

	m->m_root->v_mode |= (rootmode & EXT2_S_IRUSR) ? S_IRUSR : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IWUSR) ? S_IWUSR : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IXUSR) ? S_IXUSR : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IRGRP) ? S_IRGRP : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IWGRP) ? S_IWGRP : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IXGRP) ? S_IXGRP : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IROTH) ? S_IROTH : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IWOTH) ? S_IWOTH : 0;
	m->m_root->v_mode |= (rootmode & EXT2_S_IXOTH) ? S_IXOTH : 0;

	m->m_root->v_ctime = le32_to_cpu(root->inode.ctime);
	m->m_root->v_atime = le32_to_cpu(root->inode.atime);
	m->m_root->v_mtime = le32_to_cpu(root->inode.mtime);

	m->m_root->v_size = ext4fs_node_get_size(root);

	/* Save control as mount point data */
	m->m_data = ctrl;
	return 0;

fail:
	free(ctrl);
	return rc;
}

static int ext4fs_unmount(struct vfs_mount_t * m)
{
	int rc;
	struct ext4fs_control_t *ctrl = m->m_data;

	if(!ctrl)
	{
		return -1;
	}

	rc = ext4fs_control_exit(ctrl);

	free(ctrl);

	return rc;
}

static int ext4fs_msync(struct vfs_mount_t * m)
{
	struct ext4fs_control_t *ctrl = m->m_data;

	if(!ctrl)
	{
		return -1;
	}

	return ext4fs_control_sync(ctrl);
}

static int ext4fs_vget(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	int rc;
	struct ext4fs_node_t *node;

	node = calloc(1, sizeof(struct ext4fs_node_t));
	if(!node)
	{
		return -1;
	}

	rc = ext4fs_node_init(node);

	n->v_data = node;

	return rc;
}

static int ext4fs_vput(struct vfs_mount_t * m, struct vfs_node_t * n)
{
	int rc;
	struct ext4fs_node_t *node = n->v_data;

	if(!node)
	{
		return -1;
	}

	rc = ext4fs_node_exit(node);

	free(node);

	return rc;
}

/* 
 * Vnode operations 
 */

static u64_t ext4fs_read(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	struct ext4fs_node_t *node = n->v_data;
	u64_t filesize = ext4fs_node_get_size(node);

	if(filesize <= off)
	{
		return 0;
	}

	if(filesize < (len + off))
	{
		len = filesize - off;
	}

	return ext4fs_node_read(node, off, len, buf);
}

static u64_t ext4fs_write(struct vfs_node_t * n, s64_t off, void * buf, u64_t len)
{
	u32_t wlen;
	struct ext4fs_node_t *node = n->v_data;

	wlen = ext4fs_node_write(node, off, len, buf);

	/* Size and mtime might have changed */
	n->v_size = ext4fs_node_get_size(node);
	n->v_mtime = le32_to_cpu(node->inode.mtime);

	return wlen;
}

static int ext4fs_truncate(struct vfs_node_t * n, loff_t off)
{
	int rc;
	struct ext4fs_node_t *node = n->v_data;
	u64_t fileoff = off;
	u64_t filesize = ext4fs_node_get_size(node);

	if(filesize <= fileoff)
	{
		return -1;
	}

	rc = ext4fs_node_truncate(node, fileoff);
	if(rc)
	{
		return rc;
	}

	/* Size and mtime might have changed */
	n->v_size = ext4fs_node_get_size(node);
	n->v_mtime = le32_to_cpu(node->inode.mtime);

	return 0;
}

static int ext4fs_sync(struct vfs_node_t * n)
{
	struct ext4fs_node_t *node = n->v_data;

	if(!node)
	{
		return -1;
	}

	return ext4fs_node_sync(node);
}

static int ext4fs_readdir(struct vfs_node_t * dn, s64_t off, struct vfs_dirent_t * d)
{
	struct ext4fs_node_t *dnode = dn->v_data;

	return ext4fs_node_read_dirent(dnode, off, d);
}

static int ext4fs_lookup(struct vfs_node_t * dn, const char *name, struct vfs_node_t * n)
{
	int rc;
	u16_t filemode;
	struct ext2_dirent_t dent;
	struct ext4fs_node_t *node = n->v_data;
	struct ext4fs_node_t *dnode = dn->v_data;

	rc = ext4fs_node_find_dirent(dnode, name, &dent);
	if(rc)
	{
		return rc;
	}

	rc = ext4fs_node_load(dnode->ctrl, le32_to_cpu(dent.inode), node);
	if(rc)
	{
		return rc;
	}

	filemode = le16_to_cpu(node->inode.mode);

	n->v_mode = 0;

	switch(filemode & EXT2_S_IFMASK)
	{
	case EXT2_S_IFSOCK:
		n->v_type = VNT_SOCK;
		n->v_mode |= S_IFSOCK;
		break;
	case EXT2_S_IFLNK:
		n->v_type = VNT_LNK;
		n->v_mode |= S_IFLNK;
		break;
	case EXT2_S_IFREG:
		n->v_type = VNT_REG;
		n->v_mode |= S_IFREG;
		break;
	case EXT2_S_IFBLK:
		n->v_type = VNT_BLK;
		n->v_mode |= S_IFBLK;
		break;
	case EXT2_S_IFDIR:
		n->v_type = VNT_DIR;
		n->v_mode |= S_IFDIR;
		break;
	case EXT2_S_IFCHR:
		n->v_type = VNT_CHR;
		n->v_mode |= S_IFCHR;
		break;
	case EXT2_S_IFIFO:
		n->v_type = VNT_FIFO;
		n->v_mode |= S_IFIFO;
		break;
	default:
		n->v_type = VNT_UNK;
		break;
	};

	n->v_mode |= (filemode & EXT2_S_IRUSR) ? S_IRUSR : 0;
	n->v_mode |= (filemode & EXT2_S_IWUSR) ? S_IWUSR : 0;
	n->v_mode |= (filemode & EXT2_S_IXUSR) ? S_IXUSR : 0;
	n->v_mode |= (filemode & EXT2_S_IRGRP) ? S_IRGRP : 0;
	n->v_mode |= (filemode & EXT2_S_IWGRP) ? S_IWGRP : 0;
	n->v_mode |= (filemode & EXT2_S_IXGRP) ? S_IXGRP : 0;
	n->v_mode |= (filemode & EXT2_S_IROTH) ? S_IROTH : 0;
	n->v_mode |= (filemode & EXT2_S_IWOTH) ? S_IWOTH : 0;
	n->v_mode |= (filemode & EXT2_S_IXOTH) ? S_IXOTH : 0;

	n->v_ctime = le32_to_cpu(node->inode.ctime);
	n->v_atime = le32_to_cpu(node->inode.atime);
	n->v_mtime = le32_to_cpu(node->inode.mtime);

	n->v_size = ext4fs_node_get_size(node);

	return 0;
}

static int ext4fs_create(struct vfs_node_t * dn, const char *name, u32_t mode)
{
	int rc;
	u16_t filemode;
	u32_t inode_no;
	struct ext2_dirent_t dent;
	struct ext2_inode_t inode;
	struct ext4fs_node_t *dnode = dn->v_data;

	rc = ext4fs_node_find_dirent(dnode, name, &dent);
	if(rc != -1)
	{
		if(!rc)
		{
			return -1;
		}
		else
		{
			return rc;
		}
	}

	rc = ext4fs_control_alloc_inode(dnode->ctrl, dnode->inode_no, &inode_no);
	if(rc)
	{
		return rc;
	}

	memset(&inode, 0, sizeof(inode));
	inode.nlinks = le16_to_cpu(1);

	filemode = EXT2_S_IFREG;
	filemode |= (mode & S_IRUSR) ? EXT2_S_IRUSR : 0;
	filemode |= (mode & S_IWUSR) ? EXT2_S_IWUSR : 0;
	filemode |= (mode & S_IXUSR) ? EXT2_S_IXUSR : 0;
	filemode |= (mode & S_IRGRP) ? EXT2_S_IRGRP : 0;
	filemode |= (mode & S_IWGRP) ? EXT2_S_IWGRP : 0;
	filemode |= (mode & S_IXGRP) ? EXT2_S_IXGRP : 0;
	filemode |= (mode & S_IROTH) ? EXT2_S_IROTH : 0;
	filemode |= (mode & S_IWOTH) ? EXT2_S_IWOTH : 0;
	filemode |= (mode & S_IXOTH) ? EXT2_S_IXOTH : 0;
	inode.mode = le16_to_cpu(filemode);

	inode.mtime = le32_to_cpu(ext4fs_current_timestamp());
	inode.atime = le32_to_cpu(ext4fs_current_timestamp());
	inode.ctime = le32_to_cpu(ext4fs_current_timestamp());

	rc = ext4fs_control_write_inode(dnode->ctrl, inode_no, &inode);
	if(rc)
	{
		ext4fs_control_free_inode(dnode->ctrl, inode_no);
		return rc;
	}

	rc = ext4fs_node_add_dirent(dnode, name, inode_no, 0);
	if(rc)
	{
		ext4fs_control_free_inode(dnode->ctrl, inode_no);
		return rc;
	}

	return 0;
}

static int ext4fs_remove(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	int rc;
	struct ext2_dirent_t dent;
	struct ext4fs_node_t *dnode = dn->v_data;
	struct ext4fs_node_t *node = n->v_data;

	rc = ext4fs_node_find_dirent(dnode, name, &dent);
	if(rc)
	{
		return rc;
	}

	if(le32_to_cpu(dent.inode) != node->inode_no)
	{
		return -1;
	}

	rc = ext4fs_node_del_dirent(dnode, name);
	if(rc)
	{
		return rc;
	}

	rc = ext4fs_control_free_inode(dnode->ctrl, node->inode_no);
	if(rc)
	{
		return rc;
	}

	return 0;
}

static int ext4fs_rename(struct vfs_node_t * sv, const char * sname, struct vfs_node_t * n, struct vfs_node_t * dn, const char * dname)
{
	int rc;
	struct ext2_dirent_t dent;
	struct ext4fs_node_t *snode = sv->v_data;
	struct ext4fs_node_t *dnode = dn->v_data;

	rc = ext4fs_node_find_dirent(dnode, dname, &dent);
	if(rc != -1)
	{
		if(!rc)
		{
			return -1;
		}
		else
		{
			return rc;
		}
	}

	rc = ext4fs_node_find_dirent(snode, sname, &dent);
	if(rc)
	{
		return rc;
	}

	rc = ext4fs_node_del_dirent(snode, sname);
	if(rc)
	{
		return rc;
	}

	rc = ext4fs_node_add_dirent(dnode, dname, le32_to_cpu(dent.inode), 0);
	if(rc)
	{
		return rc;
	}

	return 0;
}

static int ext4fs_mkdir(struct vfs_node_t * dn, const char *name, u32_t mode)
{
	int rc;
	u16_t filemode;
	u32_t i, inode_no, blkno;
	char buf[64];
	struct ext2_dirent_t dent;
	struct ext2_inode_t inode;
	struct ext4fs_node_t *dnode = dn->v_data;
	struct ext4fs_control_t *ctrl = dnode->ctrl;

	rc = ext4fs_node_find_dirent(dnode, name, &dent);
	if(rc != -1)
	{
		if(!rc)
		{
			return -1;
		}
		else
		{
			return rc;
		}
	}

	rc = ext4fs_control_alloc_inode(ctrl, dnode->inode_no, &inode_no);
	if(rc)
	{
		return rc;
	}

	memset(&inode, 0, sizeof(inode));

	inode.nlinks = le16_to_cpu(1);

	filemode = EXT2_S_IFDIR;
	filemode |= (mode & S_IRUSR) ? EXT2_S_IRUSR : 0;
	filemode |= (mode & S_IWUSR) ? EXT2_S_IWUSR : 0;
	filemode |= (mode & S_IXUSR) ? EXT2_S_IXUSR : 0;
	filemode |= (mode & S_IRGRP) ? EXT2_S_IRGRP : 0;
	filemode |= (mode & S_IWGRP) ? EXT2_S_IWGRP : 0;
	filemode |= (mode & S_IXGRP) ? EXT2_S_IXGRP : 0;
	filemode |= (mode & S_IROTH) ? EXT2_S_IROTH : 0;
	filemode |= (mode & S_IWOTH) ? EXT2_S_IWOTH : 0;
	filemode |= (mode & S_IXOTH) ? EXT2_S_IXOTH : 0;
	inode.mode = le16_to_cpu(filemode);

	inode.mtime = le32_to_cpu(ext4fs_current_timestamp());
	inode.atime = le32_to_cpu(ext4fs_current_timestamp());
	inode.ctime = le32_to_cpu(ext4fs_current_timestamp());

	rc = ext4fs_control_alloc_block(ctrl, dnode->inode_no, &blkno);
	if(rc)
	{
		goto failed1;
	}

	memset(buf, 0, sizeof(buf));
	for(i = 0; i < ctrl->block_size; i += sizeof(buf))
	{
		rc = ext4fs_devwrite(ctrl, blkno, i, sizeof(buf), buf);
		if(rc)
		{
			goto failed2;
		}
	}
	i = 0;
	dent.inode = le32_to_cpu(inode_no);
	dent.filetype = 0;
	dent.namelen = 1;
	dent.direntlen = le16_to_cpu(sizeof(dent) + 1);
	memcpy(&buf[i], &dent, sizeof(dent));
	i += sizeof(dent);
	memcpy(&buf[i], ".", 1);
	i += 1;
	dent.inode = le32_to_cpu(dnode->inode_no);
	dent.filetype = 0;
	dent.namelen = 2;
	dent.direntlen = le16_to_cpu(ctrl->block_size - i);
	memcpy(&buf[i], &dent, sizeof(dent));
	i += sizeof(dent);
	memcpy(&buf[i], "..", 2);
	i += 2;

	rc = ext4fs_devwrite(ctrl, blkno, 0, i, buf);
	if(rc)
	{
		goto failed2;
	}

	inode.b.blocks.dir_blocks[0] = le32_to_cpu(blkno);
	inode.size = le32_to_cpu(ctrl->block_size);
	inode.blockcnt = le32_to_cpu(ctrl->block_size >> EXT2_SECTOR_BITS);

	rc = ext4fs_control_write_inode(ctrl, inode_no, &inode);
	if(rc)
	{
		goto failed2;
	}

	rc = ext4fs_node_add_dirent(dnode, name, inode_no, 0);
	if(rc)
	{
		goto failed2;
	}

	return 0;

failed2:
	ext4fs_control_free_block(ctrl, blkno);
failed1:
	ext4fs_control_free_inode(ctrl, inode_no);
	return rc;
}

static int ext4fs_rmdir(struct vfs_node_t * dn, struct vfs_node_t * n, const char *name)
{
	int rc;
	struct ext2_dirent_t dent;
	struct ext4fs_node_t *dnode = dn->v_data;
	struct ext4fs_node_t *node = n->v_data;

	rc = ext4fs_node_find_dirent(dnode, name, &dent);
	if(rc)
	{
		return rc;
	}

	if(le32_to_cpu(dent.inode) != node->inode_no)
	{
		return -1;
	}

	rc = ext4fs_node_truncate(node, 0);
	if(rc)
	{
		return rc;
	}

	rc = ext4fs_node_del_dirent(dnode, name);
	if(rc)
	{
		return rc;
	}

	rc = ext4fs_control_free_inode(dnode->ctrl, node->inode_no);
	if(rc)
	{
		return rc;
	}

	return 0;
}

static int ext4fs_chmod(struct vfs_node_t * n, u32_t mode)
{
	u16_t filemode;
	struct ext4fs_node_t *node = n->v_data;

	filemode = 0;
	switch(n->v_type)
	{
	case VNT_SOCK:
		filemode = EXT2_S_IFSOCK;
		break;
	case VNT_LNK:
		filemode = EXT2_S_IFLNK;
		break;
	case VNT_REG:
		filemode = EXT2_S_IFREG;
		break;
	case VNT_BLK:
		filemode = EXT2_S_IFBLK;
		break;
	case VNT_DIR:
		filemode = EXT2_S_IFDIR;
		break;
	case VNT_CHR:
		filemode = EXT2_S_IFCHR;
		break;
	case VNT_FIFO:
		filemode = EXT2_S_IFIFO;
		break;
	default:
		filemode = 0;
		break;
	};

	filemode |= (mode & S_IRUSR) ? EXT2_S_IRUSR : 0;
	filemode |= (mode & S_IWUSR) ? EXT2_S_IWUSR : 0;
	filemode |= (mode & S_IXUSR) ? EXT2_S_IXUSR : 0;
	filemode |= (mode & S_IRGRP) ? EXT2_S_IRGRP : 0;
	filemode |= (mode & S_IWGRP) ? EXT2_S_IWGRP : 0;
	filemode |= (mode & S_IXGRP) ? EXT2_S_IXGRP : 0;
	filemode |= (mode & S_IROTH) ? EXT2_S_IROTH : 0;
	filemode |= (mode & S_IWOTH) ? EXT2_S_IWOTH : 0;
	filemode |= (mode & S_IXOTH) ? EXT2_S_IXOTH : 0;

	node->inode.mode = le16_to_cpu(filemode);
	node->inode.atime = le32_to_cpu(ext4fs_current_timestamp());
	node->inode_dirty = TRUE;

	n->v_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO);
	n->v_mode |= mode;

	return 0;
}

static struct filesystem_t ext4 = {
	.name		= "ext4",

	.mount		= ext4fs_mount,
	.unmount	= ext4fs_unmount,
	.msync		= ext4fs_msync,
	.vget		= ext4fs_vget,
	.vput		= ext4fs_vput,

	.read		= ext4fs_read,
	.write		= ext4fs_write,
	.truncate	= ext4fs_truncate,
	.sync		= ext4fs_sync,
	.readdir	= ext4fs_readdir,
	.lookup		= ext4fs_lookup,
	.create		= ext4fs_create,
	.remove		= ext4fs_remove,
	.rename		= ext4fs_rename,
	.mkdir		= ext4fs_mkdir,
	.rmdir		= ext4fs_rmdir,
	.chmod		= ext4fs_chmod,
};

static __init void filesystem_ext4_init(void)
{
	register_filesystem(&ext4);
}

static __exit void filesystem_ext4_exit(void)
{
	unregister_filesystem(&ext4);
}

core_initcall(filesystem_ext4_init);
core_exitcall(filesystem_ext4_exit);
