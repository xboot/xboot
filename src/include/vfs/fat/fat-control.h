#ifndef __FAT_CONTROL_H__
#define __FAT_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <vfs/fat/fat.h>

#define FAT_TABLE_CACHE_SIZE	(32)

/*
 * Information about a "mounted" FAT filesystem
 */
struct fatfs_control_t {
	/* FAT boot sector */
	struct fat_bootsec_t bsec;

	/* Underlying block device */
	struct block_t * bdev;

	/* Frequently required boot sector info */
	u16_t bytes_per_sector;
	u8_t sectors_per_cluster;
	u8_t number_of_fat;
	u32_t bytes_per_cluster;
	u32_t total_sectors;

	/* Derived FAT info */
	u32_t first_fat_sector;
	u32_t sectors_per_fat;
	u32_t fat_sectors;

	u32_t first_root_sector;
	u32_t root_sectors;
	u32_t first_root_cluster;

	u32_t first_data_sector;
	u32_t data_sectors;
	u32_t data_clusters;

	/* FAT type */
	enum fat_type_t type;

	/* FAT sector cache */
	struct mutex_t fat_cache_lock;
	u32_t fat_cache_victim;
	bool_t fat_cache_dirty[FAT_TABLE_CACHE_SIZE];
	u32_t fat_cache_num[FAT_TABLE_CACHE_SIZE];
	u8_t * fat_cache_buf;
};

u32_t fatfs_pack_timestamp(u32_t year, u32_t mon, u32_t day, u32_t hour, u32_t min, u32_t sec);
void fatfs_current_timestamp(u32_t * year, u32_t * mon, u32_t * day, u32_t * hour, u32_t * min, u32_t * sec);
bool_t fatfs_control_valid_cluster(struct fatfs_control_t * ctrl, u32_t clust);
int fatfs_control_nth_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t pos, u32_t * next);
int fatfs_control_set_last_cluster(struct fatfs_control_t * ctrl, u32_t clust);
int fatfs_control_alloc_first_cluster(struct fatfs_control_t * ctrl, u32_t * newclust);
int fatfs_control_append_free_cluster(struct fatfs_control_t * ctrl, u32_t clust, u32_t * newclust);
int fatfs_control_truncate_clusters(struct fatfs_control_t * ctrl, u32_t clust);
int fatfs_control_sync(struct fatfs_control_t * ctrl);
int fatfs_control_init(struct fatfs_control_t * ctrl, struct block_t * bdev);
int fatfs_control_exit(struct fatfs_control_t * ctrl);

#ifdef __cplusplus
}
#endif

#endif /* __FAT_CONTROL_H__ */
