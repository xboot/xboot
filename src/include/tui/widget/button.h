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

	/* caption's foreground color and background color */
	enum console_color cf, cb;

	/* widget body's foreground color and background color */
	enum console_color bf, bb;

	/* widget shadow's foreground color and background color */
	enum console_color sf, sb;

	/* align method */
	enum align align;

	/* shadow flag of button */
	x_bool shadow;

	/* enable flag of button */
	x_bool enable;

	/* visible flag of button */
	x_bool visible;
};


struct tui_button * new_tui_button(struct tui_widget * parent, x_s8 * caption);

#endif /* __TUI_BUTTON_H__ */
