#ifndef __TUI_THEME_H__
#define __TUI_THEME_H__

#include <configs.h>
#include <default.h>
#include <tui/tui.h>

struct tui_theme
{
	struct {
		x_u32 cp;
		enum tcolor fg, bg;
	} workspace;

	struct {
		x_u32 cp;
		enum tcolor fg, bg;

		enum tcolor c_fg, c_bg;

		x_u32 h, v, lt, rt, lb, rb;
		enum tcolor b_fg, b_bg;
	} button;
};

struct tui_theme * get_tui_theme(void);

#endif /* __TUI_THEME_H__ */
