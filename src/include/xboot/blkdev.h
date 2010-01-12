#ifndef __BLKDEV_H__
#define __BLKDEV_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the struct of blkdev.
 */
struct blkdev
{
	/* the blkdev name */
	const char * name;

	/* ... */
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
x_bool register_blkdev(struct blkdev * dev);
x_bool unregister_blkdev(struct blkdev * dev);

#endif /* __BLKDEV_H__ */
