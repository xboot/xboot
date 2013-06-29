#ifndef __EVENT_H__
#define __EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum event_type_t {
	EVENT_TYPE_UNKNOWN				= 0,

	EVENT_TYPE_KEY_DOWN				= 1000,
	EVENT_TYPE_KEY_UP					= 1001,

	EVENT_TYPE_MOUSE_DOWN			= 2000,
	EVENT_TYPE_MOUSE_MOVE			= 2001,
	EVENT_TYPE_MOUSE_UP				= 2002,

	EVENT_TYPE_TOUCHES_BEGAN			= 3000,
	EVENT_TYPE_TOUCHES_MOVE			= 3001,
	EVENT_TYPE_TOUCHES_END			= 3002,
	EVENT_TYPE_TOUCHES_CANCEL		= 3003,
};

enum {
	MOUSE_BUTTON_LEFT					= (0x1 << 0),
	MOUSE_BUTTON_RIGHT				= (0x1 << 1),
};

struct event_t {
	enum event_type_t type;
	u32_t timestamp;

	union {
		struct key_event_t {
			u32_t code;
		} key;

		struct mouse_event_t {
			s32_t x, y;
			u8_t button;
		} mouse;
	} e;
};

struct event_base_t {
	struct fifo_t * fifo;
	struct list_head entry;
};

struct event_base_t * __event_base_alloc(void);
void __event_base_free(struct event_base_t * eb);
void push_event(struct event_t * event);
void push_event_mounse(u8_t btndown, u8_t btnup, s32_t relx, s32_t rely);
struct event_t * peek_event(void);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */
