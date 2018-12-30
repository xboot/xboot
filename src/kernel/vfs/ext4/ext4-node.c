/*
 * kernel/vfs/ext4/ext4-node.c
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

#include <vfs/ext4/ext4-control.h>
#include <vfs/ext4/ext4-node.h>

u64_t ext4fs_node_get_size(struct ext4fs_node_t * node)
{
	u64_t ret = le32_to_cpu(node->inode.size);

	if(le32_to_cpu(node->ctrl->sblock.revision_level) != 0)
	{
		ret |= ((u64_t)le32_to_cpu(node->inode.dir_acl)) << 32;
	}
	return ret;
}

void ext4fs_node_set_size(struct ext4fs_node_t * node, u64_t size)
{
	node->inode.size = le32_to_cpu((u32_t )(size & 0xFFFFFFFFULL));
	if(le32_to_cpu(node->ctrl->sblock.revision_level) != 0)
	{
		node->inode.dir_acl = le32_to_cpu((u32_t )(size >> 32));
	}
	node->inode.blockcnt = le32_to_cpu((u32_t)(size >> EXT2_SECTOR_BITS));
	node->inode_dirty = TRUE;
}

int ext4fs_node_read_blk(struct ext4fs_node_t *node, u32_t blkno, u32_t blkoff, u32_t blklen, char *buf)
{
	int rc;
	struct ext4fs_control_t *ctrl = node->ctrl;

	if(blklen > ctrl->block_size)
	{
		return -1;
	}

	/* If the block number is 0 then 
	 * this block is not stored on disk
	 * but is zero filled instead.  
	 */
	if(!blkno)
	{
		memset(buf, 0, blklen);
		return 0;
	}

	if(!node->cached_block)
	{
		node->cached_block = calloc(1, ctrl->block_size);
		if(!node->cached_block)
		{
			return -1;
		}
	}
	if(node->cached_blkno != blkno)
	{
		if(node->cached_dirty)
		{
			rc = ext4fs_devwrite(ctrl, node->cached_blkno, 0, ctrl->block_size, (char *)node->cached_block);
			if(rc)
			{
				return rc;
			}
			node->cached_dirty = FALSE;
		}
		rc = ext4fs_devread(ctrl, blkno, 0, ctrl->block_size, (char *)node->cached_block);
		if(rc)
		{
			return rc;
		}
		node->cached_blkno = blkno;
	}

	memcpy(buf, &node->cached_block[blkoff], blklen);

	return 0;
}

int ext4fs_node_write_blk(struct ext4fs_node_t * node, u32_t blkno, u32_t blkoff, u32_t blklen, char * buf)
{
	int rc;
	struct ext4fs_control_t *ctrl = node->ctrl;

	if(blklen > ctrl->block_size)
	{
		return -1;
	}

	/* We skip writes to block number 0
	 * since its expected to be zero filled.
	 */
	if(!blkno)
	{
		return 0;
	}

	if(!node->cached_block)
	{
		node->cached_block = calloc(1, ctrl->block_size);
		if(!node->cached_block)
		{
			return -1;
		}
	}
	if(node->cached_blkno != blkno)
	{
		if(node->cached_dirty)
		{
			rc = ext4fs_devwrite(ctrl, node->cached_blkno, 0, ctrl->block_size, (char *)node->cached_block);
			if(rc)
			{
				return rc;
			}
			node->cached_dirty = FALSE;
		}
		if(blkoff != 0 || blklen != ctrl->block_size)
		{
			rc = ext4fs_devread(ctrl, blkno, 0, ctrl->block_size, (char *)node->cached_block);
			if(rc)
			{
				return rc;
			}
			node->cached_blkno = blkno;
		}
	}

	memcpy(&node->cached_block[blkoff], buf, blklen);
	node->cached_dirty = TRUE;

	return 0;
}

