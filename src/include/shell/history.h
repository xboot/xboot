#ifndef __HISTORY_H__
#define __HISTORY_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the list of history.
 */
struct history_list
{
	x_s8 * cmdline;
	struct list_head entry;
};


x_bool history_add(const x_s8 * cmdline);
x_bool history_remove(void);
x_s32 history_numberof(void);
struct history_list * get_history_list(void);
x_s8 * history_get_next_cmd(void);
x_s8 * history_get_prev_cmd(void);


#endif /* __HISTORY_H__ */
