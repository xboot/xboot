#ifndef __DISK_H__
#define __DISK_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>
#include <xboot/blkdev.h>
#include <disk/partition.h>

/*
 * forward declare
 */
struct partition;

/*
 * the struct of disk
 */
struct disk
{
	/* the disk name */
	const char * name;

	/* partition information */
	struct partition info;

	/* the size of sector */
	u32_t sector_size;

	/* the count of sector */
	u32_t sector_count;

	/* read sectors from disk, return the sector counts of reading */
	s32_t (*read_sectors)(struct disk * disk, u8_t * buf, u32_t sector, u32_t count);

	/* write sectors to disk, return the sector counts of writing */
	s32_t (*write_sectors)(struct disk * disk, const u8_t * buf, u32_t sector, u32_t count);

	/* priv data pointer */
	void * priv;
};

/*
 * the list of disk
 */
struct disk_list
{
	struct disk * disk;
	struct list_head entry;
};

bool_t register_disk(struct disk * disk, enum blkdev_type type);
bool_t unregister_disk(struct disk * disk);

#endif /* __DISK_H__ */
