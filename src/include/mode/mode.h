#ifndef __MODE_H__
#define __MODE_H__

#include <configs.h>
#include <default.h>
#include <xboot.h>

/*
 * the type of system running mode.
 */
enum mode {
	MODE_NORMAL			= 0,
	MODE_SHELL			= 1,
	MODE_MENU			= 2,
	MODE_GRAPHIC		= 3,
};

inline enum mode xboot_get_mode(void);
x_bool xboot_set_mode(enum mode m);

void run_normal_mode(void);
void run_shell_mode(void);
void run_menu_mode(void);
void run_graphic_mode(void);

#endif /* __MODE_H__ */
