/*
 * kernel/vfs/ext4/ext4-control.c
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

#include <vfs/ext4/ext4-control.h>

u32_t ext4fs_current_timestamp(void)
{
	struct timeval tv;

	gettimeofday(&tv, 0);
	return (u32_t)tv.tv_sec;
}

int ext4fs_devread(struct ext4fs_control_t * ctrl, u32_t blkno, u32_t blkoff, u32_t buf_len, char * buf)
{
	u64_t off, len;

	off = ((u64_t)blkno << (ctrl->log2_block_size + EXT2_SECTOR_BITS));
	off += blkoff;
	len = buf_len;
	len = block_read(ctrl->bdev, (u8_t *)buf, off, len);

	return (len == buf_len) ? 0 : -1;
}

int ext4fs_devwrite(struct ext4fs_control_t * ctrl, u32_t blkno, u32_t blkoff, u32_t buf_len, char * buf)
{
	u64_t off, len;

	off = ((u64_t)blkno << (ctrl->log2_block_size + EXT2_SECTOR_BITS));
	off += blkoff;
	len = buf_len;
	len = block_write(ctrl->bdev, (u8_t *)buf, off, len);

	return (len == buf_len) ? 0 : -1;
}

int ext4fs_control_read_inode(struct ext4fs_control_t * ctrl, u32_t inode_no, struct ext2_inode_t * inode)
{
	int rc;
	u32_t g, blkno, blkoff;
	struct ext4fs_group_t *group;

	/* inodes are addressed from 1 onwards */
	inode_no--;

	/* determine block group */
	g = udiv32(inode_no, le32_to_cpu(ctrl->sblock.inodes_per_group));
	if(g >= ctrl->group_count)
	{
		return -1;
	}
	group = &ctrl->groups[g];

	blkno = umod32(inode_no, le32_to_cpu(ctrl->sblock.inodes_per_group));
	blkno = udiv32(blkno, ctrl->inodes_per_block);
	blkno += le32_to_cpu(group->grp.inode_table_id);
	blkoff = umod32(inode_no, ctrl->inodes_per_block) * ctrl->inode_size;

	/* read the inode.  */
	rc = ext4fs_devread(ctrl, blkno, blkoff, sizeof(struct ext2_inode_t), (char *)inode);
	if(rc)
	{
		return rc;
	}

	return 0;
}

int ext4fs_control_write_inode(struct ext4fs_control_t * ctrl, u32_t inode_no, struct ext2_inode_t * inode)
{
	int rc;
	u32_t g, blkno, blkoff;
	struct ext4fs_group_t * group;

	/* inodes are addressed from 1 onwards */
	inode_no--;

	/* determine block group */
	g = udiv32(inode_no, le32_to_cpu(ctrl->sblock.inodes_per_group));
	if(g >= ctrl->group_count)
	{
		return -1;
	}
	group = &ctrl->groups[g];

	blkno = umod32(inode_no, le32_to_cpu(ctrl->sblock.inodes_per_group));
	blkno = udiv32(blkno, ctrl->inodes_per_block);
	blkno += le32_to_cpu(group->grp.inode_table_id);
	blkoff = umod32(inode_no, ctrl->inodes_per_block) * ctrl->inode_size;

	/* write the inode.  */
	rc = ext4fs_devwrite(ctrl, blkno, blkoff, sizeof(struct ext2_inode_t), (char *)inode);
	if(rc)
	{
		return rc;
	}

	return 0;
}

int ext4fs_control_alloc_block(struct ext4fs_control_t * ctrl, u32_t inode_no, u32_t * blkno)
{
	bool_t found;
	u32_t g, group_count, b, blocks_per_group;
	struct ext4fs_group_t *group;

	/* inodes are addressed from 1 onwards */
	inode_no--;

	/* alloc free indoe from a block group */
	blocks_per_group = le32_to_cpu(ctrl->sblock.blocks_per_group);
	g = udiv32(inode_no, le32_to_cpu(ctrl->sblock.inodes_per_group));
	if(g >= ctrl->group_count)
	{
		return -1;
	}
	found = FALSE;
	group_count = ctrl->group_count;
	group = NULL;
	while(group_count)
	{
		group = &ctrl->groups[g];

		mutex_lock(&group->grp_lock);
		if(le16_to_cpu(group->grp.free_blocks))
		{
			for(b = 0; b < blocks_per_group; b++)
			{
				if(group->block_bmap[b >> 3] & (1 << (b & 0x7)))
				{
					continue;
				}
				break;
			}
			if(b >= blocks_per_group)
			{
				mutex_unlock(&group->grp_lock);
				goto next_group;
			}
			group->grp.free_blocks = le16_to_cpu((le16_to_cpu(group->grp.free_blocks) - 1));
			group->block_bmap[b >> 3] |= (1 << (b & 0x7));
			group->grp_dirty = TRUE;
			found = TRUE;
			*blkno = b + g * blocks_per_group + le32_to_cpu(ctrl->sblock.first_data_block);
		}
		mutex_unlock(&group->grp_lock);

		if(found)
		{
			break;
		}

		next_group: g++;
		if(g >= ctrl->group_count)
		{
			g = 0;
		}
		group_count--;
	}
	if(!found)
	{
		return -1;
	}

	/* update superblock */
	mutex_lock(&ctrl->sblock_lock);
	ctrl->sblock.free_blocks = le32_to_cpu((le32_to_cpu(ctrl->sblock.free_blocks) - 1));
	ctrl->sblock_dirty = TRUE;
	mutex_unlock(&ctrl->sblock_lock);

	return 0;
}

