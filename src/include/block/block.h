#ifndef __BLOCK_H__
#define __BLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct block_t
{
	char * name;

	u64_t (*capacity)(struct block_t * blk);
	u64_t (*read)(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count);
	u64_t (*write)(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count);
	void (*sync)(struct block_t * blk);

	void * priv;
};

static inline u64_t block_available(struct block_t * blk, u64_t offset, u64_t length)
{
	u64_t cap;

	if(blk)
	{
		cap = blk->capacity(blk);
		if(offset + length > cap)
			return cap - offset;
		return length;
	}
	return 0;
}

struct block_t * search_block(const char * name);
struct device_t * register_block(struct block_t * blk, struct driver_t * drv);
void unregister_block(struct block_t * blk);
struct device_t * register_sub_block(struct block_t * pblk, u64_t offset, u64_t length, const char * name);
void unregister_sub_block(struct block_t * pblk);

u64_t block_capacity(struct block_t * blk);
u64_t block_read(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count);
u64_t block_write(struct block_t * blk, u8_t * buf, u64_t offset, u64_t count);
void block_sync(struct block_t * blk);

#ifdef __cplusplus
}
#endif

#endif /* __BLOCK_H__ */
