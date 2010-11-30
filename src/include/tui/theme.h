#ifndef __TUI_THEME_H__
#define __TUI_THEME_H__

#include <configs.h>
#include <default.h>
#include <tui/tui.h>

struct tui_theme
{
	struct {
		/* background code pointer */
		x_u32 cp;

		/* foreground color */
		enum tcolor fg;

		/* background color */
		enum tcolor bg;
	} workspace;

	struct {
		/* background code pointer */
		x_u32 cp;

		/* foreground color */
		enum tcolor fg;

		/* background color */
		enum tcolor bg;
	} button;
};

struct tui_theme * get_tui_theme(void);

#endif /* __TUI_THEME_H__ */