int ext4fs_control_free_block(struct ext4fs_control_t * ctrl, u32_t blkno)
{
	u32_t g, b;
	struct ext4fs_group_t * group;

	/* blocks are address from 0 onwards */
	/* For 1KB block size, block group 0 starts at block 1 */
	/* For greater than 1KB block size, block group 0 starts at block 0 */
	blkno = blkno - le32_to_cpu(ctrl->sblock.first_data_block);

	/* determine block group */
	g = udiv32(blkno, le32_to_cpu(ctrl->sblock.blocks_per_group));
	if(g >= ctrl->group_count)
	{
		return -1;
	}
	group = &ctrl->groups[g];

	/* update superblock */
	mutex_lock(&ctrl->sblock_lock);
	ctrl->sblock.free_blocks = le32_to_cpu((le32_to_cpu(ctrl->sblock.free_blocks) + 1));
	ctrl->sblock_dirty = TRUE;
	mutex_unlock(&ctrl->sblock_lock);

	/* update block group descriptor and block group bitmap */
	mutex_lock(&group->grp_lock);
	group->grp.free_blocks = le16_to_cpu((le16_to_cpu(group->grp.free_blocks) + 1));
	b = umod32(blkno, le32_to_cpu(ctrl->sblock.blocks_per_group));
	group->block_bmap[b >> 3] &= ~(1 << (b & 0x7));
	group->grp_dirty = TRUE;
	mutex_unlock(&group->grp_lock);

	return 0;
}

int ext4fs_control_alloc_inode(struct ext4fs_control_t * ctrl, u32_t parent_inode_no, u32_t * inode_no)
{
	bool_t found;
	u32_t g, group_count, i, inodes_per_group;
	struct ext4fs_group_t *group;

	/* inodes are addressed from 1 onwards */
	parent_inode_no--;

	/* alloc free inode from a block group */
	inodes_per_group = le32_to_cpu(ctrl->sblock.inodes_per_group);
	g = udiv32(parent_inode_no, inodes_per_group);
	if(g >= ctrl->group_count)
	{
		return -1;
	}
	found = FALSE;
	group = NULL;
	group_count = ctrl->group_count;
	while(group_count)
	{
		group = &ctrl->groups[g];

		mutex_lock(&group->grp_lock);
		if(le16_to_cpu(group->grp.free_inodes))
		{
			for(i = 0; i < inodes_per_group; i++)
			{
				if(group->inode_bmap[i >> 3] & (1 << (i & 0x7)))
				{
					continue;
				}
				break;
			}
			if(i >= inodes_per_group)
			{
				mutex_unlock(&group->grp_lock);
				goto next_group;
			}
			group->grp.free_inodes = le16_to_cpu((le16_to_cpu(group->grp.free_inodes) - 1));
			group->inode_bmap[i >> 3] |= (1 << (i & 0x7));
			group->grp_dirty = TRUE;
			found = TRUE;
			*inode_no = i + g * inodes_per_group + 1;
		}
		mutex_unlock(&group->grp_lock);

		if(found)
		{
			break;
		}

		next_group: g++;
		if(g >= ctrl->group_count)
		{
			g = 0;
		}
		group_count--;
	}
	if(!found)
	{
		return -1;
	}

	/* update superblock */
	mutex_lock(&ctrl->sblock_lock);
	ctrl->sblock.free_inodes = le32_to_cpu((le32_to_cpu(ctrl->sblock.free_inodes) - 1));
	ctrl->sblock_dirty = TRUE;
	mutex_unlock(&ctrl->sblock_lock);

	return 0;
}

