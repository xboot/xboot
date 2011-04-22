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
	BLK_DEV_RAMDISK,
	BLK_DEV_LOOP,
	BLK_DEV_MMC,
	BLK_DEV_MTD,
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

	/* the size of block */
	x_u32 blksz;

	/* the count of block */
	x_u32 blkcnt;

	/* open device */
	x_s32 (*open)(struct blkdev * dev);

	/* read a block from device, return the block counts of reading */
	x_s32 (*read)(struct blkdev * dev, x_u8 * buf, x_u32 blkno, x_u32 blkcnt);

	/* write a block to device, return the block counts of writing */
	x_s32 (*write)(struct blkdev * dev, const x_u8 * buf, x_u32 blkno, x_u32 blkcnt);

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
x_u32 get_blkdev_total_count(struct blkdev * dev);
x_u32 get_blkdev_size(struct blkdev * dev);
x_off get_blkdev_offset(struct blkdev * dev, x_u32 blkno);

#endif /* __BLKDEV_H__ */
