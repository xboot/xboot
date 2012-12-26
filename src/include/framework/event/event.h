#ifndef __FRAMEWORK_EVENT_H__
#define __FRAMEWORK_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum event_type_t {
	/* Keyboard event */
	EVENT_TYPE_KEYBOARD_KEY_DOWN,
	EVENT_TYPE_KEYBOARD_KEY_UP,

	/* Mouse event */
	EVENT_TYPE_MOUSE_MOTION,
	EVENT_TYPE_MOUSE_BUTTON_DOWN,
	EVENT_TYPE_MOUSE_BUTTON_UP,
	EVENT_TYPE_MOUSE_WHEEL,
};

struct event_t {
	/* Event type */
	enum event_type_t type;

	/* Time stamp */
	u32_t timestamp;

	/* Event ... */
	union {
		struct keyboard_event_t {
		} keyboard;

		struct mouse_event_t {
		} mouse;
	} e;
};

struct event_base_t {
	struct fifo * fifo;
	struct list_head entry;
};


struct event_base_t * __event_base_alloc(void);
void __event_base_free(struct event_base_t * eb);
bool_t event_push(struct event_t * event);
bool_t event_pop(struct event_t * event);
bool_t event_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_EVENT_H__ */
