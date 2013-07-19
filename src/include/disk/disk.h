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
struct partition_t;

/*
 * the struct of disk_t
 */
struct disk_t
{
	/* the disk name */
	const char * name;

	/* partition information */
	struct partition_t info;

	/* the size of sector */
	size_t sector_size;

	/* the count of sector */
	size_t sector_count;

	/* read sectors from disk, return the sector counts of reading */
	ssize_t (*read_sectors)(struct disk_t * disk, u8_t * buf, size_t sector, size_t count);

	/* write sectors to disk, return the sector counts of writing */
	ssize_t (*write_sectors)(struct disk_t * disk, const u8_t * buf, size_t sector, size_t count);

	/* priv data pointer */
	void * priv;
};

/*
 * the list of disk
 */
struct disk_list
{
	struct disk_t * disk;
	struct list_head entry;
};

bool_t register_disk(struct disk_t * disk, enum blkdev_type_t type);
bool_t unregister_disk(struct disk_t * disk);

#ifdef __cplusplus
}
#endif

#endif /* __DISK_H__ */
