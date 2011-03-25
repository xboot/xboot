#ifndef __PARTITION_H__
#define __PARTITION_H__

#include <configs.h>
#include <default.h>
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
	x_s32 sector_from;

	/* the sector of the end */
	x_s32 sector_to;

	/* the sector size */
	x_s32 sector_size;

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
	x_bool (*probe)(struct disk * disk);
};

/*
 * the list of partition parser
 */
struct partition_parser_list
{
	struct partition_parser * parser;
	struct list_head entry;
};

x_bool register_partition_parser(struct partition_parser * parser);
x_bool unregister_partition_parser(struct partition_parser * parser);
x_bool partition_parser_probe(struct disk * disk);

#endif /* __PARTITION_H__ */
