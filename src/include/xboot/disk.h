#ifndef __DISK_H__
#define __DISK_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>
#include <xboot/partition.h>

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

	/* partition parser */
	struct partition_parser * parser;

	/* the size of sector */
	x_u32 sector_size;

	/* the count of sector */
	x_u32 sector_count;

	/* read a sector from disk */
	x_bool (*read_sector)(struct disk * disk, x_u32 sector, x_u8 * data);

	/* write a sector to disk */
	x_bool (*write_sector)(struct disk * disk, x_u32 sector, x_u8 * data);

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


struct disk * search_disk(const char * name);
x_bool register_disk(struct disk * disk);
x_bool unregister_disk(struct disk * disk);

#endif /* __DISK_H__ */
