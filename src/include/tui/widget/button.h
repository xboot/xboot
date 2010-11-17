#ifndef __TUI_BUTTON_H__
#define __TUI_BUTTON_H__

#include <configs.h>
#include <default.h>
#include <rect.h>
#include <tui/tui.h>

struct tui_button
{
	struct tui_widget widget;

	/* button caption */
	x_s8 * caption;

	/* caption's frontground color and background color */
	enum console_color cf, cb;

	/* widget body's front color and background color */
	enum console_color bf, bb;

	/* widget shadow's front color and background color */
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

#endif /* __TUI_BUTTON_H__ */
