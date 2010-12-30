#ifndef __MODE_H__
#define __MODE_H__

#include <configs.h>
#include <default.h>
#include <xboot.h>

/*
 * the type of system running mode.
 */
enum mode {
	MODE_NORMAL		= 0,
	MODE_SHELL		= 1,
	MODE_MENU		= 2,
	MODE_GRAPHIC	= 3,
};

x_bool xboot_set_mode(enum mode m);
inline enum mode xboot_get_mode(void);

#endif /* __MODE_H__ */