int ext4fs_control_free_inode(struct ext4fs_control_t * ctrl, u32_t inode_no)
{
	u32_t g, i;
	struct ext4fs_group_t * group;

	/* inodes are addressed from 1 onwards */
	inode_no--;

	/* determine block group */
	g = udiv32(inode_no, le32_to_cpu(ctrl->sblock.inodes_per_group));
	if(g >= ctrl->group_count)
	{
		return -1;
	}
	group = &ctrl->groups[g];

	/* update superblock */
	mutex_lock(&ctrl->sblock_lock);
	ctrl->sblock.free_inodes = le32_to_cpu((le32_to_cpu(ctrl->sblock.free_inodes) + 1));
	ctrl->sblock_dirty = TRUE;
	mutex_unlock(&ctrl->sblock_lock);

	/* update block group descriptor and block group bitmap */
	mutex_lock(&group->grp_lock);
	group->grp.free_inodes = le16_to_cpu((le16_to_cpu(group->grp.free_inodes) + 1));
	i = umod32(inode_no, le32_to_cpu(ctrl->sblock.inodes_per_group));
	group->inode_bmap[i >> 3] &= ~(1 << (i & 0x7));
	group->grp_dirty = TRUE;
	mutex_unlock(&group->grp_lock);

	return 0;
}

int ext4fs_control_sync(struct ext4fs_control_t * ctrl)
{
	int rc;
	u32_t g, wr;
	u32_t blkno, blkoff, desc_per_blk;

	/* Lock sblock */
	mutex_lock(&ctrl->sblock_lock);

	if(ctrl->sblock_dirty)
	{
		/* Write superblock to block device */
		wr = block_write(ctrl->bdev, (u8_t *)&ctrl->sblock, 1024, sizeof(struct ext2_sblock_t));
		if(wr != sizeof(struct ext2_sblock_t))
		{
			mutex_unlock(&ctrl->sblock_lock);
			return -1;
		}

		/* Clear sblock_dirty flag */
		ctrl->sblock_dirty = FALSE;
	}

	/* Unlock sblock */
	mutex_unlock(&ctrl->sblock_lock);

	desc_per_blk = udiv32(ctrl->block_size, sizeof(struct ext2_block_group_t));
	for(g = 0; g < ctrl->group_count; g++)
	{
		/* Lock group */
		mutex_lock(&ctrl->groups[g].grp_lock);

		/* Check group dirty flag */
		if(!ctrl->groups[g].grp_dirty)
		{
			mutex_unlock(&ctrl->groups[g].grp_lock);
			continue;
		}

		/* Write group descriptor to block device */
		blkno = ctrl->group_table_blkno + udiv32(g, desc_per_blk);
		blkoff = umod32(g, desc_per_blk) * sizeof(struct ext2_block_group_t);
		rc = ext4fs_devwrite(ctrl, blkno, blkoff, sizeof(struct ext2_block_group_t), (char *)&ctrl->groups[g].grp);
		if(rc)
		{
			mutex_unlock(&ctrl->groups[g].grp_lock);
			return rc;
		}

		/* Write block bitmap to block device */
		blkno = le32_to_cpu(ctrl->groups[g].grp.block_bmap_id);
		blkoff = 0;
		rc = ext4fs_devwrite(ctrl, blkno, blkoff, ctrl->block_size, (char *)ctrl->groups[g].block_bmap);
		if(rc)
		{
			mutex_unlock(&ctrl->groups[g].grp_lock);
			return rc;
		}

		/* Write inode bitmap to block device */
		blkno = le32_to_cpu(ctrl->groups[g].grp.inode_bmap_id);
		blkoff = 0;
		rc = ext4fs_devwrite(ctrl, blkno, blkoff, ctrl->block_size, (char *)ctrl->groups[g].inode_bmap);
		if(rc)
		{
			mutex_unlock(&ctrl->groups[g].grp_lock);
			return rc;
		}

		/* Clear grp_dirty flag */
		ctrl->groups[g].grp_dirty = FALSE;

		/* Unlock group */
		mutex_unlock(&ctrl->groups[g].grp_lock);
	}

	/* Flush cached data in device request queue */
	block_sync(ctrl->bdev);

	return 0;
}

