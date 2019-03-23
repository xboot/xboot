#ifndef __INPUT_H__
#define __INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum {
	INPUT_IOCTL_MOUSE_SET_RANGE				= 1,
	INPUT_IOCTL_MOUSE_GET_RANGE				= 2,
	INPUT_IOCTL_MOUSE_SET_SENSITIVITY		= 3,
	INPUT_IOCTL_MOUSE_GET_SENSITIVITY		= 4,
	INPUT_IOCTL_TOUCHSCEEN_SET_CALIBRATION	= 5,
	INPUT_IOCTL_TOUCHSCEEN_GET_CALIBRATION	= 6,
};

struct input_t
{
	/* The input name */
	char * name;

	/* Ioctl interface */
	int (*ioctl)(struct input_t * input, int cmd, void * arg);

	/* Private data */
	void * priv;
};

struct input_t * search_input(const char * name);
bool_t register_input(struct device_t ** device, struct input_t * input);
bool_t unregister_input(struct input_t * input);
int input_ioctl(struct input_t * input, int cmd, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* __INPUT_H__ */
