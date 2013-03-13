/*
 * driver/realview_keyboard.c
 *
 * realview keyboard drivers, the primecell pl050 ps2 controller.
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
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

#include <xboot.h>
#include <types.h>
#include <stddef.h>
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

#define	KBD_LEFT_SHIFT		(0x00000001)
#define	KBD_RIGHT_SHIFT		(0x00000002)
#define	KBD_LEFT_CTRL		(0x00000004)
#define	KBD_RIGHT_CTRL		(0x00000008)
#define	KBD_CAPS_LOCK		(0x00000040)
#define	KBD_NUM_LOCK		(0x00000080)
#define	KBD_SCROLL_LOCK		(0x00000100)

enum decode_state {
	DECODE_STATE_MAKE_CODE,
	DECODE_STATE_BREAK_CODE,
	DECODE_STATE_LONG_MAKE_CODE,
	DECODE_STATE_LONG_BREAK_CODE
};

struct keymap {
	u8_t data;
	enum key_code key;
	enum key_code caps_key;
	enum key_code shift_key;
	enum key_code ctrl_key;
};

static const struct keymap map[] = {
	/* code	normal-key					caps-key					shift-key					control-key	*/
	{0x1c,	KEY_a,						KEY_A,						KEY_A,						KEY_CTRL_A},
	{0x32,	KEY_b,						KEY_B,						KEY_B,						KEY_CTRL_B},
	{0x21,	KEY_c,						KEY_C,						KEY_C,						KEY_CTRL_C},
	{0x23,	KEY_d,						KEY_D,						KEY_D,						KEY_CTRL_D},
	{0x24,	KEY_e,						KEY_E,						KEY_E,						KEY_CTRL_E},
	{0x2b,	KEY_f,						KEY_F,						KEY_F,						KEY_CTRL_F},
	{0x34,	KEY_g,						KEY_G,						KEY_G,						KEY_CTRL_G},
	{0x33,	KEY_h,						KEY_H,						KEY_H,						KEY_CTRL_H},
	{0x43,	KEY_i,						KEY_I,						KEY_I,						KEY_CTRL_I},
	{0x3b,	KEY_j,						KEY_J,						KEY_J,						KEY_CTRL_J},
	{0x42,	KEY_k,						KEY_K,						KEY_K,						KEY_CTRL_K},
	{0x4b,	KEY_l,						KEY_L,						KEY_L,						KEY_CTRL_L},
	{0x3a,	KEY_m,						KEY_M,						KEY_M,						KEY_CTRL_M},
	{0x31,	KEY_n,						KEY_N,						KEY_N,						KEY_CTRL_N},
	{0x44,	KEY_o,						KEY_O,						KEY_O,						KEY_CTRL_O},
	{0x4d,	KEY_p,						KEY_P,						KEY_P,						KEY_CTRL_P},
	{0x15,	KEY_q,						KEY_Q,						KEY_Q,						KEY_CTRL_Q},
	{0x2d,	KEY_r,						KEY_R,						KEY_R,						KEY_CTRL_R},
	{0x1b,	KEY_s,						KEY_S,						KEY_S,						KEY_CTRL_S},
	{0x2c,	KEY_t,						KEY_T,						KEY_T,						KEY_CTRL_T},
	{0x3c,	KEY_u,						KEY_U,						KEY_U,						KEY_CTRL_U},
	{0x2a,	KEY_v,						KEY_V,						KEY_V,						KEY_CTRL_V},
	{0x1d,	KEY_w,						KEY_W,						KEY_W,						KEY_CTRL_W},
	{0x22,	KEY_x,						KEY_X,						KEY_X,						KEY_CTRL_X},
	{0x35,	KEY_y,						KEY_Y,						KEY_Y,						KEY_CTRL_Y},
	{0x1a,	KEY_z,						KEY_Z,						KEY_Z,						KEY_CTRL_Z},

	{0x45,	KEY_0,						KEY_0,						KEY_PARENTHESIS_RIGHT,		KEY_0},
	{0x16,	KEY_1,						KEY_1,						KEY_EXCLAMATION_MARK,		KEY_1},
	{0x1e,	KEY_2,						KEY_2,						KEY_AT,						KEY_2},
	{0x26,	KEY_3,						KEY_3,						KEY_POUNDSIGN,				KEY_3},
	{0x25,	KEY_4,						KEY_4,						KEY_DOLLAR,					KEY_4},
	{0x2e,	KEY_5,						KEY_5,						KEY_PERCENT,				KEY_5},
	{0x36,	KEY_6,						KEY_6,						KEY_CIRCUMFLEX_ACCENT,		KEY_6},
	{0x3d,	KEY_7,						KEY_7,						KEY_AMPERSAND,				KEY_7},
	{0x3e,	KEY_8,						KEY_8,						KEY_ASTERISK,				KEY_8},
	{0x46,	KEY_9,						KEY_9,						KEY_PARENTHESIS_LEFT,		KEY_9},

	{0x29,	KEY_SPACE,					KEY_SPACE,					KEY_SPACE,					KEY_SPACE},
	{0x52,	KEY_APOSTROPHE,				KEY_APOSTROPHE,				KEY_QUOTATION_MARK,			KEY_APOSTROPHE},
	{0x55,	KEY_EQUAL,					KEY_EQUAL,					KEY_PLUS,					KEY_EQUAL},
	{0x41,	KEY_COMMA,					KEY_COMMA,					KEY_LESS_THAN,				KEY_COMMA},
	{0x4e,	KEY_MINUS,					KEY_MINUS,					KEY_LOW_LINE,				KEY_MINUS},
	{0x49,	KEY_FULL_STOP,				KEY_FULL_STOP,				KEY_GREATER_THAN,			KEY_FULL_STOP},
	{0x4a,	KEY_SOLIDUS,				KEY_SOLIDUS,				KEY_QUESTION_MARK,			KEY_SOLIDUS},
	{0x4c,	KEY_SEMICOLON,				KEY_SEMICOLON,				KEY_COLON,					KEY_SEMICOLON},
	{0x54,	KEY_SQUARE_BRACKET_LEFT,	KEY_SQUARE_BRACKET_LEFT,	KEY_CURLY_BRACKET_LEFT,		KEY_SQUARE_BRACKET_LEFT},
	{0x5d,	KEY_REVERSE_SOLIDUS,		KEY_REVERSE_SOLIDUS,		KEY_VERTICAL_LINE,			KEY_REVERSE_SOLIDUS},
	{0x5b,	KEY_SQUARE_BRACKET_RIGHT,	KEY_SQUARE_BRACKET_RIGHT,	KEY_CURLY_BRACKET_RIGHT,	KEY_SQUARE_BRACKET_RIGHT},
	{0x0e,	KEY_GRAVE_ACCENT,			KEY_GRAVE_ACCENT,			KEY_TILDE,					KEY_GRAVE_ACCENT},

	{0x75,	KEY_UP,						KEY_UP,						KEY_UP,						KEY_UP},
	{0x72,	KEY_DOWN,					KEY_DOWN,					KEY_DOWN,					KEY_DOWN},
	{0x6b,	KEY_LEFT,					KEY_LEFT,					KEY_LEFT,					KEY_LEFT},
	{0x74,	KEY_RIGHT,					KEY_RIGHT,					KEY_RIGHT,					KEY_RIGHT},
	{0x0d,	KEY_TAB,					KEY_TAB,					KEY_TAB,					KEY_TAB},
	{0x66,	KEY_BACKSPACE,				KEY_BACKSPACE,				KEY_BACKSPACE,				KEY_BACKSPACE},
	{0x5a,	KEY_ENTER,					KEY_ENTER,					KEY_ENTER,					KEY_ENTER},
	{0x6c,	KEY_HOME,					KEY_HOME,					KEY_HOME,					KEY_HOME},
	{0x69,	KEY_MENU,					KEY_MENU,					KEY_MENU,					KEY_MENU},
	{0x76,	KEY_BACK,					KEY_BACK,					KEY_BACK,					KEY_BACK},
	{0x37,	KEY_POWER,					KEY_POWER,					KEY_POWER,					KEY_POWER},
	{0x3f,	KEY_RESET,					KEY_RESET,					KEY_RESET,					KEY_RESET},
};