int ext4fs_node_sync(struct ext4fs_node_t * node)
{
	int rc;
	struct ext4fs_control_t *ctrl = node->ctrl;

	if(node->inode_dirty)
	{
		rc = ext4fs_control_write_inode(ctrl, node->inode_no, &node->inode);
		if(rc)
		{
			return rc;
		}
		node->inode_dirty = FALSE;
	}

	if(node->cached_block && node->cached_dirty)
	{
		rc = ext4fs_devwrite(ctrl, node->cached_blkno, 0, ctrl->block_size, (char *)node->cached_block);
		if(rc)
		{
			return rc;
		}
		node->cached_dirty = FALSE;
	}

	if(node->indir_block && node->indir_dirty)
	{
		rc = ext4fs_devwrite(ctrl, node->indir_blkno, 0, ctrl->block_size, (char *)node->indir_block);
		if(rc)
		{
			return rc;
		}
		node->indir_dirty = FALSE;
	}

	if(node->dindir1_block && node->dindir1_dirty)
	{
		rc = ext4fs_devwrite(ctrl, node->dindir1_blkno, 0, ctrl->block_size, (char *)node->dindir1_block);
		if(rc)
		{
			return rc;
		}
		node->dindir1_dirty = FALSE;
	}

	if(node->dindir2_block && node->dindir2_dirty)
	{
		rc = ext4fs_devwrite(ctrl, node->dindir2_blkno, 0, ctrl->block_size, (char *)node->dindir2_block);
		if(rc)
		{
			return rc;
		}
		node->dindir2_dirty = FALSE;
	}

	return 0;
}

int ext4fs_node_read_blkno(struct ext4fs_node_t * node, u32_t blkpos, u32_t *blkno)
{
	int rc;
	u32_t dindir2_blkno;
	struct ext2_inode_t *inode = &node->inode;
	struct ext4fs_control_t *ctrl = node->ctrl;

	if(blkpos < ctrl->dir_blklast)
	{
		/* Direct blocks.  */
		*blkno = le32_to_cpu(inode->b.blocks.dir_blocks[blkpos]);
	}
	else if(blkpos < ctrl->indir_blklast)
	{
		/* Indirect.  */
		u32_t indir_blkpos = blkpos - ctrl->dir_blklast;

		if(!node->indir_block)
		{
			node->indir_block = malloc(ctrl->block_size);
			if(!node->indir_block)
			{
				return -1;
			}
			rc = ext4fs_devread(ctrl, node->indir_blkno, 0, ctrl->block_size, (char *)node->indir_block);
			if(rc)
			{
				return rc;
			}
		}

		*blkno = le32_to_cpu(node->indir_block[indir_blkpos]);
	}
	else if(blkpos < ctrl->dindir_blklast)
	{
		/* Double indirect.  */
		u32_t t = blkpos - ctrl->indir_blklast;
		u32_t dindir1_blkpos = udiv32(t, ctrl->block_size / 4);
		u32_t dindir2_blkpos = t - dindir1_blkpos * (ctrl->block_size / 4);

		if(!node->dindir1_block)
		{
			node->dindir1_block = malloc(ctrl->block_size);
			if(!node->dindir1_block)
			{
				return -1;
			}
			rc = ext4fs_devread(ctrl, node->dindir1_blkno, 0, ctrl->block_size, (char *)node->dindir1_block);
			if(rc)
			{
				return rc;
			}
		}

		dindir2_blkno = le32_to_cpu(node->dindir1_block[dindir1_blkpos]);

		if(!node->dindir2_block)
		{
			node->dindir2_block = malloc(ctrl->block_size);
			if(!node->dindir2_block)
			{
				return -1;
			}
			node->dindir2_blkno = 0;
		}
		if(dindir2_blkno != node->dindir2_blkno)
		{
			if(node->dindir2_dirty)
			{
				rc = ext4fs_devwrite(ctrl, node->dindir2_blkno, 0, ctrl->block_size, (char *)node->dindir2_block);
				if(rc)
				{
					return rc;
				}
				node->dindir2_dirty = FALSE;
			}
			rc = ext4fs_devread(ctrl, dindir2_blkno, 0, ctrl->block_size, (char *)node->dindir2_block);
			if(rc)
			{
				return rc;
			}
			node->dindir2_blkno = dindir2_blkno;
		}

		*blkno = le32_to_cpu(node->dindir2_block[dindir2_blkpos]);
	}
	else
	{
		/* Tripple indirect.  */
		return -1;
	}

	return 0;
}

