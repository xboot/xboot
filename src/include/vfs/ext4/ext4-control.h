#ifndef __EXT4_CONTROL_H__
#define __EXT4_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vfs/ext4/ext4.h>

/* Information for accessing block groups */
struct ext4fs_group_t {
	/* lock to protect group */
	struct mutex_t grp_lock;
	struct ext2_block_group_t grp;

	u8_t * block_bmap;
	u8_t * inode_bmap;

	bool_t grp_dirty;
};

/* Information about a "mounted" ext filesystem */
struct ext4fs_control_t {
	struct block_t * bdev;

	/*
	 * lock to protect:
	 * sblock.free_blocks, 
	 * sblock.free_inodes,
	 * sblock.mtime,
	 * sblock.utime,
	 * sblock_dirty
	 */
	struct mutex_t sblock_lock;
	struct ext2_sblock_t sblock;

	/* flag to show whether sblock,
	 * groups, or bitmaps are updated.
	 */
	bool_t sblock_dirty;

	u32_t log2_block_size;
	u32_t block_size;
	u32_t dir_blklast;
	u32_t indir_blklast;
	u32_t dindir_blklast;

	u32_t inode_size;
	u32_t inodes_per_block;

	u32_t group_count;
	u32_t group_table_blkno;
	struct ext4fs_group_t * groups;
};

u32_t ext4fs_current_timestamp(void);
int ext4fs_devread(struct ext4fs_control_t * ctrl, u32_t blkno, u32_t blkoff, u32_t buf_len, char * buf);
int ext4fs_devwrite(struct ext4fs_control_t * ctrl, u32_t blkno, u32_t blkoff, u32_t buf_len, char * buf);
int ext4fs_control_read_inode(struct ext4fs_control_t * ctrl, u32_t inode_no, struct ext2_inode_t * inode);
int ext4fs_control_write_inode(struct ext4fs_control_t * ctrl, u32_t inode_no, struct ext2_inode_t * inode);
int ext4fs_control_alloc_block(struct ext4fs_control_t * ctrl, u32_t inode_no, u32_t * blkno);
int ext4fs_control_free_block(struct ext4fs_control_t * ctrl, u32_t blkno);
int ext4fs_control_alloc_inode(struct ext4fs_control_t * ctrl, u32_t parent_inode_no, u32_t * inode_no);
int ext4fs_control_free_inode(struct ext4fs_control_t * ctrl, u32_t inode_no);
int ext4fs_control_sync(struct ext4fs_control_t * ctrl);
int ext4fs_control_init(struct ext4fs_control_t * ctrl, struct block_t * bdev);
int ext4fs_control_exit(struct ext4fs_control_t * ctrl);

#ifdef __cplusplus
}
#endif

#endif /* __EXT4_CONTROL_H__ */
