#ifndef __KREF_H__
#define __KREF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <atomic.h>

struct kref_t {
	atomic_t count;
};

static inline void kref_init(struct kref_t * kref)
{
	atomic_set(&kref->count, 0);
}

static inline void kref_get(struct kref_t * kref)
{
	atomic_inc(&kref->count);
}

static inline int kref_sub(struct kref_t * kref, unsigned int count, void (*release)(struct kref_t * kref))
{
	if(atomic_sub_and_test(&kref->count, (int)count))
	{
		if(release)
			release(kref);
		return 1;
	}
	return 0;
}

static inline int kref_put(struct kref_t * kref, void (*release)(struct kref_t * kref))
{
	return kref_sub(kref, 1, release);
}

#ifdef __cplusplus
}
#endif

#endif /* __KREF_H__ */
