#ifndef __KREF_H__
#define __KREF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <atomic.h>

struct kref_t {
	atomic_t count;
};

static inline void kref_init(struct kref_t * ref)
{
	atomic_set(&ref->count, 1);
}

static inline void kref_get(struct kref_t * ref)
{
	atomic_inc(&ref->count);
}

static inline int kref_sub(struct kref_t * ref, unsigned int count, void (*release)(struct kref_t * ref))
{
	if(atomic_sub_and_test((int)count, &ref->count))
	{
		if(release)
			release(ref);
		return 1;
	}
	return 0;
}

static inline int kref_put(struct kref_t * ref, void (*release)(struct kref_t * ref))
{
	return kref_sub(ref, 1, release);
}

#ifdef __cplusplus
}
#endif

#endif /* __KREF_H__ */
