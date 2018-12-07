#ifndef _FAT_NODE_H__
#define _FAT_NODE_H__

#include "fat_common.h"

#define FAT_NODE_LOOKUP_SIZE		4

/* Information for accessing a FAT file/directory. */
struct fatfs_node {
	/* Parent FAT control */
	struct fatfs_control *ctrl;

	/* Parent directory entry */
	struct fatfs_node *parent;
	u32 parent_dent_off;
	u32 parent_dent_len;
	struct fat_dirent parent_dent;
	bool parent_dent_dirty;

	/* First cluster */
	u32 first_cluster;

	/* Cached clusters */
	u8 *cached_data;
	u32 cached_clust;
	bool cached_dirty;

	/* Child directory entry lookup table */
	u32 lookup_victim;
	char lookup_name[FAT_NODE_LOOKUP_SIZE][VFS_MAX_NAME];
	u32 lookup_off[FAT_NODE_LOOKUP_SIZE];
	u32 lookup_len[FAT_NODE_LOOKUP_SIZE];
	struct fat_dirent lookup_dent[FAT_NODE_LOOKUP_SIZE];
};

u32 fatfs_node_get_size(struct fatfs_node *node);

u32 fatfs_node_read(struct fatfs_node *node, u32 pos, u32 len, u8 *buf);

u32 fatfs_node_write(struct fatfs_node *node, u32 pos, u32 len, u8 *buf);

int fatfs_node_truncate(struct fatfs_node *node, u32 pos);

int fatfs_node_sync(struct fatfs_node *node);

int fatfs_node_init(struct fatfs_control *ctrl, struct fatfs_node *node);

int fatfs_node_exit(struct fatfs_node *node);

int fatfs_node_read_dirent(struct fatfs_node *dnode, 
			    loff_t off, struct vfs_dirent_t * d);

int fatfs_node_find_dirent(struct fatfs_node *dnode, 
			   const char *name,
			   struct fat_dirent *dent, 
			   u32 *dent_off, u32 *dent_len);

int fatfs_node_add_dirent(struct fatfs_node *dnode, 
			   const char *name,
			   struct fat_dirent *ndent);

int fatfs_node_del_dirent(struct fatfs_node *dnode, 
			  const char *name,
			  u32 dent_off, u32 dent_len);

#endif