int ext4fs_node_write_blkno(struct ext4fs_node_t * node, u32_t blkpos, u32_t blkno)
{
	int rc;
	u32_t dindir2_blkno;
	struct ext2_inode_t *inode = &node->inode;
	struct ext4fs_control_t *ctrl = node->ctrl;

	if(blkpos < ctrl->dir_blklast)
	{
		/* Direct blocks.  */
		inode->b.blocks.dir_blocks[blkpos] = le32_to_cpu(blkno);
		node->inode_dirty = TRUE;
	}
	else if(blkpos < ctrl->indir_blklast)
	{
		/* Indirect.  */
		u32_t indir_blkpos = blkpos - ctrl->dir_blklast;

		if(!node->indir_block)
		{
			node->indir_block = malloc(ctrl->block_size);
			if(!node->indir_block)
			{
				return -1;
			}
			rc = ext4fs_devread(ctrl, node->indir_blkno, 0, ctrl->block_size, (char *)node->indir_block);
			if(rc)
			{
				return rc;
			}
		}

		node->indir_block[indir_blkpos] = le32_to_cpu(blkno);
		node->indir_dirty = TRUE;
	}
	else if(blkpos < ctrl->dindir_blklast)
	{
		/* Double indirect.  */
		u32_t t = blkpos - ctrl->indir_blklast;
		u32_t dindir1_blkpos = udiv32(t, ctrl->block_size / 4);
		u32_t dindir2_blkpos = t - dindir1_blkpos * (ctrl->block_size / 4);

		if(!node->dindir1_block)
		{
			node->dindir1_block = malloc(ctrl->block_size);
			if(!node->dindir1_block)
			{
				return -1;
			}
			rc = ext4fs_devread(ctrl, node->dindir1_blkno, 0, ctrl->block_size, (char *)node->dindir1_block);
			if(rc)
			{
				return rc;
			}
		}

		dindir2_blkno = le32_to_cpu(node->dindir1_block[dindir1_blkpos]);

		if(!node->dindir2_block)
		{
			node->dindir2_block = malloc(ctrl->block_size);
			if(!node->dindir2_block)
			{
				return -1;
			}
			node->dindir2_blkno = 0;
		}
		if(dindir2_blkno != node->dindir2_blkno)
		{
			if(node->dindir2_dirty)
			{
				rc = ext4fs_devwrite(ctrl, node->dindir2_blkno, 0, ctrl->block_size, (char *)node->dindir2_block);
				if(rc)
				{
					return rc;
				}
				node->dindir2_dirty = FALSE;
			}
			if(!dindir2_blkno)
			{
				rc = ext4fs_control_alloc_block(ctrl, node->inode_no, &dindir2_blkno);
				if(rc)
				{
					return rc;
				}
				node->dindir1_block[dindir1_blkpos] = le32_to_cpu(dindir2_blkno);
				node->dindir1_dirty = TRUE;
				memset(node->dindir2_block, 0, ctrl->block_size);
			}
			else
			{
				rc = ext4fs_devread(ctrl, dindir2_blkno, 0, ctrl->block_size, (char *)node->dindir2_block);
				if(rc)
				{
					return rc;
				}
			}
			node->dindir2_blkno = dindir2_blkno;
		}

		node->dindir2_block[dindir2_blkpos] = le32_to_cpu(blkno);
		node->dindir2_dirty = TRUE;
	}
	else
	{
		/* Tripple indirect.  */
		return -1;
	}

	return 0;
}

/* Note: Node position has to be 64-bit */
u32_t ext4fs_node_read(struct ext4fs_node_t * node, u64_t pos, u32_t len, char * buf)
{
	int rc;
	u64_t filesize = ext4fs_node_get_size(node);
	u32_t i, rlen, blkno, blkoff, blklen;
	u32_t last_blkpos, last_blklen;
	u32_t first_blkpos, first_blkoff, first_blklen;
	struct ext4fs_control_t *ctrl = node->ctrl;

	if(filesize <= pos)
	{
		return 0;
	}
	if(filesize < (len + pos))
	{
		len = filesize - pos;
	}

	/* Note: div result < 32-bit */
	first_blkpos = udiv64(pos, ctrl->block_size);
	first_blkoff = pos - (first_blkpos * ctrl->block_size);
	first_blklen = ctrl->block_size - first_blkoff;
	if(len < first_blklen)
	{
		first_blklen = len;
	}

	/* Note: div result < 32-bit */
	last_blkpos = udiv64((len + pos), ctrl->block_size);
	last_blklen = (len + pos) - (last_blkpos * ctrl->block_size);

	rlen = len;
	i = first_blkpos;
	while(rlen)
	{
		rc = ext4fs_node_read_blkno(node, i, &blkno);
		if(rc)
		{
			goto done;
		}

		if(i == first_blkpos)
		{
			/* First block.  */
			blkoff = first_blkoff;
			blklen = first_blklen;
		}
		else if(i == last_blkpos)
		{
			/* Last block.  */
			blkoff = 0;
			blklen = last_blklen;
		}
		else
		{
			/* Middle block. */
			blkoff = 0;
			blklen = ctrl->block_size;
		}

		/* Read cached block */
		rc = ext4fs_node_read_blk(node, blkno, blkoff, blklen, buf);
		if(rc)
		{
			goto done;
		}

		buf += blklen;
		rlen -= blklen;
		i++;
	}

	done: return len - rlen;
}

u32_t ext4fs_node_write(struct ext4fs_node_t * node, u64_t pos, u32_t len, char * buf)
{
	int rc;
	bool_t update_nodesize = FALSE, alloc_newblock = FALSE;
	u32_t wlen, blkpos, blkno, blkoff, blklen;
	u64_t wpos, filesize = ext4fs_node_get_size(node);
	struct ext4fs_control_t *ctrl = node->ctrl;

	wlen = len;
	wpos = pos;
	update_nodesize = FALSE;

	while(wlen)
	{
		/* Note: div result < 32-bit */
		blkpos = udiv64(wpos, ctrl->block_size);
		blkoff = wpos - (blkpos * ctrl->block_size);
		blklen = ctrl->block_size - blkoff;
		blklen = (wlen < blklen) ? wlen : blklen;

		rc = ext4fs_node_read_blkno(node, blkpos, &blkno);
		if(rc)
		{
			goto done;
		}

		if(!blkno)
		{
			rc = ext4fs_control_alloc_block(ctrl, node->inode_no, &blkno);
			if(rc)
			{
				goto done;
			}

			rc = ext4fs_node_write_blkno(node, blkpos, blkno);
			if(rc)
			{
				return rc;
			}

			alloc_newblock = TRUE;
		}
		else
		{
			alloc_newblock = FALSE;
		}

		rc = ext4fs_node_write_blk(node, blkno, blkoff, blklen, buf);
		if(rc)
		{
			if(alloc_newblock)
			{
				ext4fs_control_free_block(ctrl, blkno);
				ext4fs_node_write_blkno(node, blkpos, 0);
			}
			goto done;
		}

		if(wpos >= filesize)
		{
			update_nodesize = TRUE;
		}

		wpos += blklen;
		buf += blklen;
		wlen -= blklen;
		if(update_nodesize)
		{
			filesize += blklen;
		}
	}

	done: if(update_nodesize)
	{
		/* Update node size */
		ext4fs_node_set_size(node, filesize);
	}
	if(len - wlen)
	{
		/* Update node modify time */
		node->inode.mtime = le32_to_cpu(ext4fs_current_timestamp());
		node->inode_dirty = TRUE;
	}

	return len - wlen;
}

