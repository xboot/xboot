#ifndef __SANDBOX_INPUT_H__
#define __SANDBOX_INPUT_H__

struct input_t {
	void (*cb_keyboard)(struct input_t * input);
	void (*cb_mouse)(struct input_t * input);

	void * data;
};

struct input_t * input_alloc(void);
void input_free(struct input_t * input);

#endif /* __SANDBOX_INPUT_H__ */
