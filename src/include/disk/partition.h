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
struct disk_t;

/*
 * the struct of partition
 */
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
	struct blkdev * dev;

	/* link other partition */
	struct list_head entry;
};

/*
 * the struct of partition parser
 */
struct partition_t_parser
{
	/* the partition parser name */
	const char * name;

	/* probe disk's partition */
	bool_t (*probe)(struct disk_t * disk);
};

/*
 * the list of partition parser
 */
struct partition_t_parser_list
{
	struct partition_t_parser * parser;
	struct list_head entry;
};

bool_t register_partition_parser(struct partition_t_parser * parser);
bool_t unregister_partition_parser(struct partition_t_parser * parser);
bool_t partition_parser_probe(struct disk_t * disk);

#ifdef __cplusplus
}
#endif

#endif /* __PARTITION_H__ */
