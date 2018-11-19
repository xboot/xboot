#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <atomic.h>

struct semaphore_t {
	atomic_t value;
	struct list_head waiting;
};

void semaphore_init(struct semaphore_t * sem, int v);
void semaphore_post(struct semaphore_t * sem);
void semaphore_wait(struct semaphore_t * sem);

#ifdef __cplusplus
}
#endif

#endif /* __SEMAPHORE_H__ */
