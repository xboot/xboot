#ifndef __TUI_BUTTON_H__
#define __TUI_BUTTON_H__

#include <configs.h>
#include <default.h>
#include <rect.h>
#include <tui/tui.h>

struct tui_button
{
	/* tui widget */
	struct tui_widget widget;

	/* button caption */
	x_s8 * caption;

	/* shadow flag */
	x_bool shadow;
};

struct tui_button * tui_button_new(struct tui_widget * parent, const x_s8 * id, const x_s8 * caption);

#endif /* __TUI_BUTTON_H__ */
