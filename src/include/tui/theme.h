#ifndef __TUI_THEME_H__
#define __TUI_THEME_H__

#include <configs.h>
#include <default.h>
#include <tui/tui.h>

struct tui_theme
{
	struct {
		/* foreground color */
		enum console_color fg;

		/* background color */
		enum console_color bg;

		/* background character */
		x_u32 ch;
	} workspace;
};

struct tui_theme * get_tui_theme(void);

#endif /* __TUI_THEME_H__ */
