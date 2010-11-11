#ifndef __VERSION_H__
#define __VERSION_H__

#include <configs.h>
#include <default.h>

/*
 * xboot's version
 */
#define	XBOOT_MAJOY					(1)
#define	XBOOT_MINIOR				(2)
#define	XBOOT_PATCH					(0)

void xboot_banner(void);
void xboot_char_logo(x_u32 x0, x_u32 y0);

#endif /* __VERSION_H__ */
