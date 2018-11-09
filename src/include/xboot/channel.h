#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <fifo.h>
#include <irqflags.h>
#include <spinlock.h>
#include <xboot/task.h>

struct channel_t {
	struct fifo_t * fifo;
	struct list_head list;
	spinlock_t lock;
};

struct channel_t * channel_alloc(unsigned int size);
void channel_free(struct channel_t * c);

#ifdef __cplusplus
}
#endif

#endif /* __CHANNEL_H__ */
