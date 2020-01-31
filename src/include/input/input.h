#ifndef __INPUT_H__
#define __INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct input_t
{
	/* The input name */
	char * name;

	/* Ioctl interface */
	int (*ioctl)(struct input_t * input, const char * cmd, void * arg);

	/* Private data */
	void * priv;
};

struct input_t * search_input(const char * name);
struct device_t * register_input(struct input_t * input, struct driver_t * drv);
void unregister_input(struct input_t * input);
int input_ioctl(struct input_t * input, const char * cmd, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* __INPUT_H__ */
