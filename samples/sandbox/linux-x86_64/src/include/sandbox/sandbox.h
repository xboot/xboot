#ifndef __SANDBOX_H__
#define __SANDBOX_H__

#include <sandbox/display.h>
#include <sandbox/input.h>

struct sandbox_t {
	struct display_t * display;
	struct input_t * input;
};

struct sandbox_t * sandbox_alloc(void);
void sandbox_free(struct sandbox_t * sandbox);

#endif /* __SANDBOX_H__ */
