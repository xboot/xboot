#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <configs.h>
#include <default.h>
#include <input/input.h>

enum mouse_code {
	MOUSE_LEFT					= 1,
	MOUSE_RIGHT					= 2,
	MOUSE_MIDDLE				= 3,
	MOUSE_REL_X					= 4,
	MOUSE_REL_Y					= 5,
	MOUSE_REL_Z					= 6,
};

enum mouse_value {
	MOUSE_BUTTON_UP				= 0,
	MOUSE_BUTTON_DOWN			= 1,
};

typedef void (*handler_onmouseraw)(struct input_event * event);

x_bool install_listener_onmouseraw(handler_onmouseraw raw);
x_bool remove_listener_onmouseraw(handler_onmouseraw raw);

#endif /* __MOUSE_H__ */
