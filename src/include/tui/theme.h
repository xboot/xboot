#ifndef __TUI_THEME_H__
#define __TUI_THEME_H__

#include <configs.h>
#include <default.h>
#include <tui/tui.h>

struct tui_theme
{
	struct {
		enum console_color fg;
		enum console_color bg;
		x_u32 c;
	} workspace;
};

struct tui_theme * get_tui_theme(void);

#endif /* __TUI_THEME_H__ */
