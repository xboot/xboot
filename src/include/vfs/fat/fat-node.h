#ifndef __FAT_NODE_H__
#define __FAT_NODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vfs/fat/fat.h>

#define FAT_NODE_LOOKUP_SIZE	(4)

/*
 * Information for accessing a FAT file/directory
 */
struct fatfs_node_t {
	/* Parent FAT control */
	struct fatfs_control_t * ctrl;

	/* Parent directory entry */
	struct fatfs_node_t * parent;
	u32_t parent_dent_off;
	u32_t parent_dent_len;
	struct fat_dirent_t parent_dent;
	bool_t parent_dent_dirty;

	/* First cluster */
	u32_t first_cluster;

	/* Cached clusters */
	u8_t *cached_data;
	u32_t cached_clust;
	bool_t cached_dirty;

	/* Child directory entry lookup table */
	u32_t lookup_victim;
	char lookup_name[FAT_NODE_LOOKUP_SIZE][VFS_MAX_NAME];
	u32_t lookup_off[FAT_NODE_LOOKUP_SIZE];
	u32_t lookup_len[FAT_NODE_LOOKUP_SIZE];
	struct fat_dirent_t lookup_dent[FAT_NODE_LOOKUP_SIZE];
};

u32_t fatfs_node_get_size(struct fatfs_node_t * node);
u32_t fatfs_node_read(struct fatfs_node_t * node, u32_t pos, u32_t len, u8_t * buf);
u32_t fatfs_node_write(struct fatfs_node_t * node, u32_t pos, u32_t len, u8_t * buf);
int fatfs_node_truncate(struct fatfs_node_t * node, u32_t pos);
int fatfs_node_sync(struct fatfs_node_t * node);
int fatfs_node_init(struct fatfs_control_t * ctrl, struct fatfs_node_t * node);
int fatfs_node_exit(struct fatfs_node_t * node);
int fatfs_node_read_dirent(struct fatfs_node_t * dnode, s64_t off, struct vfs_dirent_t * d);
int fatfs_node_find_dirent(struct fatfs_node_t * dnode, const char * name, struct fat_dirent_t * dent, u32_t * dent_off, u32_t * dent_len);
int fatfs_node_add_dirent(struct fatfs_node_t * dnode, const char * name, struct fat_dirent_t * ndent);
int fatfs_node_del_dirent(struct fatfs_node_t * dnode, const char * name, u32_t dent_off, u32_t dent_len);

#ifdef __cplusplus
}
#endif

#endif /* __FAT_NODE_H__ */
