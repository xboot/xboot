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
typedef void (*event_callback_t)(struct event_t * event, void * data);

enum event_type_t {
	/* Unknown */
	EVENT_TYPE_UNKNOWN				= 0,

	/* Mouse event */
	EVENT_TYPE_MOUSE_RAW			= 1000,

	/* Keyboard event */
	EVENT_TYPE_KEYBOARD_KEY_DOWN	= 2000,
	EVENT_TYPE_KEYBOARD_KEY_UP		= 2001,

	/* Mouse event */
	EVENT_TYPE_MOUSE_MOTION			= 3000,
	EVENT_TYPE_MOUSE_BUTTON_DOWN	= 3001,
	EVENT_TYPE_MOUSE_BUTTON_UP		= 3002,
	EVENT_TYPE_MOUSE_WHEEL			= 3003,
};

enum {
	MOUSE_BUTTON_LEFT				= (0x1 << 0),
	MOUSE_BUTTON_RIGHT				= (0x1 << 1),
	MOUSE_BUTTON_MIDDLE				= (0x1 << 2),
};

struct event_t {
	enum event_type_t type;
	u32_t timestamp;

	union {
		struct mouse_raw_event_t {
			u8_t btndown, btnup;
			s32_t xrel, yrel, zrel;
		} mouse_raw;

		struct keyboard_event_t {
			u32_t code;
		} keyboard;

		struct mouse_event_t {
			u32_t btndown;
			u32_t btnup;
			s32_t xrel, yrel, zrel;
			s32_t x, y, z;
		} mouse;
	} e;
};

struct event_watcher_t {
	enum event_type_t type;
	event_callback_t callback;
	void * data;
	struct list_head entry;
};

struct event_base_t {
	struct fifo_t * fifo;
	struct event_watcher_t * watcher;
	struct list_head entry;
};

struct event_base_t * __event_base_alloc(void);
void __event_base_free(struct event_base_t * eb);
bool_t event_base_add_watcher(struct event_base_t * eb, enum event_type_t type, event_callback_t callback, void * data);
bool_t event_base_del_watcher(struct event_base_t * eb, enum event_type_t type, event_callback_t callback);

bool_t event_push(struct event_t * event);
bool_t event_pop(struct event_t * event);
bool_t event_dispatch(void);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */
