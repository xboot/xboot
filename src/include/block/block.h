#ifndef __BLOCK_H__
#define __BLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct block_t
{
	/* The block name */
	const char * name;

	/* The size of block */
	size_t blksz;

	/* The count of block */
	size_t blkcnt;

	/* Open device */
	int (*open)(struct block_t * blk);

	/* Read a block from device, return the block counts of reading */
	ssize_t (*read)(struct block_t * blk, u8_t * buf, size_t blkno, size_t blkcnt);

	/* Write a block to device, return the block counts of writing */
	ssize_t (*write)(struct block_t * blk, const u8_t * buf, size_t blkno, size_t blkcnt);

	/* Close device */
	int (*close)(struct block_t * blk);

	/* Private data */
	void * priv;
};

struct block_t * search_block(const char * name);
bool_t register_block(struct block_t * blk);
bool_t unregister_block(struct block_t * blk);

loff_t get_block_total_size(struct block_t * blk);
size_t get_block_total_count(struct block_t * blk);
size_t get_block_size(struct block_t * blk);
loff_t get_block_offset(struct block_t * blk, size_t blkno);
loff_t block_read(struct block_t * blk, u8_t * buf, loff_t offset, loff_t count);
loff_t block_write(struct block_t * blk, u8_t * buf, loff_t offset, loff_t count);

#ifdef __cplusplus
}
#endif

#endif /* __BLOCK_H__ */
