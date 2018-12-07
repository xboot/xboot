#ifndef _FAT_CONTROL_H__
#define _FAT_CONTROL_H__

#include "fat_common.h"

#define __le32(x)				le32_to_cpu(x)
#define __le16(x)				le16_to_cpu(x)

#define FAT_TABLE_CACHE_SIZE		32

/* Information about a "mounted" FAT filesystem. */
struct fatfs_control {
	/* FAT boot sector */
	struct fat_bootsec bsec;

	/* Underlying block device */
	struct block_t * bdev;

	/* Frequently required boot sector info */
	u16 bytes_per_sector;
	u8 sectors_per_cluster;
	u8 number_of_fat;
	u32 bytes_per_cluster;
	u32 total_sectors;

	/* Derived FAT info */
	u32 first_fat_sector;
	u32 sectors_per_fat;
	u32 fat_sectors;

	u32 first_root_sector;
	u32 root_sectors;
	u32 first_root_cluster;

	u32 first_data_sector;
	u32 data_sectors;
	u32 data_clusters;

	/* FAT type (i.e. FAT12/FAT16/FAT32) */
	enum fat_types type;

	/* FAT sector cache */
	struct mutex_t fat_cache_lock;
	u32 fat_cache_victim;
	bool fat_cache_dirty[FAT_TABLE_CACHE_SIZE];
	u32 fat_cache_num[FAT_TABLE_CACHE_SIZE];
	u8 *fat_cache_buf;
};

u32 fatfs_pack_timestamp(u32 year, u32 mon, u32 day, 
			 u32 hour, u32 min, u32 sec);

void fatfs_current_timestamp(u32 *year, u32 *mon, u32 *day, 
			     u32 *hour, u32 *min, u32 *sec);

bool fatfs_control_valid_cluster(struct fatfs_control *ctrl, u32 clust);

int fatfs_control_nth_cluster(struct fatfs_control *ctrl, 
			      u32 clust, u32 pos, u32 *next);

int fatfs_control_set_last_cluster(struct fatfs_control *ctrl, u32 clust);

int fatfs_control_alloc_first_cluster(struct fatfs_control *ctrl, 
				      u32 *newclust);

int fatfs_control_append_free_cluster(struct fatfs_control *ctrl, 
				      u32 clust, u32 *newclust);

int fatfs_control_truncate_clusters(struct fatfs_control *ctrl, 
				    u32 clust);

int fatfs_control_sync(struct fatfs_control *ctrl);

int fatfs_control_init(struct fatfs_control *ctrl, struct block_t *bdev);

int fatfs_control_exit(struct fatfs_control *ctrl);

#endif
