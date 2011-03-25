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
	x_s32 sector_size;

	/* the count of sector */
	x_s32 sector_count;

	/* read sectors from disk */
	x_bool (*read_sector)(struct disk * disk, x_u8 * buf, x_s32 sector, x_s32 count);

	/* write sectors to disk */
	x_bool (*write_sector)(struct disk * disk, const x_u8 * buf, x_s32 sector, x_s32 count);

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

x_bool register_disk(struct disk * disk, enum blkdev_type type);
x_bool unregister_disk(struct disk * disk);

#endif /* __DISK_H__ */
