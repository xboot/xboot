/*
 * driver/realview_keyboard.c
 *
 * realview keyboard drivers, the primecell pl050 ps2 controller.
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <input/input.h>
#include <input/keyboard/keyboard.h>
#include <realview/reg-keyboard.h>

#define	KBD_SHIFT					0x00000001
#define	KBD_CTRL					0x00000002
#define	KBD_ALT						0x00000004
#define	KBD_CAPS_LOCK				0x00000008
#define	KBD_NUM_LOCK				0x00000001
#define	KBD_SCROLL_LOCK				0x00000002

#define SCANCODE_LEFT_SHIFT			0x2a
#define SCANCODE_RIGHT_SHIFT		0x36
#define SCANCODE_LEFT_CTRL			0x1d
#define SCANCODE_RIGHT_CTRL			0x1d
#define SCANCODE_LEFT_ALT			0x38
#define SCANCODE_RIGHT_ALT			0x38
#define SCANCODE_CAPS_LOCK			0x3a
#define SCANCODE_NUM_LOCK			0x45
#define SCANCODE_SCROLL_LOCK		0x46

struct keymap {
	x_u8 data;
	enum key_code key;
	enum key_code caps_key;
	enum key_code shift_key;
	enum key_code ctrl_key;
	enum key_code alt_key;
};

static const struct keymap map[] = {
/*	code	normal-key					caps-key					shift-key					control-key				alt-key	*/
	{0x1e,	KEY_a,						KEY_A,						KEY_A,						KEY_CTRL_A,				KEY_ALT_A},
	{0x30,	KEY_b,						KEY_B,						KEY_B,						KEY_CTRL_B,				KEY_ALT_B},
	{0x2e,	KEY_c,						KEY_C,						KEY_C,						KEY_CTRL_C,				KEY_ALT_C},
	{0x20,	KEY_d,						KEY_D,						KEY_D,						KEY_CTRL_D,				KEY_ALT_D},
	{0x12,	KEY_e,						KEY_E,						KEY_E,						KEY_CTRL_E,				KEY_ALT_E},
	{0x21,	KEY_f,						KEY_F,						KEY_F,						KEY_CTRL_F,				KEY_ALT_F},
	{0x22,	KEY_g,						KEY_G,						KEY_G,						KEY_CTRL_G,				KEY_ALT_G},
	{0x23,	KEY_h,						KEY_H,						KEY_H,						KEY_CTRL_H,				KEY_ALT_H},
	{0x17,	KEY_i,						KEY_I,						KEY_I,						KEY_CTRL_I,				KEY_ALT_I},
	{0x24,	KEY_j,						KEY_J,						KEY_J,						KEY_CTRL_J,				KEY_ALT_J},
	{0x25,	KEY_k,						KEY_K,						KEY_K,						KEY_CTRL_K,				KEY_ALT_K},
	{0x26,	KEY_l,						KEY_L,						KEY_L,						KEY_CTRL_L,				KEY_ALT_L},
	{0x32,	KEY_m,						KEY_M,						KEY_M,						KEY_CTRL_M,				KEY_ALT_M},
	{0x31,	KEY_n,						KEY_N,						KEY_N,						KEY_CTRL_N,				KEY_ALT_N},
	{0x18,	KEY_o,						KEY_O,						KEY_O,						KEY_CTRL_O,				KEY_ALT_O},
	{0x19,	KEY_p,						KEY_P,						KEY_P,						KEY_CTRL_P,				KEY_ALT_P},
	{0x10,	KEY_q,						KEY_Q,						KEY_Q,						KEY_CTRL_Q,				KEY_ALT_Q},
	{0x13,	KEY_r,						KEY_R,						KEY_R,						KEY_CTRL_R,				KEY_ALT_R},
	{0x1f,	KEY_s,						KEY_S,						KEY_S,						KEY_CTRL_S,				KEY_ALT_S},
	{0x14,	KEY_t,						KEY_T,						KEY_T,						KEY_CTRL_T,				KEY_ALT_T},
	{0x16,	KEY_u,						KEY_U,						KEY_U,						KEY_CTRL_U,				KEY_ALT_U},
	{0x2f,	KEY_v,						KEY_V,						KEY_V,						KEY_CTRL_V,				KEY_ALT_V},
	{0x11,	KEY_w,						KEY_W,						KEY_W,						KEY_CTRL_W,				KEY_ALT_W},
	{0x2d,	KEY_x,						KEY_X,						KEY_X,						KEY_CTRL_X,				KEY_ALT_X},
	{0x15,	KEY_y,						KEY_Y,						KEY_Y,						KEY_CTRL_Y,				KEY_ALT_Y},
	{0x2c,	KEY_z,						KEY_Z,						KEY_Z,						KEY_CTRL_Z,				KEY_ALT_Z},

	{0x0b,	KEY_0,						KEY_0,						KEY_ROUND_BRACKET_RIGHT,	KEY_CTRL_0,				KEY_ALT_0},
	{0x02,	KEY_1,						KEY_1,						KEY_EXCLAMATION_MARK,		KEY_CTRL_1,				KEY_ALT_1},
	{0x03,	KEY_2,						KEY_2,						KEY_AT,						KEY_CTRL_2,				KEY_ALT_2},
	{0x04,	KEY_3,						KEY_3,						KEY_POUNDSIGN,				KEY_CTRL_3,				KEY_ALT_3},
	{0x05,	KEY_4,						KEY_4,						KEY_DOLLAR,					KEY_CTRL_4,				KEY_ALT_4},
	{0x06,	KEY_5,						KEY_5,						KEY_PERCENT,				KEY_CTRL_5,				KEY_ALT_5},
	{0x07,	KEY_6,						KEY_6,						KEY_POWER,					KEY_CTRL_6,				KEY_ALT_6},
	{0x08,	KEY_7,						KEY_7,						KEY_AND,					KEY_CTRL_7,				KEY_ALT_7},
	{0x09,	KEY_8,						KEY_8,						KEY_STAR,					KEY_CTRL_8,				KEY_ALT_8},
	{0x0a,	KEY_9,						KEY_9,						KEY_ROUND_BRACKET_LEFT,		KEY_CTRL_9,				KEY_ALT_9},

	{0x29,	KEY_UNQUOTE,				KEY_UNQUOTE,				KEY_WAVE,					0,						0},
	{0x0c,	KEY_MINUS,					KEY_MINUS,					KEY_UNDERLINE,				0,						0},
	{0x0d,	KEY_EQUAL,					KEY_EQUAL,					KEY_PLUS,					0,						0},
	{0x2b,	KEY_BACKLASH,				KEY_BACKLASH,				KEY_OR,						0,						0},
	{0x0e,	KEY_BACKSPACE,				KEY_BACKSPACE,				KEY_BACKSPACE,				0,						0},
	{0x39,	KEY_SPACE,					KEY_SPACE,					KEY_SPACE,					0,						0},
	{0x0f,	KEY_TAB,					KEY_TAB,					KEY_TAB,					0,						0},
	{0x1c,	KEY_ENTER,					KEY_ENTER,					KEY_ENTER,					0,						0},
	{0x01,	KEY_ESC,					KEY_ESC,					KEY_ESC,					0,						0},

	{0x3b,	KEY_F1,						KEY_F1,						0,							0,						0},
	{0x3c,	KEY_F2,						KEY_F2,						0,							0,						0},
	{0x3d,	KEY_F3,						KEY_F3,						0,							0,						0},
	{0x3e,	KEY_F4,						KEY_F4,						0,							0,						0},
	{0x3f,	KEY_F5,						KEY_F5,						0,							0,						0},
	{0x40,	KEY_F6,						KEY_F6,						0,							0,						0},
	{0x41,	KEY_F7,						KEY_F7,						0,							0,						0},
	{0x42,	KEY_F8,						KEY_F8,						0,							0,						0},
	{0x43,	KEY_F9,						KEY_F9,						0,							0,						0},
	{0x44,	KEY_F10,					KEY_F10,					0,							0,						0},
	{0x57,	KEY_F11,					KEY_F11,					0,							0,						0},
	{0x58,	KEY_F12,					KEY_F12,					0,							0,						0},

	{0x1a,	KEY_SQUARE_BRACKET_LEFT,	KEY_SQUARE_BRACKET_LEFT,	KEY_BRACE_LEFT,				0,						0},
	{0x1b,	KEY_SQUARE_BRACKET_RIGHT,	KEY_SQUARE_BRACKET_RIGHT,	KEY_BRACE_RIGHT,			0,						0},
	{0x27,	KEY_SEMICOLON,				KEY_SEMICOLON,				KEY_COLON,					0,						0},
	{0x28,	KEY_SINGLE_QUOTES,			KEY_SINGLE_QUOTES,			KEY_DOUBLE_QUOTES,			0,						0},
	{0x33,	KEY_COMMA,					KEY_COMMA,					KEY_ANGLE_BRACKET_LEFT,		0,						0},
	{0x34,	KEY_PERIOD,					KEY_PERIOD,					KEY_ANGLE_BRACKET_RIGHT,	0,						0},
	{0x35,	KEY_SLASH,					KEY_SLASH,					KEY_QUESTION_MARK,			0,						0},

	{0x48,	KEY_UP,						KEY_UP,						0,							0,						0},
	{0x50,	KEY_DOWN,					KEY_DOWN,					0,							0,						0},
	{0x4b,	KEY_LEFT,					KEY_LEFT,					0,							0,						0},
	{0x4d,	KEY_RIGHT,					KEY_RIGHT,					0,							0,						0},
	{0x47,	KEY_HOME,					KEY_HOME,					0,							0,						0},
	{0x4f,	KEY_END,					KEY_END,					0,							0,						0},
	{0x49,	KEY_PAGEUP,					KEY_PAGEUP,					0,							0,						0},
	{0x51,	KEY_PAGEDOWN,				KEY_PAGEDOWN,				0,							0,						0},
	{0x52,	KEY_INSERT,					KEY_INSERT,					0,							0,						0},
	{0x53,	KEY_DELETE,					KEY_DELETE,					0,							0,						0},

	{0x37,	KEY_HALT,					KEY_HALT,					0,							0,						0},
	{0x3f,	KEY_STANDBY,				KEY_STANDBY,				0,							0,						0},
	{0x5e,	KEY_RESUME,					KEY_RESUME,					0,							0,						0},
};

