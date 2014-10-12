#ifndef __POLLER_H__
#define __POLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

struct poller_t
{
	u64_t interval;
	u64_t timeout;
	void (*func)(struct poller_t * poller, void * data);
	void * data;
};

struct poller_t * poller_create(u64_t interval, void (*func)(struct poller_t *, void *), void * data);
bool_t poller_destroy(struct poller_t * poller);
void schedule_poller_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* __POLLER_H__ */
