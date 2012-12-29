#ifndef __MODE_H__
#define __MODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>

typedef void (*application_t)(void);

/*
 * the type of system running mode.
 */
enum mode_t {
	MODE_NORMAL			= 0,
	MODE_SHELL			= 1,
	MODE_MENU			= 2,
	MODE_GRAPHIC		= 3,
	MODE_APPLICATION	= 4,
};

inline enum mode_t xboot_get_mode(void);
bool_t xboot_set_mode(enum mode_t m);

void run_normal_mode(void);
void run_shell_mode(void);
void run_menu_mode(void);
void run_graphic_mode(void);
void run_application_mode(void);

bool_t register_application(application_t app);

#ifdef __cplusplus
}
#endif

#endif /* __MODE_H__ */
