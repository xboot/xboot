#ifndef __PARTITION_H__
#define __PARTITION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <disk/disk.h>

struct disk_t;

struct partition_t
{
	/* partition name */
	char name[32 + 1];

	/* the sector of the start */
	size_t sector_from;

	/* the sector of the end */
	size_t sector_to;

	/* the sector size */
	size_t sector_size;

	/* link to this partition's block device */
	struct block_t * dev;

	/* link other partition */
	struct list_head entry;
};

struct partition_parser_t
{
	/* the partition parser name */
	const char * name;

	/* probe disk's partition */
	bool_t (*probe)(struct disk_t * disk);
};

bool_t register_partition_parser(struct partition_parser_t * parser);
bool_t unregister_partition_parser(struct partition_parser_t * parser);
bool_t partition_parser_probe(struct disk_t * disk);

#ifdef __cplusplus
}
#endif

#endif /* __PARTITION_H__ */
