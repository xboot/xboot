#ifndef __NOTIFIER_H__
#define __NOTIFIER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stddef.h>
#include <spinlock.h>

struct notifier_t {
	struct notifier_t * next;
	int priority;
	void * data;
	int (*call)(struct notifier_t * n, int cmd, void * arg);
};

struct notifier_chain_t {
	spinlock_t lock;
	struct notifier_t * head;
};

#define NOTIFIER_CHAIN_INIT() \
	{.lock = SPIN_LOCK_INIT(), .head = NULL}

void notifier_chain_init(struct notifier_chain_t * nc);
bool_t notifier_chain_register(struct notifier_chain_t * nc, struct notifier_t * n);
bool_t notifier_chain_unregister(struct notifier_chain_t * nc, struct notifier_t * n);
bool_t notifier_chain_call(struct notifier_chain_t * nc, int cmd, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* __NOTIFIER_H__ */
