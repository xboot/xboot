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
};

struct tui_workspace * tui_workspace_new(struct console * console, const x_s8 * id);

#endif /* __TUI_WORKSPACE_H__ */
