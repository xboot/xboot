#ifndef __BLKDEV_H__
#define __BLKDEV_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * block device type
 */
enum blkdev_type {
	BLK_DEV_MTD,
	BLK_DEV_MMC,
};

/*
 * the struct of blkdev.
 */
struct blkdev
{
	/* the device name */
	const char * name;

	/* the type of block device */
	const enum blkdev_type type;

	/* ... */

	/* block device's driver */
	void * driver;
};

/*
 * the list of blkdev
 */
struct blkdev_list
{
	struct blkdev * dev;
	struct list_head entry;
};

struct blkdev * search_blkdev(const char * name);
struct blkdev * search_blkdev_with_type(const char * name, enum blkdev_type type);
x_bool register_blkdev(struct blkdev * dev);
x_bool unregister_blkdev(const char * name);

#endif /* __BLKDEV_H__ */
