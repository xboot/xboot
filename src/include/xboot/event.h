#ifndef __EVENT_H__
#define __EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/*
 * Forward declare
 */
enum event_type_t;
struct event_t;
struct event_watcher_t;
struct event_base_t;

/*
 * Event callback function
 */
typedef void (*event_callback_t)(struct event_t * event);

enum event_type_t {
	/* Unknown */
	EVENT_TYPE_UNKNOWN,

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
	enum event_type_t type;
	u32_t timestamp;

	union {
		struct keyboard_event_t {
		} keyboard;

		struct mouse_event_t {
		} mouse;
	} e;
};

struct event_watcher_t {
	enum event_type_t type;
	event_callback_t callback;
	struct list_head entry;
};

struct event_base_t {
	struct fifo_t * fifo;
	struct event_watcher_t * watcher;
	struct list_head entry;
};

struct event_base_t * __event_base_alloc(void);
void __event_base_free(struct event_base_t * eb);
bool_t event_base_add_watcher(struct event_base_t * eb, enum event_type_t type, event_callback_t callback);
bool_t event_base_del_watcher(struct event_base_t * eb, enum event_type_t type, event_callback_t callback);

bool_t event_push(struct event_t * event);
bool_t event_pop(struct event_t * event);
bool_t event_dispatch(void);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */
