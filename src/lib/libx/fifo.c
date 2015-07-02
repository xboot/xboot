/*
 * libx/fifo.c
 */

#include <types.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include <spinlock.h>
#include <fifo.h>

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

struct fifo_t * fifo_alloc(size_t size)
{
	struct fifo_t * f;

	f = malloc(sizeof(struct fifo_t));
	if(!f)
		return NULL;

	f->buffer = malloc(size);
	if(!f->buffer)
	{
		free(f);
		return NULL;
	}
	f->size = size;
	f->in = 0;
	f->out = 0;
	spin_lock_init(&f->lock);

	return f;
}
EXPORT_SYMBOL(fifo_alloc);

void fifo_free(struct fifo_t * f)
{
	if(f)
	{
		free(f->buffer);
		free(f);
	}
}
EXPORT_SYMBOL(fifo_free);

void fifo_clear(struct fifo_t * f)
{
	irq_flags_t flags;

	if(f)
	{
		spin_lock_irqsave(&f->lock, flags);
		f->in = 0;
		f->out = 0;
		spin_unlock_irqrestore(&f->lock, flags);
	}
}
EXPORT_SYMBOL(fifo_clear);

bool_t fifo_isempty(struct fifo_t * f)
{
	irq_flags_t flags;
	bool_t ret = FALSE;

	if(!f)
		return TRUE;

	spin_lock_irqsave(&f->lock, flags);
	if(f->in - f->out <= 0)
		ret = TRUE;
	spin_unlock_irqrestore(&f->lock, flags);
	return ret;
}
EXPORT_SYMBOL(fifo_isempty);

bool_t fifo_isfull(struct fifo_t * f)
{
	irq_flags_t flags;
	bool_t ret = FALSE;

	if(!f)
		return TRUE;

	spin_lock_irqsave(&f->lock, flags);
	if(f->in - f->out >= f->size)
		ret = TRUE;
	spin_unlock_irqrestore(&f->lock, flags);
	return ret;
}
EXPORT_SYMBOL(fifo_isfull);

size_t fifo_avail(struct fifo_t * f)
{
	irq_flags_t flags;
	size_t ret = 0;

	if(f)
	{
		spin_lock_irqsave(&f->lock, flags);
		ret = f->in - f->out;
		spin_unlock_irqrestore(&f->lock, flags);
	}
	return ret;
}
EXPORT_SYMBOL(fifo_avail);

size_t fifo_put(struct fifo_t * f, u8_t * buf, size_t len)
{
	irq_flags_t flags;
	size_t ret = 0;
	size_t l;

	if(!f || !buf)
		return 0;

	spin_lock_irqsave(&f->lock, flags);
	ret = MIN(len, f->size - f->in + f->out);
	if(ret > 0)
	{
		l = MIN(ret, f->size - (f->in % f->size));
		memcpy(f->buffer + (f->in % f->size), buf, l);
		memcpy(f->buffer, buf + l, ret - l);
		f->in += ret;
	}
	else
	{
		ret = 0;
	}
	spin_unlock_irqrestore(&f->lock, flags);

	return ret;
}
EXPORT_SYMBOL(fifo_put);

size_t fifo_get(struct fifo_t * f, u8_t * buf, size_t len)
{
	irq_flags_t flags;
	size_t ret = 0;
	size_t l;

	if(!f || !buf)
		return 0;

	spin_lock_irqsave(&f->lock, flags);
	ret = MIN(len, f->in - f->out);
	if(ret > 0)
	{
		l = MIN(ret, f->size - (f->out % f->size));
		memcpy(buf, f->buffer + (f->out % f->size), l);
		memcpy(buf + l, f->buffer, ret - l);
		f->out += ret;
		if(f->in == f->out)
			f->in = f->out = 0;
	}
	else
	{
		ret = 0;
	}
	spin_unlock_irqrestore(&f->lock, flags);

	return ret;
}
EXPORT_SYMBOL(fifo_get);
