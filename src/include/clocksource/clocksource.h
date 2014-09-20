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

struct clocksource_t * get_clocksource(void);
void clocks_calc_mult_shift(u32_t * mult, u32_t * shift, u32_t from, u32_t to, u32_t maxsec);
u32_t clocksource_hz2mult(u32_t hz, u32_t shift);
bool_t register_clocksource(struct clocksource_t * cs);
bool_t unregister_clocksource(struct clocksource_t * cs);
bool_t init_system_clocksource(void);

u64_t clocksource_gettime(void);
bool_t is_timeout(u64_t start, u64_t offset);
void udelay(u32_t us);
void mdelay(u32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __CLOCKSOURCE_H__ */

