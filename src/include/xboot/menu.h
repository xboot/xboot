#ifndef __MENU_H__
#define __MENU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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

bool_t menu_load(char * file);

inline struct menu_list * get_menu_list(void);
struct menu_item * get_menu_indexof_item(s32_t index);
s32_t get_menu_total_items(void);

#ifdef __cplusplus
}
#endif

#endif /* __MENU_H__ */
