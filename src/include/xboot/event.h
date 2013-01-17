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
struct event_listener_t;
struct event_base_t;

/*
 * Event listener callback
 */
typedef void (*event_listener_callback_t)(struct event_t * event, void * data);

enum event_type_t {
	EVENT_TYPE_UNKNOWN				= 0,

	EVENT_TYPE_MOUSE_RAW			= 1000,
	/*
	EVENT_TYPE_KEYBOARD_KEY_DOWN	= 1000,
	EVENT_TYPE_KEYBOARD_KEY_UP		= 1001,

	EVENT_TYPE_MOUSE_MOTION			= 2001,
	EVENT_TYPE_MOUSE_BUTTON_DOWN	= 2002,
	EVENT_TYPE_MOUSE_BUTTON_UP		= 2003,
	EVENT_TYPE_MOUSE_WHEEL			= 2004,
*/
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

/*		struct keyboard_event_t {
			u32_t code;
		} keyboard;*/
	} e;
};

struct event_listener_t {
	enum event_type_t type;
	event_listener_callback_t callback;
	void * data;
	struct list_head entry;
};

struct event_base_t {
	struct fifo_t * fifo;
	struct event_listener_t * listener;
	struct list_head entry;
};

struct event_base_t * __event_base_alloc(void);
void __event_base_free(struct event_base_t * eb);
struct event_listener_t * event_listener_alloc(enum event_type_t type, 	event_listener_callback_t callback, void * data);
void event_listener_free(struct event_listener_t * el);
bool_t event_base_add_event_listener(struct event_base_t * eb, struct event_listener_t * el);
bool_t event_base_del_event_listener(struct event_base_t * eb, struct event_listener_t * el);
bool_t event_base_dispatcher(struct event_base_t * eb);
bool_t event_send(struct event_t * event);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */
