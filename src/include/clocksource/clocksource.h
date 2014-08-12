#ifndef __CLOCKSOURCE_H__
#define __CLOCKSOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

#define CLOCKSOURCE_MASK(bits)		(u64_t)((bits) < 64 ? ((1ULL<<(bits))-1) : -1)

struct clocksource_t
{
	const char * name;
	int rating;
	u64_t last;
	u64_t mask;

	void (*init)(struct clocksource_t * cs);
	u64_t (*read)(struct clocksource_t * cs);

	struct kobj_t * kobj;
	void * pirv;
};

bool_t register_clocksource(struct clocksource_t * cs);
bool_t unregister_clocksource(struct clocksource_t * cs);

#ifdef __cplusplus
}
#endif

#endif /* __CLOCKSOURCE_H__ */

