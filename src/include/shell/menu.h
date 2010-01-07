#ifndef __MENU_H__
#define __MENU_H__

#include <configs.h>
#include <default.h>

/*
 * menu item struct.
 */
struct menu_item {
	char * name;
	char * context;
};

/*
 * run menu routline.
 */
void run_menu_mode(void);

/*
 * run normal routline.
 */
void run_normal_mode(void);


#endif /* __MENU_H__ */
