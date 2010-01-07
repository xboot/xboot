#ifndef __VERSION_H__
#define __VERSION_H__

#include <configs.h>
#include <default.h>
#include <terminal/terminal.h>

/*
 * xboot's version
 */
#define	XBOOT_MAJOY				(1)
#define	XBOOT_MINIOR			(1)
#define	XBOOT_PATCH				(0)


void xboot_banner(struct terminal * term);
void xboot_char_logo(struct terminal * term, x_u32 x0, x_u32 y0);

#endif /* __VERSION_H__ */
