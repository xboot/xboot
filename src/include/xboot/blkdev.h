#ifndef __BLKDEV_H__
#define __BLKDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * block device type
 */
enum blkdev_type {
	BLK_DEV_ROMDISK,
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
	size_t blksz;

	/* the count of block */
	size_t blkcnt;

	/* open device */
	int (*open)(struct blkdev * dev);

	/* read a block from device, return the block counts of reading */
	ssize_t (*read)(struct blkdev * dev, u8_t * buf, size_t blkno, size_t blkcnt);

	/* write a block to device, return the block counts of writing */
	ssize_t (*write)(struct blkdev * dev, const u8_t * buf, size_t blkno, size_t blkcnt);

	/* ioctl device */
	int (*ioctl)(struct blkdev * dev, int cmd, void * arg);

	/* close device */
	int (*close)(struct blkdev * dev);

	/* block device's driver */
	void * driver;
};

struct blkdev * search_blkdev(const char * name);
struct blkdev * search_blkdev_with_type(const char * name, enum blkdev_type type);
bool_t register_blkdev(struct blkdev * dev);
bool_t unregister_blkdev(const char * name);

loff_t get_blkdev_total_size(struct blkdev * dev);
size_t get_blkdev_total_count(struct blkdev * dev);
size_t get_blkdev_size(struct blkdev * dev);
loff_t get_blkdev_offset(struct blkdev * dev, size_t blkno);

#ifdef __cplusplus
}
#endif

#endif /* __BLKDEV_H__ */
