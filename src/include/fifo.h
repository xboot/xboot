#ifndef __FIFO_H__
#define __FIFO_H__

#include <configs.h>
#include <default.h>
#include <types.h>


struct fifo {
	/* the buffer holding the data */
	x_u8 * buffer;

	/* the size of the allocated buffer */
	x_u32 size;

	/* data is added at offset (in % size) */
	x_u32 in;

	/* data is extracted from off. (out % size) */
	x_u32 out;
};


struct fifo * fifo_init(x_u8 * buffer, x_u32 size);
struct fifo * fifo_alloc(x_u32 size);
void fifo_free(struct fifo * fifo);
void fifo_reset(struct fifo * fifo);
x_u32 fifo_len(struct fifo * fifo);
x_u32 fifo_put(struct fifo * fifo, x_u8 * buffer, x_u32 len);
x_u32 fifo_get(struct fifo *fifo, x_u8 * buffer, x_u32 len);


#endif /* __FIFO_H__ */
