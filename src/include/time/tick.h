#ifndef __TICK_H__
#define __TICK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <time/jiffies.h>
#include <time/timer.h>

struct tick_t {
	const int hz;
	bool_t (*init)(void);
};

static inline __attribute__((always_inline)) void tick_interrupt(void)
{
	jiffies++;
	schedule_timer_task();
}

bool_t register_tick(struct tick_t * tick);
struct tick_t * get_system_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* __TICK_H__ */
