#ifndef __DISK_H__
#define __DISK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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
	size_t sector_size;

	/* the count of sector */
	size_t sector_count;

	/* read sectors from disk, return the sector counts of reading */
	ssize_t (*read_sectors)(struct disk * disk, u8_t * buf, size_t sector, size_t count);

	/* write sectors to disk, return the sector counts of writing */
	ssize_t (*write_sectors)(struct disk * disk, const u8_t * buf, size_t sector, size_t count);

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

#ifdef __cplusplus
}
#endif

#endif /* __DISK_H__ */
