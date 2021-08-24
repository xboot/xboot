#ifndef __MUTEX_H__
#define __MUTEX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <atomic.h>

struct mutex_t {
	atomic_t atomic;
};

void mutex_init(struct mutex_t * m);
void mutex_lock(struct mutex_t * m);
void mutex_unlock(struct mutex_t * m);

#ifdef __cplusplus
}
#endif

#endif /* __MUTEX_H__ */
