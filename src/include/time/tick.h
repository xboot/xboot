#ifndef __TICK_H__
#define __TICK_H__


#include <configs.h>
#include <default.h>

extern x_u32 jiffies;

/*
 * the struct of tick.
 */
struct tick {
	/* system hz */
	const x_u32 hz;

	/* initialize system tick */
	x_bool (*init)(void);
};

void tick_interrupt(void);
x_bool register_tick(struct tick * tick);
x_u32 get_system_hz(void);
x_bool init_system_tick(void);

#endif /* __TICK_H__ */
