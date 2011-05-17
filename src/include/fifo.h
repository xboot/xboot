#ifndef __FIFO_H__
#define __FIFO_H__

#include <xboot.h>
#include <types.h>

struct fifo {
	/* the buffer holding the data */
	u8_t * buffer;

	/* the size of the allocated buffer */
	size_t size;

	/* data is added at offset (in % size) */
	size_t in;

	/* data is extracted from off. (out % size) */
	size_t out;
};


struct fifo * fifo_init(u8_t * buffer, size_t size);
struct fifo * fifo_alloc(size_t size);
void fifo_free(struct fifo * fifo);
void fifo_reset(struct fifo * fifo);
size_t fifo_len(struct fifo * fifo);
size_t fifo_put(struct fifo * fifo, u8_t * buffer, size_t len);
size_t fifo_get(struct fifo * fifo, u8_t * buffer, size_t len);

#endif /* __FIFO_H__ */
