#ifndef __CLOCKSOURCE_H__
#define __CLOCKSOURCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/*
 * Base type of clocksource cycle
 */
typedef u64_t	cycle_t;

/*
 * Simplify initialization of mask field
 */
#define CLOCKSOURCE_MASK(bits)	(cycle_t)((bits) < 64 ? ((1ULL<<(bits))-1) : -1)

struct clocksource_t
{
	struct kobj_t * kobj;
	const char * name;
	u32_t mult;
	u32_t shift;
	cycle_t mask;
	cycle_t last;
	u64_t usec;

	void (*init)(struct clocksource_t * cs);
	cycle_t (*read)(struct clocksource_t * cs);
	void * pirv;
};

u64_t clocksource_gettime(void);
u32_t clocksource_hz2mult(u32_t hz, u32_t shift);
void clocksource_calc_mult_shift(u32_t * mult, u32_t * shift, u32_t from, u32_t to, u32_t maxsec);
bool_t register_clocksource(struct clocksource_t * cs);
bool_t unregister_clocksource(struct clocksource_t * cs);

void subsys_init_clocksource(void);

#ifdef __cplusplus
}
#endif

#endif /* __CLOCKSOURCE_H__ */

