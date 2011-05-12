#ifndef __TICK_H__
#define __TICK_H__


#include <configs.h>
#include <default.h>

extern u32_t jiffies;

/*
 * the struct of tick.
 */
struct tick {
	/* system hz */
	const u32_t hz;

	/* initialize system tick */
	bool_t (*init)(void);
};

void tick_interrupt(void);
bool_t register_tick(struct tick * tick);
inline u32_t get_system_hz(void);
bool_t init_system_tick(void);

#endif /* __TICK_H__ */