int ext4fs_node_truncate(struct ext4fs_node_t * node, u64_t pos)
{
	int rc;
	u32_t blkpos, blkno, blkcnt;
	u32_t first_blkpos, first_blkoff;
	u64_t filesize = ext4fs_node_get_size(node);
	struct ext4fs_control_t *ctrl = node->ctrl;

	if(filesize <= pos)
	{
		return 0;
	}

	/* Note: div result < 32-bit */
	first_blkpos = udiv64(pos, ctrl->block_size);
	first_blkoff = pos - (first_blkpos * ctrl->block_size);

	/* Note: div result < 32-bit */
	blkcnt = udiv64(filesize, ctrl->block_size);
	if(filesize > ((u64_t)blkcnt * (u64_t)ctrl->block_size))
	{
		blkcnt++;
	}

	/* If first block to truncate will have some data left
	 * then do not free first block
	 */
	if(first_blkoff)
	{
		blkpos = first_blkpos + 1;
	}
	else
	{
		blkpos = first_blkpos;
	}

	/* Free node blocks */
	while(blkpos < blkcnt)
	{
		rc = ext4fs_node_read_blkno(node, blkpos, &blkno);
		if(rc)
		{
			return rc;
		}

		rc = ext4fs_control_free_block(ctrl, blkno);
		if(rc)
		{
			return rc;
		}

		rc = ext4fs_node_write_blkno(node, blkpos, 0);
		if(rc)
		{
			return rc;
		}

		blkpos++;
	}

	/* Free indirect & double indirect blocks */
	if(pos != filesize)
	{
		/* Update node mtime */
		node->inode.mtime = le32_to_cpu(ext4fs_current_timestamp());
		node->inode_dirty = TRUE;
		/* Update node size */
		ext4fs_node_set_size(node, pos);
	}

	return 0;
}

int ext4fs_node_load(struct ext4fs_control_t * ctrl, u32_t inode_no, struct ext4fs_node_t * node)
{
	int rc;

	node->ctrl = ctrl;

	node->inode_no = inode_no;
	rc = ext4fs_control_read_inode(ctrl, node->inode_no, &node->inode);
	if(rc)
	{
		return rc;
	}
	node->inode_dirty = FALSE;

	node->cached_block = NULL;
	node->cached_blkno = 0;
	node->cached_dirty = FALSE;

	node->indir_block = NULL;
	node->indir_blkno = le32_to_cpu(node->inode.b.blocks.indir_block);
	node->indir_dirty = FALSE;

	node->dindir1_block = NULL;
	node->dindir1_blkno = le32_to_cpu(node->inode.b.blocks.double_indir_block);
	node->dindir1_dirty = FALSE;

	node->dindir2_block = NULL;
	node->dindir2_blkno = 0;
	node->dindir2_dirty = FALSE;

	return 0;
}

int ext4fs_node_init(struct ext4fs_node_t * node)
{
	int idx;

	node->inode_no = 0;
	node->inode_dirty = FALSE;

	node->cached_block = NULL;
	node->cached_blkno = 0;
	node->cached_dirty = FALSE;

	node->indir_block = NULL;
	node->indir_blkno = 0;
	node->indir_dirty = FALSE;

	node->dindir1_block = NULL;
	node->dindir1_blkno = 0;
	node->dindir1_dirty = FALSE;

	node->dindir2_block = NULL;
	node->dindir2_blkno = 0;
	node->dindir2_dirty = FALSE;

	node->lookup_victim = 0;
	for(idx = 0; idx < EXT4_NODE_LOOKUP_SIZE; idx++)
	{
		node->lookup_name[idx][0] = '\0';
	}

	return 0;
}

int ext4fs_node_exit(struct ext4fs_node_t * node)
{
	if(node->cached_block)
	{
		free(node->cached_block);
	}

	if(node->indir_block)
	{
		free(node->indir_block);
	}

	if(node->dindir1_block)
	{
		free(node->dindir1_block);
	}

	if(node->dindir2_block)
	{
		free(node->dindir2_block);
	}

	return 0;
}

