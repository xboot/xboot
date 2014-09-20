#ifndef __CLOCKSOURCE_H__
#define __CLOCKSOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

#define CLOCKSOURCE_MASK(bits)	(u64_t)((bits) < 64 ? ((1ULL<<(bits))-1) : -1)

struct clocksource_t
{
	struct kobj_t * kobj;
	const char * name;
	u32_t shift;
	u32_t mult;
	u64_t mask;
	u64_t last;
	u64_t time;

	void (*init)(struct clocksource_t * cs);
	u64_t (*read)(struct clocksource_t * cs);
	void * pirv;
};

bool_t register_clocksource(struct clocksource_t * cs);
bool_t unregister_clocksource(struct clocksource_t * cs);
bool_t init_system_clocksource(void);

u32_t clocksource_hz2mult(u32_t hz, u32_t shift);
u64_t clocksource_gettime(void);

#ifdef __cplusplus
}
#endif

#endif /* __CLOCKSOURCE_H__ */

