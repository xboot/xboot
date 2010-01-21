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
	BLK_DEV_LOOP,
	BLK_DEV_MTD,
	BLK_DEV_MMC,
};

struct blkinfo {
	/* the no of block */
	x_s32 blkno;

	/* the offset of block */
	x_s32 offset;

	/* the size of block */
	x_s32 size;

	/* the number of block with same type */
	x_s32 number;

	/* link other blkinfo */
	struct list_head entry;
};

/*
 * the struct of blkdev.
 */
struct blkdev
{
	/* the device name */
	const char * name;

	/* the type of block device */
	enum blkdev_type type;

	/* the information of blocks */
	struct blkinfo * info;

	/* open device */
	x_s32 (*open)(struct blkdev * dev);

	/* read a block from device */
	x_s32 (*read)(struct blkdev * dev, x_u8 * buf, x_s32 blkno);

	/* write a block to device */
	x_s32 (*write)(struct blkdev * dev, const x_u8 * buf, x_s32 blkno);

	/* ioctl device */
	x_s32 (*ioctl)(struct blkdev * dev, x_u32 cmd, void * arg);

	/* close device */
	x_s32 (*close)(struct blkdev * dev);

	/* block device's driver */
	void * driver;
};


struct blkdev * search_blkdev(const char * name);
struct blkdev * search_blkdev_with_type(const char * name, enum blkdev_type type);
x_bool register_blkdev(struct blkdev * dev);
x_bool unregister_blkdev(const char * name);

x_size get_blkdev_total_size(struct blkdev * dev);
x_size get_blkdev_total_number(struct blkdev * dev);
x_s32 get_blkdev_size(struct blkdev * dev, x_s32 blkno);
x_s32 get_blkdev_offset(struct blkdev * dev, x_s32 blkno);
x_s32 get_blkdev_blkno(struct blkdev * dev, x_s32 offset);

#endif /* __BLKDEV_H__ */
