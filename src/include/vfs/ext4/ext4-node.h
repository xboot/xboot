#ifndef __EXT4_NODE_H__
#define __EXT4_NODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vfs/ext4/ext4.h>

#define EXT4_NODE_LOOKUP_SIZE	(4)

/* Information for accessing a ext4fs file/directory */
struct ext4fs_node_t {
	/* Parent ext4fs control */
	struct ext4fs_control_t * ctrl;

	/* Underlying Inode */
	struct ext2_inode_t inode;
	u32_t inode_no;
	bool_t inode_dirty;

	/*
	 * Cached data block
	 * Allocated on demand. Must be freed in vput()
	 */
	u32_t cached_blkno;
	u8_t * cached_block;
	bool_t cached_dirty;

	/*
	 * Indirect block
	 * Allocated on demand. Must be freed in vpuf()
	 */
	u32_t * indir_block;
	u32_t indir_blkno;
	bool_t indir_dirty;

	/* Double-Indirect level1 block
	 * Allocated on demand. Must be freed in vput()
	 */
	u32_t * dindir1_block;
	u32_t dindir1_blkno;
	bool_t dindir1_dirty;

	/* Double-Indirect level2 block
	 * Allocated on demand. Must be freed in vput()
	 */
	u32_t * dindir2_block;
	u32_t dindir2_blkno;
	bool_t dindir2_dirty;

	/* Child directory entry lookup table */
	u32_t lookup_victim;
	char lookup_name[EXT4_NODE_LOOKUP_SIZE][VFS_MAX_NAME];
	struct ext2_dirent_t lookup_dent[EXT4_NODE_LOOKUP_SIZE];
};

u64_t ext4fs_node_get_size(struct ext4fs_node_t * node);
void ext4fs_node_set_size(struct ext4fs_node_t * node, u64_t size);
int ext4fs_node_read_blk(struct ext4fs_node_t * node, u32_t blkno, u32_t blkoff, u32_t blklen, char * buf);
int ext4fs_node_write_blk(struct ext4fs_node_t * node, u32_t blkno, u32_t blkoff, u32_t blklen, char * buf);
int ext4fs_node_sync(struct ext4fs_node_t * node);
int ext4fs_node_read_blkno(struct ext4fs_node_t * node, u32_t blkpos, u32_t * blkno);
int ext4fs_node_write_blkno(struct ext4fs_node_t * node, u32_t blkpos, u32_t blkno);
u32_t ext4fs_node_read(struct ext4fs_node_t * node, u64_t pos, u32_t len, char * buf);
u32_t ext4fs_node_write(struct ext4fs_node_t * node, u64_t pos, u32_t len, char * buf);
int ext4fs_node_truncate(struct ext4fs_node_t * node, u64_t pos);
int ext4fs_node_load(struct ext4fs_control_t * ctrl, u32_t inode_no, struct ext4fs_node_t * node);
int ext4fs_node_init(struct ext4fs_node_t * node);
int ext4fs_node_exit(struct ext4fs_node_t * node);
int ext4fs_node_read_dirent(struct ext4fs_node_t * dnode, s64_t off, struct vfs_dirent_t * d);
int ext4fs_node_find_dirent(struct ext4fs_node_t * dnode, const char * name, struct ext2_dirent_t * dent);
int ext4fs_node_add_dirent(struct ext4fs_node_t * dnode, const char * name, u32_t inode_no, u8_t type);
int ext4fs_node_del_dirent(struct ext4fs_node_t * dnode, const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __EXT4_NODE_H__ */
