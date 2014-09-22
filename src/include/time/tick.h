#ifndef __TICK_H__
#define __TICK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/*
 * When HZ = 1000, The Max delay is 24 days
 */
#define time_after(a, b)		(((s32_t)(b) - (s32_t)(a) < 0))
#define time_before(a, b)		time_after(b, a)
#define time_after_eq(a,b)		(((s32_t)(a) - (s32_t)(b) >= 0))
#define time_before_eq(a,b)		time_after_eq(b, a)

extern volatile u32_t jiffies;
extern volatile u32_t HZ;

struct tick_t {
	const u32_t hz;
	bool_t (*init)(void);
};

void tick_interrupt(void);
u64_t jiffies_to_msecs(const u64_t j);
u64_t jiffies_to_usecs(const u64_t j);
u64_t msecs_to_jiffies(const u64_t m);
u64_t usecs_to_jiffies(const u64_t u);
bool_t register_tick(struct tick_t * tick);

void subsys_init_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* __TICK_H__ */
