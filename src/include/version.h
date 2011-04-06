#ifndef __VERSION_H__
#define __VERSION_H__

#include <configs.h>
#include <default.h>
#include <console/console.h>

/*
 * xboot's version
 */
#define	XBOOT_MAJOY					(1)
#define	XBOOT_MINIOR				(3)
#define	XBOOT_PATCH					(0)


x_s32 xboot_banner(struct console * console);
void xboot_char_logo(struct console * console, x_u32 x0, x_u32 y0);

#endif /* __VERSION_H__ */