static int ext4fs_node_find_lookup_dirent(struct ext4fs_node_t * dnode, const char * name, struct ext2_dirent_t * dent)
{
	int idx;

	if(name[0] == '\0')
	{
		return -1;
	}

	for(idx = 0; idx < EXT4_NODE_LOOKUP_SIZE; idx++)
	{
		if(!strcmp(dnode->lookup_name[idx], name))
		{
			memcpy(dent, &dnode->lookup_dent[idx], sizeof(*dent));
			return idx;
		}
	}

	return -1;
}

static void ext4fs_node_add_lookup_dirent(struct ext4fs_node_t * dnode, const char * name, struct ext2_dirent_t * dent)
{
	int idx;
	bool_t found = FALSE;

	if(name[0] == '\0')
	{
		return;
	}

	for(idx = 0; idx < EXT4_NODE_LOOKUP_SIZE; idx++)
	{
		if(!strcmp(dnode->lookup_name[idx], name))
		{
			found = TRUE;
			break;
		}
	}

	if(!found)
	{
		idx = dnode->lookup_victim;
		dnode->lookup_victim++;
		if(dnode->lookup_victim == EXT4_NODE_LOOKUP_SIZE)
		{
			dnode->lookup_victim = 0;
		}
		strncpy(&dnode->lookup_name[idx][0], name, VFS_MAX_NAME);
		memcpy(&dnode->lookup_dent[idx], dent, sizeof(*dent));
	}
}

static void ext4fs_node_del_lookup_dirent(struct ext4fs_node_t * dnode, const char * name)
{
	int idx;

	if(name[0] == '\0')
	{
		return;
	}

	for(idx = 0; idx < EXT4_NODE_LOOKUP_SIZE; idx++)
	{
		if(!strcmp(dnode->lookup_name[idx], name))
		{
			dnode->lookup_name[idx][0] = '\0';
			break;
		}
	}

}

int ext4fs_node_read_dirent(struct ext4fs_node_t * dnode, s64_t off, struct vfs_dirent_t * d)
{
	u32_t readlen;
	struct ext2_dirent_t dent;
	u64_t filesize = ext4fs_node_get_size(dnode);
	u64_t fileoff = off;

	if(filesize <= fileoff)
	{
		return -1;
	}

	if(filesize < (sizeof(struct ext2_dirent_t) + fileoff))
	{
		return -1;
	}

	d->d_reclen = 0;

	do
	{
		readlen = ext4fs_node_read(dnode, fileoff, sizeof(struct ext2_dirent_t), (char *)&dent);
		if(readlen != sizeof(struct ext2_dirent_t))
		{
			return -1;
		}

		if(dent.namelen > (VFS_MAX_NAME - 1))
		{
			dent.namelen = (VFS_MAX_NAME - 1);
		}
		readlen = ext4fs_node_read(dnode, fileoff + sizeof(struct ext2_dirent_t), dent.namelen, d->d_name);
		if(readlen != dent.namelen)
		{
			return -1;
		}
		d->d_name[dent.namelen] = '\0';

		d->d_reclen += le16_to_cpu(dent.direntlen);
		fileoff += le16_to_cpu(dent.direntlen);

		if((strcmp(d->d_name, ".") == 0) || (strcmp(d->d_name, "..") == 0))
		{
			continue;
		}
		else
		{
			break;
		}
	}while(1);

	d->d_off = off;

	switch(dent.filetype)
	{
	case EXT2_FT_REG_FILE:
		d->d_type = VDT_REG;
		break;
	case EXT2_FT_DIR:
		d->d_type = VDT_DIR;
		break;
	case EXT2_FT_CHRDEV:
		d->d_type = VDT_CHR;
		break;
	case EXT2_FT_BLKDEV:
		d->d_type = VDT_BLK;
		break;
	case EXT2_FT_FIFO:
		d->d_type = VDT_FIFO;
		break;
	case EXT2_FT_SOCK:
		d->d_type = VDT_SOCK;
		break;
	case EXT2_FT_SYMLINK:
		d->d_type = VDT_LNK;
		break;
	default:
		d->d_type = VDT_UNK;
		break;
	};

	/* Add dent to lookup table */
	ext4fs_node_add_lookup_dirent(dnode, d->d_name, &dent);

	return 0;
}

