#ifndef __TIMER_H__
#define __TIMER_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot/list.h>


/*
 * timer_list struct.
 */
struct timer_list {
	struct list_head list;
	x_u32 expires;
	x_u32 data;
	void (*function)(x_u32);
};


void init_timer(struct timer_list * timer);
x_bool timer_pending(const struct timer_list * timer);
void add_timer(struct timer_list *timer);
x_bool mod_timer(struct timer_list *timer, x_u32 expires);
x_bool del_timer(struct timer_list * timer);
void setup_timer(struct timer_list * timer,	void (*function)(x_u32), x_u32 data);

#endif /* __TIMER_LIST_H__ */
