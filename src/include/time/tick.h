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

/*
 * The struct of tick.
 */
struct tick {
	/* system hz */
	const u32_t hz;

	/* initialize system tick */
	bool_t (*init)(void);
};

void tick_interrupt(void);
bool_t register_tick(struct tick * tick);
u32_t get_system_hz(void);
bool_t init_system_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* __TICK_H__ */
