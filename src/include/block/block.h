#ifndef __BLOCK_H__
#define __BLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct block_t
{
	/* The block name */
	char * name;

	/* The size of block */
	u64_t blksz;

	/* The total count of block */
	u64_t blkcnt;

	/* Read block device, return the block counts of reading */
	u64_t (*read)(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt);

	/* Write block device, return the block counts of writing */
	u64_t (*write)(struct block_t * blk, u8_t * buf, u64_t blkno, u64_t blkcnt);

	/* Sync cache to block device */
	void (*sync)(struct block_t * blk);

	/* Private data */
	void * priv;
};

static inline u64_t block_size(struct block_t * blk)
{
	return (blk->blksz);
}

static inline u64_t block_count(struct block_t * blk)
{
	return (blk->blkcnt);
}

static inline u64_t block_capacity(struct block_t * blk)
{
	return (blk->blksz * blk->blkcnt);
}

static inline u64_t block_offset(struct block_t * blk, u64_t blkno)
{
	return (blk->blksz * blkno);
}

static inline u64_t block_available_count(struct block_t * blk, u64_t blkno, u64_t blkcnt)
{
	u64_t count = 0;

	if(blk->blkcnt > blkno)
	{
		count = blk->blkcnt - blkno;
		if(count > blkcnt)
			count = blkcnt;
	}
	return count;
}

static inline u64_t block_available_length(struct block_t * blk, u64_t blkno, u64_t blkcnt)
{
	return (block_size(blk) * block_available_count(blk, blkno, blkcnt));
}

struct block_t * search_block(const char * name);
struct device_t * register_block(struct block_t * blk, struct driver_t * drv);
void unregister_block(struct block_t * blk);
struct device_t * register_sub_block(struct block_t * pblk, u64_t offset, u64_t length, const char * name);
void unregister_sub_block(struct block_t * pblk);

u64_t block_read(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count);
u64_t block_write(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count);
void block_sync(struct block_t * blk);

#ifdef __cplusplus
}
#endif

#endif /* __BLOCK_H__ */