int ext4fs_control_init(struct ext4fs_control_t * ctrl, struct block_t * bdev)
{
	int rc;
	u64_t sb_read;
	u32_t g, blkno, blkoff, desc_per_blk;

	/* Save underlying block device pointer */
	ctrl->bdev = bdev;

	/* Init superblock lock */
	mutex_init(&ctrl->sblock_lock);

	/* Read the superblock.  */
	sb_read = block_read(bdev, (u8_t *)&ctrl->sblock, 1024, sizeof(struct ext2_sblock_t));
	if(sb_read != sizeof(struct ext2_sblock_t))
	{
		rc = -1;
		goto fail;
	}

	/* Clear the sblock_dirty flag */
	ctrl->sblock_dirty = FALSE;

	/* Make sure this is an ext2 filesystem.  */
	if(le16_to_cpu(ctrl->sblock.magic) != EXT2_MAGIC)
	{
		rc = -1;
		goto fail;
	}

	/* Directory indexing not supported so throw warning */
	if(le32_to_cpu(ctrl->sblock.feature_compatibility) &
	EXT2_FEAT_COMPAT_DIR_INDEX)
	{
		LOG("ext4: directory indexing is not available");
	}

	/* Pre-compute frequently required values */
	ctrl->log2_block_size = le32_to_cpu((ctrl)->sblock.log2_block_size) + 1;
	ctrl->block_size = 1 << (ctrl->log2_block_size + EXT2_SECTOR_BITS);
	ctrl->dir_blklast = EXT2_DIRECT_BLOCKS;
	ctrl->indir_blklast = EXT2_DIRECT_BLOCKS + (ctrl->block_size / 4);
	ctrl->dindir_blklast = EXT2_DIRECT_BLOCKS + (ctrl->block_size / 4 * (ctrl->block_size / 4 + 1));
	if(le32_to_cpu(ctrl->sblock.revision_level) == 0)
	{
		ctrl->inode_size = 128;
	}
	else
	{
		ctrl->inode_size = le16_to_cpu(ctrl->sblock.inode_size);
	}
	ctrl->inodes_per_block = udiv32(ctrl->block_size, ctrl->inode_size);

	/* Setup block groups */
	ctrl->group_count = udiv32(le32_to_cpu(ctrl->sblock.total_blocks), le32_to_cpu(ctrl->sblock.blocks_per_group));
	if(umod32(le32_to_cpu(ctrl->sblock.total_blocks), le32_to_cpu(ctrl->sblock.blocks_per_group)))
	{
		ctrl->group_count++;
	}
	ctrl->group_table_blkno = le32_to_cpu(ctrl->sblock.first_data_block) + 1;
	ctrl->groups = calloc(1, ctrl->group_count * sizeof(struct ext4fs_group_t));
	if(!ctrl->groups)
	{
		rc = -1;
		goto fail;
	}
	desc_per_blk = udiv32(ctrl->block_size, sizeof(struct ext2_block_group_t));
	for(g = 0; g < ctrl->group_count; g++)
	{
		/* Init group lock */
		mutex_init(&ctrl->groups[g].grp_lock);

		/* Load descriptor */
		blkno = ctrl->group_table_blkno + udiv32(g, desc_per_blk);
		blkoff = umod32(g, desc_per_blk) * sizeof(struct ext2_block_group_t);
		rc = ext4fs_devread(ctrl, blkno, blkoff, sizeof(struct ext2_block_group_t), (char *)&ctrl->groups[g].grp);
		if(rc)
		{
			goto fail1;
		}

		/* Load group block bitmap */
		ctrl->groups[g].block_bmap = calloc(1, ctrl->block_size);
		if(!ctrl->groups[g].block_bmap)
		{
			rc = -1;
			goto fail1;
		}
		blkno = le32_to_cpu(ctrl->groups[g].grp.block_bmap_id);
		blkoff = 0;
		rc = ext4fs_devread(ctrl, blkno, blkoff, ctrl->block_size, (char *)ctrl->groups[g].block_bmap);
		if(rc)
		{
			goto fail1;
		}

		/* Load group inode bitmap */
		ctrl->groups[g].inode_bmap = calloc(1, ctrl->block_size);
		if(!ctrl->groups[g].inode_bmap)
		{
			rc = -1;
			goto fail1;
		}
		blkno = le32_to_cpu(ctrl->groups[g].grp.inode_bmap_id);
		blkoff = 0;
		rc = ext4fs_devread(ctrl, blkno, blkoff, ctrl->block_size, (char *)ctrl->groups[g].inode_bmap);
		if(rc)
		{
			goto fail1;
		}

		/* Clear grp_dirty flag */
		ctrl->groups[g].grp_dirty = FALSE;
	}

	return 0;

	fail1: for(g = 0; g < ctrl->group_count; g++)
	{
		if(ctrl->groups[g].block_bmap)
		{
			free(ctrl->groups[g].block_bmap);
			ctrl->groups[g].block_bmap = NULL;
		}
		if(ctrl->groups[g].inode_bmap)
		{
			free(ctrl->groups[g].inode_bmap);
			ctrl->groups[g].inode_bmap = NULL;
		}
	}
	free(ctrl->groups);
fail:
	return rc;
}

int ext4fs_control_exit(struct ext4fs_control_t * ctrl)
{
	u32_t g;

	/* Free group bitmaps */
	for(g = 0; g < ctrl->group_count; g++)
	{
		if(ctrl->groups[g].block_bmap)
		{
			free(ctrl->groups[g].block_bmap);
			ctrl->groups[g].block_bmap = NULL;
		}
		if(ctrl->groups[g].inode_bmap)
		{
			free(ctrl->groups[g].inode_bmap);
			ctrl->groups[g].inode_bmap = NULL;
		}
	}

	/* Free groups */
	free(ctrl->groups);

	return 0;
}
