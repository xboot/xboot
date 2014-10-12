#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <xboot/list.h>

/*
 * timer_t struct.
 */
struct timer_t {
	struct list_head list;
	u32_t expires;
	u32_t data;
	void (*function)(u32_t);
};

void init_timer(struct timer_t * timer);
bool_t timer_pending(const struct timer_t * timer);
void add_timer(struct timer_t * timer);
bool_t mod_timer(struct timer_t * timer, u32_t expires);
bool_t del_timer(struct timer_t * timer);
void setup_timer(struct timer_t * timer, void (*function)(u32_t), u32_t data);

void schedule_timer_task(void);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_LIST_H__ */