static void keyboard_report_event(x_u32 flag, x_u8 data, enum key_value press)
{
	x_u32 i;
	enum key_code key;

	for(i = 0; i < ARRAY_SIZE(map); i++)
	{
		if(map[i].data == data)
		{
			if( (flag & KBD_CAPS_LOCK) )
				key = map[i].caps_key;
			else if( (flag & KBD_SHIFT) )
				key = map[i].shift_key;
			else if( (flag & KBD_CTRL) )
				key = map[i].ctrl_key;
			else if( (flag & KBD_ALT) )
				key = map[i].alt_key;
			else
				key = map[i].key;

			if(key != 0)
			{
				input_report(INPUT_KEYBOARD, key, press);
				input_sync(INPUT_KEYBOARD);
			}
		}
	}
}

static void keyboard_interrupt(void)
{
	static x_u32 kbd_flag = KBD_NUM_LOCK;
	x_u8 status, data;

	status = readb(REALVIEW_KEYBOARD_IIR);

	while(status & REALVIEW_KEYBOARD_IIR_RXINTR)
	{
		data = readb(REALVIEW_KEYBOARD_DATA);

		if(data & 0x80)
		{
			/* left shift */
			if(data == (SCANCODE_LEFT_SHIFT | 0x80))
			{
				kbd_flag &= ~KBD_SHIFT;
			}
			/* right shift */
			else if(data == (SCANCODE_RIGHT_SHIFT | 0x80))
			{
				kbd_flag &= ~KBD_SHIFT;
			}
			/* left ctrl */
			else if(data == (SCANCODE_LEFT_CTRL | 0x80))
			{
				kbd_flag &= ~KBD_CTRL;
			}
			/* right ctrl */
			else if(data == (SCANCODE_RIGHT_CTRL | 0x80))
			{
				kbd_flag &= ~KBD_CTRL;
			}
			/* left alt */
			else if(data == (SCANCODE_LEFT_ALT | 0x80))
			{
				kbd_flag &= ~KBD_ALT;
			}
			/* right alt */
			else if(data == (SCANCODE_RIGHT_ALT | 0x80))
			{
				kbd_flag &= ~KBD_ALT;
			}
			/* caps lock */
			else if(data == (SCANCODE_CAPS_LOCK | 0x80))
			{
				if(kbd_flag & KBD_CAPS_LOCK)
					kbd_flag &= ~KBD_CAPS_LOCK;
				else
					kbd_flag |= KBD_CAPS_LOCK;
			}
			/* num lock */
			else if(data == (SCANCODE_NUM_LOCK | 0x80))
			{
				if(kbd_flag & KBD_NUM_LOCK)
					kbd_flag &= ~KBD_NUM_LOCK;
				else
					kbd_flag |= KBD_NUM_LOCK;
			}
			/* scroll lock */
			else if(data == (SCANCODE_SCROLL_LOCK | 0x80))
			{
				if(kbd_flag & KBD_SCROLL_LOCK)
					kbd_flag &= ~KBD_SCROLL_LOCK;
				else
					kbd_flag |= KBD_SCROLL_LOCK;
			}
			/* others */
			else
			{
				keyboard_report_event(kbd_flag, (data & 0x7f), KEY_PRESS_UP);
			}
		}
		else
		{
			/* left shift */
			if(data == SCANCODE_LEFT_SHIFT)
			{
				kbd_flag |= KBD_SHIFT;
			}
			/* right shift */
			else if(data == SCANCODE_RIGHT_SHIFT)
			{
				kbd_flag |= KBD_SHIFT;
			}
			/* left ctrl */
			else if(data == SCANCODE_LEFT_CTRL)
			{
				kbd_flag |= KBD_CTRL;
			}
			/* right ctrl */
			else if(data == SCANCODE_RIGHT_CTRL)
			{
				kbd_flag |= KBD_CTRL;
			}
			/* left alt */
			else if(data == SCANCODE_LEFT_ALT)
			{
				kbd_flag |= KBD_ALT;
			}
			/* right alt */
			else if(data == SCANCODE_RIGHT_ALT)
			{
				kbd_flag |= KBD_ALT;
			}
			/* others */
			else
			{
				keyboard_report_event(kbd_flag, data, KEY_PRESS_DOWN);
			}
		}

		status = readb(REALVIEW_KEYBOARD_IIR);
	}
}

