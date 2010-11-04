#ifndef __INPUT_H__
#define __INPUT_H__

#include <configs.h>
#include <default.h>

enum input_type {
	INPUT_KEYBOARD		= 1,
	INPUT_MOUSE			= 2,
	INPUT_TOUCHSCREEN	= 3,
	INPUT_JOYSTICK		= 4,
	INPUT_SENSOR		= 5,
};

struct input_event {
	x_u32 time;
	enum input_type type;
	x_u32 code;
	x_u32 value;
};

void input_report(enum input_type type, x_u32 code, x_u32 value);
void input_sync(enum input_type type);

#endif /* __INPUT_H__ */
