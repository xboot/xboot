#ifndef __TUI_WORKSPACE_H__
#define __TUI_WORKSPACE_H__

#include <configs.h>
#include <default.h>
#include <rect.h>
#include <tui/tui.h>

struct tui_workspace
{
	/* tui widget */
	struct tui_widget widget;

	/* workspace's foreground color and background color */
	enum console_color fg, bg;
};


struct tui_workspace * new_tui_workspace(struct console * console);

#endif /* __TUI_WORKSPACE_H__ */
