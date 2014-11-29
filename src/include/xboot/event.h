#ifndef __EVENT_H__
#define __EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum event_type_t {
	EVENT_TYPE_UNKNOWN				= 0x0000,

	EVENT_TYPE_KEY_DOWN				= 0x0100,
	EVENT_TYPE_KEY_UP				= 0x0101,

	EVENT_TYPE_MOUSE_DOWN			= 0x0200,
	EVENT_TYPE_MOUSE_MOVE			= 0x0201,
	EVENT_TYPE_MOUSE_UP				= 0x0202,
	EVENT_TYPE_MOUSE_WHEEL			= 0x0203,

	EVENT_TYPE_TOUCH_BEGIN			= 0x0300,
	EVENT_TYPE_TOUCH_MOVE			= 0x0301,
	EVENT_TYPE_TOUCH_END			= 0x0302,
};

enum {
	MOUSE_BUTTON_LEFT				= (0x1 << 0),
	MOUSE_BUTTON_RIGHT				= (0x1 << 1),
	MOUSE_BUTTON_MIDDLE				= (0x1 << 2),
};

struct event_t {
	void * device;
	enum event_type_t type;
	u32_t timestamp;

	union {
		/* key */
		struct {
			u32_t key;
		} key_down;

		struct {
			u32_t key;
		} key_up;

		/* mouse */
		struct {
			s32_t x, y;
			u32_t button;
		} mouse_down;

		struct {
			s32_t x, y;
		} mouse_move;

		struct {
			s32_t x, y;
			u32_t button;
		} mouse_up;

		struct {
			s32_t dx, dy;
		} mouse_wheel;

		/* touch */
		struct {
			s32_t x, y;
			u32_t id;
		} touch_begin;

		struct {
			s32_t x, y;
			u32_t id;
		} touch_move;

		struct {
			s32_t x, y;
			u32_t id;
		} touch_end;
	} e;
};

struct event_base_t {
	struct fifo_t * fifo;
	struct list_head entry;
};

struct event_base_t * __event_base_alloc(void);
void __event_base_free(struct event_base_t * eb);

void push_event(struct event_t * event);
void push_event_key_down(void * device, u32_t key);
void push_event_key_up(void * device, u32_t key);
void push_event_mouse_button_down(void * device, s32_t x, s32_t y, u32_t btn);
void push_event_mouse_button_up(void * device, s32_t x, s32_t y, u32_t btn);
void push_event_mouse_move(void * device, s32_t x, s32_t y);
void push_event_mouse_wheel(void * device, s32_t dx, s32_t dy);
void push_event_touch_begin(void * device, s32_t x, s32_t y, u32_t id);
void push_event_touch_move(void * device, s32_t x, s32_t y, u32_t id);
void push_event_touch_end(void * device, s32_t x, s32_t y, u32_t id);
bool_t pump_event(struct event_base_t * eb, struct event_t * event);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */
