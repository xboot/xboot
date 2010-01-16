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
	enum blkdev_type type;

	/* number of blocks */
	x_size blk_num;

	/* the size of block */
	x_size blk_size;

	/* open device */
	x_s32 (*open)(struct blkdev * dev);

	/* read block from device */
	x_size (*read)(struct blkdev * dev, x_u8 * buf, x_size blkno, x_size blkcnt);

	/* write block to device */
	x_size (*write)(struct blkdev * dev, const x_u8 * buf, x_size blkno, x_size blkcnt);

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

#endif /* __BLKDEV_H__ */
