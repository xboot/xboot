#ifndef __CLOCKEVENT_H__
#define __CLOCKEVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum clock_event_mode_t {
	CLOCK_EVENT_MODE_SHUTDOWN,
	CLOCK_EVENT_MODE_PERIODIC,
	CLOCK_EVENT_MODE_ONESHOT,
};

struct clockevent_t
{
	struct kobj_t * kobj;
	const char * name;
	u64_t min_delta_ns;
	u64_t max_delta_ns;
	void (*event_handler)(struct clockevent_t * ce);

	void (*init)(struct clockevent_t * ce);
	void (*set_mode)(struct clockevent_t * ce, enum clock_event_mode_t mode);
	int  (*set_next_ktime)(struct clockevent_t * ce, ktime_t expires);
	void * pirv;
};

bool_t register_clockevent(struct clockevent_t * ce);
bool_t unregister_clockevent(struct clockevent_t * ce);

#ifdef __cplusplus
}
#endif

#endif /* __CLOCKEVENT_H__ */

