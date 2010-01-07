/*
 * lib/libc/fifo.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <malloc.h>
#include <string.h>
#include <fifo.h>

/*
 * allocates a new fifo using a preallocated buffer
 *
 * buffer: the preallocated buffer to be used.
 * size: the size of the internal buffer.
 */
struct fifo * fifo_init(x_u8 * buffer, x_u32 size)
{
	struct fifo * fifo;

	fifo = malloc(sizeof(struct fifo));
	if(!fifo)
		return NULL;

	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = fifo->out = 0;

	return fifo;
}

/*
 * allocates a new fifo and its internal buffer
 *
 * size: the size of the internal buffer to be allocated.
 */
struct fifo * fifo_alloc(x_u32 size)
{
	x_u8 * buffer;
	struct fifo * fifo;

	buffer = malloc(size);
	if(!buffer)
		return NULL;

	fifo = fifo_init(buffer, size);
	if(!fifo)
		free(buffer);

	return fifo;
}

/*
 * frees the fifo
 */
void fifo_free(struct fifo * fifo)
{
	free(fifo->buffer);
	free(fifo);
}

/*
 * removes the entire fifo contents
 */
void fifo_reset(struct fifo * fifo)
{
	fifo->in = fifo->out = 0;
}

/*
 * fifo_len - returns the number of bytes available in the FIFO
 *
 * fifo: the fifo to be used.
 */
x_u32 fifo_len(struct fifo * fifo)
{
	return fifo->in - fifo->out;
}

/*
 * fifo_put - puts some data into the fifo
 *
 * fifo: the fifo to be used.
 * buffer: the data to be added.
 * len: the length of the data to be added.
 *
 * This function copies at most len bytes from the buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 */
x_u32 fifo_put(struct fifo * fifo, x_u8 * buffer, x_u32 len)
{
	x_u32 l;

	len = MIN(len, fifo->size - fifo->in + fifo->out);

	/* first put the data starting from fifo->in to buffer end */
	l = MIN(len, fifo->size - (fifo->in % fifo->size));
	memcpy(fifo->buffer + (fifo->in % fifo->size), buffer, l);

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(fifo->buffer, buffer + l, len - l);

	fifo->in += len;

	return len;
}

/*
 * fifo_get - gets some data from the fifo
 *
 * fifo: the fifo to be used.
 * buffer: where the data must be copied.
 * len: the size of the destination buffer.
 *
 * This function copies at most len bytes from the fifo into the
 * buffer and returns the number of copied bytes.
 */
x_u32 fifo_get(struct fifo *fifo, x_u8 * buffer, x_u32 len)
{
	x_u32 l;

	len = MIN(len, fifo->in - fifo->out);
	if(len == 0)
		return 0;

	/* first get the data from fifo->out until the end of the buffer */
	l = MIN(len, fifo->size - (fifo->out % fifo->size));
	memcpy(buffer, fifo->buffer + (fifo->out % fifo->size), l);

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy(buffer + l, fifo->buffer, len - l);

	fifo->out += len;

	if (fifo->in == fifo->out)
		fifo->in = fifo->out = 0;

	return len;
}
