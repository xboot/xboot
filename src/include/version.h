#ifndef __VERSION_H__
#define __VERSION_H__

#include <xboot.h>
#include <console/console.h>

/*
 * xboot's version
 */
#define	XBOOT_MAJOY					(1)
#define	XBOOT_MINIOR				(4)
#define	XBOOT_PATCH					(0)


s32_t xboot_banner(struct console * console);
void xboot_char_logo(struct console * console, u32_t x0, u32_t y0);

#endif /* __VERSION_H__ */
