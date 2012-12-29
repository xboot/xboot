#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct fifo_t {
	/* the buffer holding the data */
	u8_t * buffer;

	/* the size of the allocated buffer */
	size_t size;

	/* data is added at offset (in % size) */
	size_t in;

	/* data is extracted from off (out % size) */
	size_t out;
};

struct fifo_t * fifo_init(u8_t * buf, size_t size);
struct fifo_t * fifo_alloc(size_t size);
void fifo_free(struct fifo_t * fifo);
void fifo_reset(struct fifo_t * fifo);
size_t fifo_len(struct fifo_t * fifo);
size_t fifo_put(struct fifo_t * fifo, u8_t * buf, size_t len);
size_t fifo_get(struct fifo_t * fifo, u8_t * buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __FIFO_H__ */