static void keyboard_probe(void)
{
	x_u32 divisor;
	x_u64 kclk;

	if(! clk_get_rate("kclk", &kclk))
	{
		LOG_E("can't get the clock of \'kclk\'");
		return;
	}

	divisor = (x_u32)(div64(kclk, 8000000) - 1);
	writeb(REALVIEW_KEYBOARD_CLKDIV, divisor);

	writeb(REALVIEW_KEYBOARD_CR, REALVIEW_KEYBOARD_CR_EN);

	if(!request_irq("KMI0", keyboard_interrupt))
	{
		LOG_E("can't request irq \'KMI0\'");
		writeb(REALVIEW_KEYBOARD_CR, 0);
		return;
	}

	writeb(REALVIEW_KEYBOARD_CR, REALVIEW_KEYBOARD_CR_EN | REALVIEW_KEYBOARD_CR_RXINTREN);
}

static void keyboard_remove(void)
{
	if(!free_irq("KMI0"))
		LOG_E("can't free irq \'KMI0\'");
	writeb(REALVIEW_KEYBOARD_CR, 0);
}

static struct input realview_keyboard = {
	.name		= "kbd",
	.type		= INPUT_KEYBOARD,
	.probe		= keyboard_probe,
	.remove		= keyboard_remove,
};

static __init void realview_keyboard_init(void)
{
	if(! clk_get_rate("kclk", 0))
	{
		LOG_E("can't get the clock of \'kclk\'");
		return;
	}

	if(!register_input(&realview_keyboard))
		LOG_E("failed to register input '%s'", realview_keyboard.name);
}

static __exit void realview_keyboard_exit(void)
{
	if(!unregister_input(&realview_keyboard))
		LOG_E("failed to unregister input '%s'", realview_keyboard.name);
}

module_init(realview_keyboard_init, LEVEL_DRIVER);
module_exit(realview_keyboard_exit, LEVEL_DRIVER);