static void keyboard_report_event(u32_t flag, u8_t data, enum key_value press)
{
	u32_t i;
	enum key_code key;

	for(i = 0; i < ARRAY_SIZE(map); i++)
	{
		if(map[i].data == data)
		{
			if( (flag & KBD_CAPS_LOCK) )
				key = map[i].caps_key;
			else if( (flag & (KBD_LEFT_SHIFT | KBD_RIGHT_SHIFT)) )
				key = map[i].shift_key;
			else if( (flag & (KBD_LEFT_CTRL | KBD_RIGHT_CTRL)) )
				key = map[i].ctrl_key;
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

static bool_t kmi_write(u8_t data)
{
	s32_t timeout = 1000;

	while((readb(REALVIEW_KEYBOARD_STAT) & REALVIEW_KEYBOARD_STAT_TXEMPTY) == 0 && timeout--);

	if(timeout)
	{
		writeb(REALVIEW_KEYBOARD_DATA, data);

		while((readb(REALVIEW_KEYBOARD_STAT) & REALVIEW_KEYBOARD_STAT_RXFULL) == 0);

		if( readb(REALVIEW_KEYBOARD_DATA) == 0xfa)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

static bool_t kmi_read(u8_t * data)
{
	if( (readb(REALVIEW_KEYBOARD_STAT) & REALVIEW_KEYBOARD_STAT_RXFULL) )
	{
		*data = readb(REALVIEW_KEYBOARD_DATA);
		return TRUE;
	}

	return FALSE;
}

static void keyboard_interrupt(void)
{
	static enum decode_state ds = DECODE_STATE_MAKE_CODE;
	static u32_t kbd_flag = KBD_NUM_LOCK;
	u8_t status, data;

	status = readb(REALVIEW_KEYBOARD_IIR);

	while(status & REALVIEW_KEYBOARD_IIR_RXINTR)
	{
		data = readb(REALVIEW_KEYBOARD_DATA);

		switch(ds)
		{
		case DECODE_STATE_MAKE_CODE:
			/* break code */
			if(data == 0xf0)
			{
				ds = DECODE_STATE_BREAK_CODE;
			}
			/* long make code */
			else if(data == 0xe0)
			{
				ds = DECODE_STATE_LONG_MAKE_CODE;
			}
			else
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left shift */
				if(data == 0x12)
				{
					kbd_flag |= KBD_LEFT_SHIFT;
				}
				/* right shift */
				else if(data == 0x59)
				{
					kbd_flag |= KBD_RIGHT_SHIFT;
				}
				/* left ctrl */
				else if(data == 0x14)
				{
					kbd_flag |= KBD_LEFT_CTRL;
				}
				/* caps lock */
				else if(data == 0x58)
				{
					if(kbd_flag & KBD_CAPS_LOCK)
						kbd_flag &= ~KBD_CAPS_LOCK;
					else
						kbd_flag |= KBD_CAPS_LOCK;
				}
				/* scroll lock */
				else if(data == 0x7e)
				{
					if(kbd_flag & KBD_SCROLL_LOCK)
						kbd_flag &= ~KBD_SCROLL_LOCK;
					else
						kbd_flag |= KBD_SCROLL_LOCK;
				}
				/* num lock */
				else if(data == 0x77)
				{
					if(kbd_flag & KBD_NUM_LOCK)
						kbd_flag &= ~KBD_NUM_LOCK;
					else
						kbd_flag |= KBD_NUM_LOCK;
				}
				/* others */
				else
				{
					keyboard_report_event(kbd_flag, data, KEY_BUTTON_DOWN);
				}
			}
			break;

		case DECODE_STATE_BREAK_CODE:
			if( (data != 0xf0) && (data != 0xe0))
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left shift */
				if(data == 0x12)
				{
					kbd_flag &= ~KBD_LEFT_SHIFT;
				}
				/* right shift */
				else if(data == 0x59)
				{
					kbd_flag &= ~KBD_RIGHT_SHIFT;
				}
				/* left ctrl */
				else if(data == 0x14)
				{
					kbd_flag &= ~KBD_LEFT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(kbd_flag, data, KEY_BUTTON_UP);
				}
			}
			else
			{
				ds = DECODE_STATE_BREAK_CODE;
			}
			break;

		case DECODE_STATE_LONG_MAKE_CODE:
			if( data != 0xf0 && data!= 0xe0)
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left ctrl */
				if(data == 0x14)
				{
					kbd_flag |= KBD_RIGHT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(kbd_flag, data, KEY_BUTTON_DOWN);
				}
			}
			else
			{
				ds = DECODE_STATE_LONG_BREAK_CODE;
			}
			break;

		case DECODE_STATE_LONG_BREAK_CODE:
			if( (data != 0xf0) && (data != 0xe0))
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left ctrl */
				if(data == 0x14)
				{
					kbd_flag &= ~KBD_RIGHT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(kbd_flag, data, KEY_BUTTON_UP);
				}
			}
			else
			{
				ds = DECODE_STATE_LONG_BREAK_CODE;
			}
			break;

		default:
			ds = DECODE_STATE_MAKE_CODE;
			break;
		}

		status = readb(REALVIEW_KEYBOARD_IIR);
	}
}

static bool_t keyboard_probe(struct input * input)
{
	u32_t divisor;
	u64_t kclk;
	u8_t data;

	if(! clk_get_rate("kclk", &kclk))
	{
		LOG_E("can't get the clock of 'kclk'");
		return FALSE;
	}

	/* set keyboard's clock divisor */
	divisor = (u32_t)(div64(kclk, 8000000) - 1);
	writeb(REALVIEW_KEYBOARD_CLKDIV, divisor);

	/* enable keyboard controller */
	writeb(REALVIEW_KEYBOARD_CR, REALVIEW_KEYBOARD_CR_EN);

	/* clear a receive buffer */
	kmi_read(&data);

	/* reset keyboard, and wait ack and pass/fail code */
	if(! kmi_write(0xff) )
		return FALSE;
	if(! kmi_read(&data))
		return FALSE;
	if(data != 0xaa)
		return FALSE;

	/* set keyboard's typematic rate/delay */
	kmi_write(0xf3);
	/* 10.9pcs, 500ms */
	kmi_write(0x2b);

	/* scan code set 2 */
	kmi_write(0xf0);
	kmi_write(0x02);

	/* set all keys typematic/make/break */
	kmi_write(0xfa);

	/* set keyboard's number lock, caps lock, and scroll lock */
	kmi_write(0xed);
	kmi_write(0x02);

	if(!request_irq("KMI0", keyboard_interrupt))
	{
		LOG_E("can't request irq 'KMI0'");
		writeb(REALVIEW_KEYBOARD_CR, 0);
		return FALSE;
	}

	/* re-enables keyboard */
	writeb(REALVIEW_KEYBOARD_CR, REALVIEW_KEYBOARD_CR_EN | REALVIEW_KEYBOARD_CR_RXINTREN);

	return TRUE;
}

static bool_t keyboard_remove(struct input * input)
{
	if(!free_irq("KMI0"))
		LOG_E("can't free irq 'KMI0'");
	writeb(REALVIEW_KEYBOARD_CR, 0);

	return TRUE;
}

static int keyboard_ioctl(struct input * input, int cmd, void * arg)
{
	return -1;
}

static struct input realview_keyboard = {
	.name		= "keyboard",
	.type		= INPUT_KEYBOARD,
	.probe		= keyboard_probe,
	.remove		= keyboard_remove,
	.ioctl		= keyboard_ioctl,
	.priv		= NULL,
};

static __init void realview_keyboard_init(void)
{
	if(! clk_get_rate("kclk", 0))
	{
		LOG_E("can't get the clock of 'kclk'");
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

device_initcall(realview_keyboard_init);
device_exitcall(realview_keyboard_exit);
