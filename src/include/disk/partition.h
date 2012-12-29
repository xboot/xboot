#ifndef __PARTITION_H__
#define __PARTITION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>
#include <disk/disk.h>

/*
 * forward declare
 */
struct disk;

/*
 * the struct of partition
 */
struct partition
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
	struct blkdev * dev;

	/* link other partition */
	struct list_head entry;
};

/*
 * the struct of partition parser
 */
struct partition_parser
{
	/* the partition parser name */
	const char * name;

	/* probe disk's partition */
	bool_t (*probe)(struct disk * disk);
};

/*
 * the list of partition parser
 */
struct partition_parser_list
{
	struct partition_parser * parser;
	struct list_head entry;
};

bool_t register_partition_parser(struct partition_parser * parser);
bool_t unregister_partition_parser(struct partition_parser * parser);
bool_t partition_parser_probe(struct disk * disk);

#ifdef __cplusplus
}
#endif

#endif /* __PARTITION_H__ */
