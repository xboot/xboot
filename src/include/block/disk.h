#ifndef __DISK_H__
#define __DISK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <block/block.h>

struct partition_t
{
	/* Partition name */
	char name[64 + 1];

	/* The sector number of the start */
	u64_t from;

	/* The sector number of the end */
	u64_t to;

	/* The sector's size in bytes */
	u64_t size;

	/* Link to this partition's block device */
	struct block_t * blk;

	/* Link other partition */
	struct list_head entry;
};

struct disk_t
{
	/* The disk name */
	char * name;

	/* The size of sector */
	u64_t size;

	/* The count of sector */
	u64_t count;

	/* Partition information */
	struct partition_t part;

	/* Read disk device, return the sector counts of reading */
	u64_t (*read)(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count);

	/* Write disk device, return the sector counts of writing */
	u64_t (*write)(struct disk_t * disk, u8_t * buf, u64_t sector, u64_t count);

	/* Sync cache to disk device */
	void (*sync)(struct disk_t * disk);

	/* Private data */
	void * priv;
};

struct disk_t * search_disk(const char * name);
bool_t register_disk(struct device_t ** device, struct disk_t * disk);
bool_t unregister_disk(struct disk_t * disk);

u64_t disk_read(struct disk_t * disk, u8_t * buf, u64_t offset, u64_t count);
u64_t disk_write(struct disk_t * disk, u8_t * buf, u64_t offset, u64_t count);
void disk_sync(struct disk_t * disk);

#ifdef __cplusplus
}
#endif

#endif /* __DISK_H__ */
