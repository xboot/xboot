#ifndef __DISK_H__
#define __DISK_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the struct of disk
 */
struct disk
{
	/* the disk name */
	const char * name;

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
