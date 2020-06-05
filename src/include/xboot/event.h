#ifndef __EVENT_H__
#define __EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <fifo.h>
#include <xboot/ktime.h>

enum event_type_t {
	EVENT_TYPE_KEY_DOWN					= 0x0100,
	EVENT_TYPE_KEY_UP					= 0x0101,

	EVENT_TYPE_ROTARY_TURN				= 0x0200,

	EVENT_TYPE_MOUSE_DOWN				= 0x0300,
	EVENT_TYPE_MOUSE_MOVE				= 0x0301,
	EVENT_TYPE_MOUSE_UP					= 0x0302,
	EVENT_TYPE_MOUSE_WHEEL				= 0x0303,

	EVENT_TYPE_TOUCH_BEGIN				= 0x0400,
	EVENT_TYPE_TOUCH_MOVE				= 0x0401,
	EVENT_TYPE_TOUCH_END				= 0x0402,

	EVENT_TYPE_JOYSTICK_LEFTSTICK		= 0x0500,
	EVENT_TYPE_JOYSTICK_RIGHTSTICK		= 0x0501,
	EVENT_TYPE_JOYSTICK_LEFTTRIGGER		= 0x0502,
	EVENT_TYPE_JOYSTICK_RIGHTTRIGGER	= 0x0503,
	EVENT_TYPE_JOYSTICK_BUTTONDOWN		= 0x0504,
	EVENT_TYPE_JOYSTICK_BUTTONUP		= 0x0505,
};

enum {
	MOUSE_BUTTON_LEFT					= (0x1 << 0),
	MOUSE_BUTTON_RIGHT					= (0x1 << 1),
	MOUSE_BUTTON_MIDDLE					= (0x1 << 2),
	MOUSE_BUTTON_X1						= (0x1 << 3),
	MOUSE_BUTTON_X2						= (0x1 << 4),
};

enum {
	JOYSTICK_BUTTON_UP					= (0x1 << 0),
	JOYSTICK_BUTTON_DOWN				= (0x1 << 1),
	JOYSTICK_BUTTON_LEFT				= (0x1 << 2),
	JOYSTICK_BUTTON_RIGHT				= (0x1 << 3),
	JOYSTICK_BUTTON_A					= (0x1 << 4),
	JOYSTICK_BUTTON_B					= (0x1 << 5),
	JOYSTICK_BUTTON_X					= (0x1 << 6),
	JOYSTICK_BUTTON_Y					= (0x1 << 7),
	JOYSTICK_BUTTON_BACK				= (0x1 << 8),
	JOYSTICK_BUTTON_START				= (0x1 << 9),
	JOYSTICK_BUTTON_GUIDE				= (0x1 << 10),
	JOYSTICK_BUTTON_LBUMPER				= (0x1 << 11),
	JOYSTICK_BUTTON_RBUMPER				= (0x1 << 12),
	JOYSTICK_BUTTON_LSTICK				= (0x1 << 13),
	JOYSTICK_BUTTON_RSTICK				= (0x1 << 14),
};

struct event_t {
	void * device;
	enum event_type_t type;
	ktime_t timestamp;

	union {
		/* Key */
		struct {
			u32_t key;
		} key_down;

		struct {
			u32_t key;
		} key_up;

		/* Rotary */
		struct {
			s32_t v;
		} rotary_turn;

		/* Mouse */
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

		/* Touch */
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

		/* Joystick */
		struct {
			s32_t x, y;
		} joystick_left_stick;

		struct {
			s32_t x, y;
		} joystick_right_stick;

		struct {
			s32_t v;
		} joystick_left_trigger;

		struct {
			s32_t v;
		} joystick_right_trigger;

		struct {
			u32_t button;
		} joystick_button_down;

		struct {
			u32_t button;
		} joystick_button_up;
	} e;
};

void push_event_key_down(void * device, u32_t key);
void push_event_key_up(void * device, u32_t key);
void push_event_rotary_turn(void * device, s32_t v);
void push_event_mouse_button_down(void * device, s32_t x, s32_t y, u32_t button);
void push_event_mouse_button_up(void * device, s32_t x, s32_t y, u32_t button);
void push_event_mouse_move(void * device, s32_t x, s32_t y);
void push_event_mouse_wheel(void * device, s32_t dx, s32_t dy);
void push_event_touch_begin(void * device, s32_t x, s32_t y, u32_t id);
void push_event_touch_move(void * device, s32_t x, s32_t y, u32_t id);
void push_event_touch_end(void * device, s32_t x, s32_t y, u32_t id);
void push_event_joystick_left_stick(void * device, s32_t x, s32_t y);
void push_event_joystick_right_stick(void * device, s32_t x, s32_t y);
void push_event_joystick_left_trigger(void * device, s32_t v);
void push_event_joystick_right_trigger(void * device, s32_t v);
void push_event_joystick_button_down(void * device, u32_t button);
void push_event_joystick_button_up(void * device, u32_t button);

#ifdef __cplusplus
}
#endif

#endif /* __EVENT_H__ */
