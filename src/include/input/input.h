#ifndef __INPUT_H__
#define __INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum input_type_t {
	INPUT_KEYBOARD			= 1,
	INPUT_MOUSE				= 2,
	INPUT_TOUCHSCREEN		= 3,
	INPUT_JOYSTICK			= 4,
	INPUT_ACCELEROMETER		= 5,
	INPUT_GYROSCOPE			= 6,
	INPUT_LIGHT				= 7,
	INPUT_MAGNETIC			= 8,
	INPUT_ORIENTATION		= 9,
	INPUT_PRESSURE			= 10,
	INPUT_PROXIMITY			= 11,
	INPUT_TEMPERATURE		= 12,
};

struct input_event_t {
	/* time stamp */
	u32_t time;

	/* input type */
	enum input_type_t type;

	/* event code */
	s32_t code;

	/* event value */
	s32_t value;
};

struct input_t
{
	/* input name */
	const char * name;

	/* input type */
	enum input_type_t type;

	/* probe input device */
	bool_t (*probe)(struct input_t * input);

	/* remove input device */
	bool_t (*remove)(struct input_t * input);

	/* ioctl input device */
	int (*ioctl)(struct input_t * input, int cmd, void * arg);

	/* private data */
	void * priv;
};

struct input_t * search_input(const char * name);
bool_t register_input(struct input_t * input);
bool_t unregister_input(struct input_t * input);

void input_report(enum input_type_t type, s32_t code, s32_t value);
void input_sync(enum input_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* __INPUT_H__ */
