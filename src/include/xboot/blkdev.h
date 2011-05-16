#ifndef __BLKDEV_H__
#define __BLKDEV_H__

#include <xboot.h>
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
	u32_t blksz;

	/* the count of block */
	u32_t blkcnt;

	/* open device */
	s32_t (*open)(struct blkdev * dev);

	/* read a block from device, return the block counts of reading */
	s32_t (*read)(struct blkdev * dev, u8_t * buf, u32_t blkno, u32_t blkcnt);

	/* write a block to device, return the block counts of writing */
	s32_t (*write)(struct blkdev * dev, const u8_t * buf, u32_t blkno, u32_t blkcnt);

	/* ioctl device */
	s32_t (*ioctl)(struct blkdev * dev, u32_t cmd, void * arg);

	/* close device */
	s32_t (*close)(struct blkdev * dev);

	/* block device's driver */
	void * driver;
};

struct blkdev * search_blkdev(const char * name);
struct blkdev * search_blkdev_with_type(const char * name, enum blkdev_type type);
bool_t register_blkdev(struct blkdev * dev);
bool_t unregister_blkdev(const char * name);

loff_t get_blkdev_total_size(struct blkdev * dev);
u32_t get_blkdev_total_count(struct blkdev * dev);
u32_t get_blkdev_size(struct blkdev * dev);
loff_t get_blkdev_offset(struct blkdev * dev, u32_t blkno);

#endif /* __BLKDEV_H__ */