int ext4fs_node_find_dirent(struct ext4fs_node_t * dnode, const char * name, struct ext2_dirent_t * dent)
{
	bool_t found;
	u32_t rlen;
	char filename[VFS_MAX_NAME];
	u64_t off, filesize = ext4fs_node_get_size(dnode);

	/* Try to find in lookup table */
	if(ext4fs_node_find_lookup_dirent(dnode, name, dent) > -1)
	{
		return 0;
	}

	/* Find desired directoy entry such that we ignore
	 * "." and ".." in search process
	 */
	off = 0;
	found = FALSE;
	while(off < filesize)
	{
		rlen = ext4fs_node_read(dnode, off, sizeof(struct ext2_dirent_t), (char *)dent);
		if(rlen != sizeof(struct ext2_dirent_t))
		{
			return -1;
		}

		if(dent->namelen > (VFS_MAX_NAME - 1))
		{
			dent->namelen = (VFS_MAX_NAME - 1);
		}
		rlen = ext4fs_node_read(dnode, off + sizeof(struct ext2_dirent_t), dent->namelen, filename);
		if(rlen != dent->namelen)
		{
			return -1;
		}
		filename[dent->namelen] = '\0';

		if((strcmp(filename, ".") != 0) && (strcmp(filename, "..") != 0))
		{
			if(strcmp(filename, name) == 0)
			{
				found = TRUE;
				break;
			}
		}

		off += le16_to_cpu(dent->direntlen);
	}

	if(!found)
	{
		return -1;
	}

	/* Add dent to lookup table */
	ext4fs_node_add_lookup_dirent(dnode, filename, dent);

	return 0;
}

int ext4fs_node_add_dirent(struct ext4fs_node_t * dnode, const char * name, u32_t inode_no, u8_t type)
{
	bool_t found;
	u16_t direntlen;
	u32_t rlen, wlen;
	char filename[VFS_MAX_NAME];
	struct ext2_dirent_t dent;
	struct ext4fs_control_t *ctrl = dnode->ctrl;
	u64_t off, filesize = ext4fs_node_get_size(dnode);

	/* Sanity check */
	if(!strcmp(name, ".") || !strcmp(name, ".."))
	{
		return -1;
	}

	/* Compute size of directory entry required */
	direntlen = sizeof(struct ext2_dirent_t) + strlen(name);

	/* Find directory entry to split */
	off = 0;
	found = FALSE;
	while(off < filesize)
	{
		rlen = ext4fs_node_read(dnode, off, sizeof(struct ext2_dirent_t), (char *)&dent);
		if(rlen != sizeof(struct ext2_dirent_t))
		{
			return -1;
		}

		if(direntlen < (le16_to_cpu(dent.direntlen) - dent.namelen - sizeof(struct ext2_dirent_t)))
		{
			found = TRUE;
			break;
		}

		off += le16_to_cpu(dent.direntlen);
	}

	if(!found)
	{
		/* Add space at end of directory to make space for
		 * new directory entry
		 */
		if((off != filesize) || umod64(filesize, ctrl->block_size))
		{
			/* Sum of length of all directory enteries 
			 * should be equal to directory filesize.
			 */
			/* Directory filesize should always be
			 * multiple of block size.
			 */
			return -1;
		}

		memset(filename, 0, VFS_MAX_NAME);
		for(rlen = 0; rlen < ctrl->block_size; rlen += VFS_MAX_NAME)
		{
			wlen = ext4fs_node_write(dnode, off + rlen,
			VFS_MAX_NAME, (char *)filename);
			if(wlen != VFS_MAX_NAME)
			{
				return -1;
			}
		}

		direntlen = ctrl->block_size;
	}
	else
	{
		/* Split existing directory entry to make space for 
		 * new directory entry
		 */
		direntlen = (le16_to_cpu(dent.direntlen) - dent.namelen - sizeof(struct ext2_dirent_t));
		dent.direntlen = le16_to_cpu(le16_to_cpu(dent.direntlen) - direntlen);

		wlen = ext4fs_node_write(dnode, off, sizeof(struct ext2_dirent_t), (char *)&dent);
		if(wlen != sizeof(struct ext2_dirent_t))
		{
			return -1;
		}

		off += le16_to_cpu(dent.direntlen);
	}

	/* Add new entry at given offset and of given length */
	strncpy(filename, name, VFS_MAX_NAME);
	filename[VFS_MAX_NAME - 1] = '\0';

	dent.inode = le32_to_cpu(inode_no);
	dent.direntlen = le16_to_cpu(direntlen);
	dent.namelen = strlen(filename);
	dent.filetype = type;

	wlen = ext4fs_node_write(dnode, off, sizeof(struct ext2_dirent_t), (char *)&dent);
	if(wlen != sizeof(struct ext2_dirent_t))
	{
		return -1;
	}

	off += sizeof(struct ext2_dirent_t);

	wlen = ext4fs_node_write(dnode, off, strlen(filename), (char *)filename);
	if(wlen != strlen(filename))
	{
		return -1;
	}

	/* Increment nlinks field of inode */
	dnode->inode.nlinks = le16_to_cpu(le16_to_cpu(dnode->inode.nlinks) + 1);
	dnode->inode_dirty = TRUE;

	return 0;
}

