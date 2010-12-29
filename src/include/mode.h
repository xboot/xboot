#ifndef __MODE_H__
#define __MODE_H__

#include <configs.h>
#include <default.h>
#include <xboot.h>

/*
 * the system running mode type.
 */
enum mode {
	MODE_NORMAL,
	MODE_MENU,
	MODE_SHELL,
};

x_bool xboot_set_mode(enum mode m);
inline enum mode xboot_get_mode(void);

#endif /* __MODE_H__ */
