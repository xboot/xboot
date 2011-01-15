#ifndef __S5PV210_KEYPAD_H__
#define __S5PV210_KEYPAD_H__

#include <configs.h>
#include <default.h>
#include <input/keyboard/keyboard.h>

struct s5pv210_keypad
{
	x_u32 rows;
	x_u32 cols;
	enum key_code keycode[8 * 14];
};

#endif /* __S5PV210_KEYPAD_H__ */
