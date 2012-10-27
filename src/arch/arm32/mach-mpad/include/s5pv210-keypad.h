#ifndef __S5PV210_KEYPAD_H__
#define __S5PV210_KEYPAD_H__

#include <xboot.h>
#include <input/keyboard/keyboard.h>

struct s5pv210_keypad
{
	u32_t rows;
	u32_t cols;
	enum key_code keycode[8 * 14];
};

#endif /* __S5PV210_KEYPAD_H__ */
