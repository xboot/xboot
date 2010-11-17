#ifndef __TUI_BUTTON_H__
#define __TUI_BUTTON_H__

#include <configs.h>
#include <default.h>
#include <tui/tui.h>

struct tui_button
{
	struct tui_component component;

	x_s8 * caption;
	enum console_color fc;
	enum console_color bc;
	x_bool enable;
	x_bool visible;
};

#endif /* __TUI_BUTTON_H__ */
