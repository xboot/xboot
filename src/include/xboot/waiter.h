#ifndef __WAITER_H__
#define __WAITER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <spinlock.h>

struct waiter_t {
	int count;
	spinlock_t lock;
};

void waiter_init(struct waiter_t * w);
void waiter_add(struct waiter_t * w, int v);
void waiter_sub(struct waiter_t * w, int v);
void waiter_wait(struct waiter_t * w);

#ifdef __cplusplus
}
#endif

#endif /* __WAITER_H__ */