int ext4fs_node_del_dirent(struct ext4fs_node_t * dnode, const char * name)
{
	bool_t found;
	u32_t rlen, wlen;
	char filename[VFS_MAX_NAME];
	struct ext2_dirent_t pdent, dent;
	u64_t poff, off, filesize = ext4fs_node_get_size(dnode);

	/* Sanity check */
	if(!strcmp(name, ".") || !strcmp(name, ".."))
	{
		return -1;
	}

	/* Delete dent from lookup table */
	ext4fs_node_del_lookup_dirent(dnode, name);

	/* Initialize perivous entry and previous offset */
	poff = 0;
	memset(&pdent, 0, sizeof(pdent));

	/* Find the directory entry and previous entry */
	off = 0;
	found = FALSE;
	while(off < filesize)
	{
		rlen = ext4fs_node_read(dnode, off, sizeof(struct ext2_dirent_t), (char *)&dent);
		if(rlen != sizeof(struct ext2_dirent_t))
		{
			return -1;
		}

		if(dent.namelen > (VFS_MAX_NAME - 1))
		{
			dent.namelen = (VFS_MAX_NAME - 1);
		}
		rlen = ext4fs_node_read(dnode, off + sizeof(struct ext2_dirent_t), dent.namelen, filename);
		if(rlen != dent.namelen)
		{
			return -1;
		}
		filename[dent.namelen] = '\0';

		if((strcmp(filename, ".") != 0) && (strcmp(filename, "..") != 0))
		{
			if(strcmp(filename, name) == 0)
			{
				found = TRUE;
				break;
			}
		}

		poff = off;
		memcpy(&pdent, &dent, sizeof(pdent));

		off += le16_to_cpu(dent.direntlen);
	}

	if(!found || !poff)
	{
		return -1;
	}

	/* Stretch previous directory entry to delete directory entry */
	/* Handle overflow in below 16-bit addition */
	pdent.direntlen = le16_to_cpu(le16_to_cpu(pdent.direntlen) + le16_to_cpu(dent.direntlen));
	wlen = ext4fs_node_write(dnode, poff, sizeof(struct ext2_dirent_t), (char *)&pdent);
	if(wlen != sizeof(struct ext2_dirent_t))
	{
		return -1;
	}

	/* Decrement nlinks field of inode */
	dnode->inode.nlinks = le16_to_cpu(le16_to_cpu(dnode->inode.nlinks) - 1);
	dnode->inode_dirty = TRUE;

	return 0;
}
