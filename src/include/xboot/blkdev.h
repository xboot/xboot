#ifndef __BLKDEV_H__
#define __BLKDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

enum blkdev_type_t {
	BLKDEV_TYPE_ROMDISK,
	BLKDEV_TYPE_LOOP,
	BLKDEV_TYPE_MMC,
	BLKDEV_TYPE_MTD,
};

struct blkdev_t
{
	/* the device name */
	const char * name;

	/* the type of block device */
	enum blkdev_type_t type;

	/* the size of block */
	size_t blksz;

	/* the count of block */
	size_t blkcnt;

	/* open device */
	int (*open)(struct blkdev_t * dev);

	/* read a block from device, return the block counts of reading */
	ssize_t (*read)(struct blkdev_t * dev, u8_t * buf, size_t blkno, size_t blkcnt);

	/* write a block to device, return the block counts of writing */
	ssize_t (*write)(struct blkdev_t * dev, const u8_t * buf, size_t blkno, size_t blkcnt);

	/* ioctl device */
	int (*ioctl)(struct blkdev_t * dev, int cmd, void * arg);

	/* close device */
	int (*close)(struct blkdev_t * dev);

	/* block device's driver */
	void * driver;
};

struct blkdev_t * search_blkdev(const char * name);
struct blkdev_t * search_blkdev_with_type(const char * name, enum blkdev_type_t type);
bool_t register_blkdev(struct blkdev_t * dev);
bool_t unregister_blkdev(const char * name);

loff_t get_blkdev_total_size(struct blkdev_t * dev);
size_t get_blkdev_total_count(struct blkdev_t * dev);
size_t get_blkdev_size(struct blkdev_t * dev);
loff_t get_blkdev_offset(struct blkdev_t * dev, size_t blkno);

#ifdef __cplusplus
}
#endif

#endif /* __BLKDEV_H__ */
