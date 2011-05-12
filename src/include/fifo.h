#ifndef __FIFO_H__
#define __FIFO_H__

#include <configs.h>
#include <default.h>
#include <types.h>


struct fifo {
	/* the buffer holding the data */
	u8_t * buffer;

	/* the size of the allocated buffer */
	u32_t size;

	/* data is added at offset (in % size) */
	u32_t in;

	/* data is extracted from off. (out % size) */
	u32_t out;
};


struct fifo * fifo_init(u8_t * buffer, u32_t size);
struct fifo * fifo_alloc(u32_t size);
void fifo_free(struct fifo * fifo);
void fifo_reset(struct fifo * fifo);
u32_t fifo_len(struct fifo * fifo);
u32_t fifo_put(struct fifo * fifo, u8_t * buffer, u32_t len);
u32_t fifo_get(struct fifo *fifo, u8_t * buffer, u32_t len);


#endif /* __FIFO_H__ */
