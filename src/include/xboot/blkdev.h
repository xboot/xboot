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
	/* the offset of block */
	x_s32 offset;

	/* the size of block */
	x_s32 size;

	/* the number of block with same type */
	x_s32 number;
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

	/* the information of blocks, must place NULL at the end */
	struct blkinfo * info;

	/* open device */
	x_s32 (*open)(struct blkdev * dev);

	/* read block from device */
	x_s32 (*read)(struct blkdev * dev, x_u8 * buf, x_s32 blkno, x_s32 blkcnt);

	/* write block to device */
	x_s32 (*write)(struct blkdev * dev, const x_u8 * buf, x_s32 blkno, x_s32 blkcnt);

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
