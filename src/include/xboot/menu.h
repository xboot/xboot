#ifndef __MENU_H__
#define __MENU_H__

#include <configs.h>
#include <default.h>
#include <xboot/list.h>


struct menu_item {
	char * title;
	char * command;
};

struct menu_list
{
	struct menu_item * item;
	struct list_head entry;
};

x_bool menu_load(char * file);

inline struct menu_list * get_menu_list(void);
struct menu_item * get_menu_indexof_item(x_s32 index);
x_s32 get_menu_total_items(void);

#endif /* __MENU_H__ */
