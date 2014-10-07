#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot/module.h>
#include <types.h>
#include <spinlock.h>

struct fifo_t {
	u8_t * buffer;
	size_t size;
	size_t in;
	size_t out;
	spinlock_t lock;
};

struct fifo_t * fifo_alloc(size_t size);
void fifo_free(struct fifo_t * f);
void fifo_clear(struct fifo_t * f);
bool_t fifo_isempty(struct fifo_t * f);
bool_t fifo_isfull(struct fifo_t * f);
size_t fifo_avail(struct fifo_t * f);
size_t fifo_put(struct fifo_t * f, u8_t * buf, size_t len);
size_t fifo_get(struct fifo_t * f, u8_t * buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __FIFO_H__ */
