#ifndef __TUI_BUTTON_H__
#define __TUI_BUTTON_H__

#include <configs.h>
#include <default.h>
#include <rect.h>
#include <tui/tui.h>

enum {
	TUI_BUTTON_SET_CAPTION			= 0,
	TUI_BUTTON_GET_CAPTION			= 1,
};

struct tui_button
{
	/* tui widget */
	struct tui_widget widget;

	/* button caption */
	x_s8 * caption;
};

struct tui_button * tui_button_new(struct tui_widget * parent, const x_s8 * id, const x_s8 * caption);

#endif /* __TUI_BUTTON_H__ */
