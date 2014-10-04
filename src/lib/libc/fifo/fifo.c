/*
 * libc/fifo/fifo.c
 */

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include <fifo.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

struct fifo_t * fifo_init(u8_t * buf, size_t size)
{
	struct fifo_t * fifo;

	if(!buf)
		return NULL;

	fifo = malloc(sizeof(struct fifo_t));
	if(!fifo)
		return NULL;

	fifo->buffer = buf;
	fifo->size = size;
	fifo->in = fifo->out = 0;

	return fifo;
}
EXPORT_SYMBOL(fifo_init);

struct fifo_t * fifo_alloc(size_t size)
{
	u8_t * buffer;
	struct fifo_t * fifo;

	buffer = malloc(size);
	if(!buffer)
		return NULL;

	fifo = fifo_init(buffer, size);
	if(!fifo)
		free(buffer);

	return fifo;
}
EXPORT_SYMBOL(fifo_alloc);

void fifo_free(struct fifo_t * fifo)
{
	if(!fifo)
		return;

	free(fifo->buffer);
	free(fifo);
}
EXPORT_SYMBOL(fifo_free);

void fifo_reset(struct fifo_t * fifo)
{
	fifo->in = fifo->out = 0;
}
EXPORT_SYMBOL(fifo_reset);

size_t fifo_len(struct fifo_t * fifo)
{
	return fifo->in - fifo->out;
}
EXPORT_SYMBOL(fifo_len);

size_t fifo_put(struct fifo_t * fifo, u8_t * buf, size_t len)
{
	size_t l;

	len = MIN(len, fifo->size - fifo->in + fifo->out);
	if(len <= 0)
		return 0;

	l = MIN(len, fifo->size - (fifo->in % fifo->size));
	memcpy(fifo->buffer + (fifo->in % fifo->size), buf, l);
	memcpy(fifo->buffer, buf + l, len - l);

	fifo->in += len;

	return len;
}
EXPORT_SYMBOL(fifo_put);

size_t fifo_get(struct fifo_t * fifo, u8_t * buf, size_t len)
{
	size_t l;

	len = MIN(len, fifo->in - fifo->out);
	if(len <= 0)
		return 0;

	l = MIN(len, fifo->size - (fifo->out % fifo->size));
	memcpy(buf, fifo->buffer + (fifo->out % fifo->size), l);
	memcpy(buf + l, fifo->buffer, len - l);

	fifo->out += len;

	if (fifo->in == fifo->out)
		fifo->in = fifo->out = 0;

	return len;
}
EXPORT_SYMBOL(fifo_get);
