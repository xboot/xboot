#ifndef __MODE_H__
#define __MODE_H__

#include <configs.h>
#include <default.h>
#include <xboot.h>


typedef void (*application_t)(void);

/*
 * the type of system running mode.
 */
enum mode {
	MODE_NORMAL			= 0,
	MODE_SHELL			= 1,
	MODE_MENU			= 2,
	MODE_GRAPHIC		= 3,
	MODE_APPLICATION	= 4,
};

inline enum mode xboot_get_mode(void);
x_bool xboot_set_mode(enum mode m);

void run_normal_mode(void);
void run_shell_mode(void);
void run_menu_mode(void);
void run_graphic_mode(void);
void run_application_mode(void);

x_bool register_application(application_t app);

#endif /* __MODE_H__ */
