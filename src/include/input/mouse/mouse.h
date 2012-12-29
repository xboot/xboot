#ifndef __MOUSE_H__
#define __MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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

bool_t install_listener_onmouseraw(handler_onmouseraw raw);
bool_t remove_listener_onmouseraw(handler_onmouseraw raw);

#ifdef __cplusplus
}
#endif

#endif /* __MOUSE_H__ */
