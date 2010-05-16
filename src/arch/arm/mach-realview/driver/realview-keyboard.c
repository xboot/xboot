/*
 * driver/realview_keyboard.c
 *
 * realview keyboard drivers, the primecell pl050 ps2 controller.
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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
#include <time/delay.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <input/keyboard/keyboard.h>
#include <realview/reg-keyboard.h>


#define	KBD_LEFT_SHIFT		(0x00000001)
#define	KBD_RIGHT_SHIFT		(0x00000002)
#define	KBD_LEFT_CTRL		(0x00000004)
#define	KBD_RIGHT_CTRL		(0x00000008)
#define	KBD_LEFT_ALT		(0x00000010)
#define	KBD_RIGHT_ALT		(0x00000020)
#define	KBD_CAPS_LOCK		(0x00000040)
#define	KBD_NUM_LOCK		(0x00000080)
#define	KBD_SCROLL_LOCK		(0x00000100)


struct keymap {
	x_u8 data;
	enum keycode key;
	enum keycode caps_key;
	enum keycode shift_key;
	enum keycode ctrl_key;
	enum keycode alt_key;
};

enum decode_state {
	DECODE_STATE_MAKE_CODE,
	DECODE_STATE_BREAK_CODE,
	DECODE_STATE_LONG_MAKE_CODE,
	DECODE_STATE_LONG_BREAK_CODE
};

static const struct keymap map[] = {
/*	code	normal-key					caps-key					shift-key					control-key				alt-key	*/
	{0x1c,	KEY_a,						KEY_A,						KEY_A,						KEY_CTRL_A,				KEY_ALT_A},
	{0x32,	KEY_b,						KEY_B,						KEY_B,						KEY_CTRL_B,				KEY_ALT_B},
	{0x21,	KEY_c,						KEY_C,						KEY_C,						KEY_CTRL_C,				KEY_ALT_C},
	{0x23,	KEY_d,						KEY_D,						KEY_D,						KEY_CTRL_D,				KEY_ALT_D},
	{0x24,	KEY_e,						KEY_E,						KEY_E,						KEY_CTRL_E,				KEY_ALT_E},
	{0x2b,	KEY_f,						KEY_F,						KEY_F,						KEY_CTRL_F,				KEY_ALT_F},
	{0x34,	KEY_g,						KEY_G,						KEY_G,						KEY_CTRL_G,				KEY_ALT_G},
	{0x33,	KEY_h,						KEY_H,						KEY_H,						KEY_CTRL_H,				KEY_ALT_H},
	{0x43,	KEY_i,						KEY_I,						KEY_I,						KEY_CTRL_I,				KEY_ALT_I},
	{0x3b,	KEY_j,						KEY_J,						KEY_J,						KEY_CTRL_J,				KEY_ALT_J},
	{0x42,	KEY_k,						KEY_K,						KEY_K,						KEY_CTRL_K,				KEY_ALT_K},
	{0x4b,	KEY_l,						KEY_L,						KEY_L,						KEY_CTRL_L,				KEY_ALT_L},
	{0x3a,	KEY_m,						KEY_M,						KEY_M,						KEY_CTRL_M,				KEY_ALT_M},
	{0x31,	KEY_n,						KEY_N,						KEY_N,						KEY_CTRL_N,				KEY_ALT_N},
	{0x44,	KEY_o,						KEY_O,						KEY_O,						KEY_CTRL_O,				KEY_ALT_O},
	{0x4d,	KEY_p,						KEY_P,						KEY_P,						KEY_CTRL_P,				KEY_ALT_P},
	{0x15,	KEY_q,						KEY_Q,						KEY_Q,						KEY_CTRL_Q,				KEY_ALT_Q},
	{0x2d,	KEY_r,						KEY_R,						KEY_R,						KEY_CTRL_R,				KEY_ALT_R},
	{0x1b,	KEY_s,						KEY_S,						KEY_S,						KEY_CTRL_S,				KEY_ALT_S},
	{0x2c,	KEY_t,						KEY_T,						KEY_T,						KEY_CTRL_T,				KEY_ALT_T},
	{0x3c,	KEY_u,						KEY_U,						KEY_U,						KEY_CTRL_U,				KEY_ALT_U},
	{0x2a,	KEY_v,						KEY_V,						KEY_V,						KEY_CTRL_V,				KEY_ALT_V},
	{0x1d,	KEY_w,						KEY_W,						KEY_W,						KEY_CTRL_W,				KEY_ALT_W},
	{0x22,	KEY_x,						KEY_X,						KEY_X,						KEY_CTRL_X,				KEY_ALT_X},
	{0x35,	KEY_y,						KEY_Y,						KEY_Y,						KEY_CTRL_Y,				KEY_ALT_Y},
	{0x1a,	KEY_z,						KEY_Z,						KEY_Z,						KEY_CTRL_Z,				KEY_ALT_Z},

	{0x45,	KEY_0,						KEY_0,						KEY_ROUND_BRACKET_RIGHT,	KEY_CTRL_0,				KEY_ALT_0},
	{0x16,	KEY_1,						KEY_1,						KEY_EXCLAMATION_MARK,		KEY_CTRL_1,				KEY_ALT_1},
	{0x1e,	KEY_2,						KEY_2,						KEY_AT,						KEY_CTRL_2,				KEY_ALT_2},
	{0x26,	KEY_3,						KEY_3,						KEY_POUNDSIGN,				KEY_CTRL_3,				KEY_ALT_3},
	{0x25,	KEY_4,						KEY_4,						KEY_DOLLAR,					KEY_CTRL_4,				KEY_ALT_4},
	{0x2e,	KEY_5,						KEY_5,						KEY_PERCENT,				KEY_CTRL_5,				KEY_ALT_5},
	{0x36,	KEY_6,						KEY_6,						KEY_POWER,					KEY_CTRL_6,				KEY_ALT_6},
	{0x3d,	KEY_7,						KEY_7,						KEY_AND,					KEY_CTRL_7,				KEY_ALT_7},
	{0x3e,	KEY_8,						KEY_8,						KEY_STAR,					KEY_CTRL_8,				KEY_ALT_8},
	{0x46,	KEY_9,						KEY_9,						KEY_ROUND_BRACKET_LEFT,		KEY_CTRL_9,				KEY_ALT_9},

	{0x0e,	KEY_UNQUOTE,				KEY_UNQUOTE,				KEY_WAVE,					KEY_NULL,				KEY_NULL},
	{0x4e,	KEY_MINUS,					KEY_MINUS,					KEY_UNDERLINE,				KEY_NULL,				KEY_NULL},
	{0x55,	KEY_EQUAL,					KEY_EQUAL,					KEY_PLUS,					KEY_NULL,				KEY_NULL},
	{0x5d,	KEY_BACKLASH,				KEY_BACKLASH,				KEY_OR,						KEY_NULL,				KEY_NULL},
	{0x66,	KEY_BACKSPACE,				KEY_BACKSPACE,				KEY_BACKSPACE,				KEY_NULL,				KEY_NULL},
	{0x29,	KEY_SPACE,					KEY_SPACE,					KEY_SPACE,					KEY_NULL,				KEY_NULL},
	{0x0d,	KEY_TAB,					KEY_TAB,					KEY_TAB,					KEY_NULL,				KEY_NULL},
	{0x5a,	KEY_ENTER,					KEY_ENTER,					KEY_ENTER,					KEY_NULL,				KEY_NULL},
	{0x76,	KEY_ESC,					KEY_ESC,					KEY_ESC,					KEY_NULL,				KEY_NULL},

	{0x05,	KEY_F1,						KEY_F1,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x06,	KEY_F2,						KEY_F2,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x04,	KEY_F3,						KEY_F3,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x0c,	KEY_F4,						KEY_F4,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x03,	KEY_F5,						KEY_F5,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x0b,	KEY_F6,						KEY_F6,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x83,	KEY_F7,						KEY_F7,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x0a,	KEY_F8,						KEY_F8,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x01,	KEY_F9,						KEY_F9,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x09,	KEY_F10,					KEY_F10,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x78,	KEY_F11,					KEY_F11,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x07,	KEY_F12,					KEY_F12,					KEY_NULL,					KEY_NULL,				KEY_NULL},

	{0x54,	KEY_SQUARE_BRACKET_LEFT,	KEY_SQUARE_BRACKET_LEFT,	KEY_BRACE_LEFT,				KEY_NULL,				KEY_NULL},
	{0x5b,	KEY_SQUARE_BRACKET_RIGHT,	KEY_SQUARE_BRACKET_RIGHT,	KEY_BRACE_RIGHT,			KEY_NULL,				KEY_NULL},
	{0x4c,	KEY_SEMICOLON,				KEY_SEMICOLON,				KEY_COLON,					KEY_NULL,				KEY_NULL},
	{0x52,	KEY_SINGLE_QUOTES,			KEY_SINGLE_QUOTES,			KEY_DOUBLE_QUOTES,			KEY_NULL,				KEY_NULL},
	{0x41,	KEY_COMMA,					KEY_COMMA,					KEY_ANGLE_BRACKET_LEFT,		KEY_NULL,				KEY_NULL},
	{0x49,	KEY_PERIOD,					KEY_PERIOD,					KEY_ANGLE_BRACKET_RIGHT,	KEY_NULL,				KEY_NULL},
	{0x4a,	KEY_SLASH,					KEY_SLASH,					KEY_QUESTION_MARK,			KEY_NULL,				KEY_NULL},

	{0x75,	KEY_UP,						KEY_UP,						KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x72,	KEY_DOWN,					KEY_DOWN,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x6b,	KEY_LEFT,					KEY_LEFT,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x74,	KEY_RIGHT,					KEY_RIGHT,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x6c,	KEY_HOME,					KEY_HOME,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x69,	KEY_END,					KEY_END,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x7d,	KEY_PAGEUP,					KEY_PAGEUP,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x7a,	KEY_PAGEDOWN,				KEY_PAGEDOWN,				KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x70,	KEY_INSERT,					KEY_INSERT,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x71,	KEY_DELETE,					KEY_DELETE,					KEY_NULL,					KEY_NULL,				KEY_NULL},

	{0x37,	KEY_HALT,					KEY_HALT,					KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x3f,	KEY_STANDBY,				KEY_STANDBY,				KEY_NULL,					KEY_NULL,				KEY_NULL},
	{0x5e,	KEY_RESUME,					KEY_RESUME,					KEY_NULL,					KEY_NULL,				KEY_NULL},
};

static x_bool get_keycode(x_u32 flag, x_u8 data, enum keycode * code)
{
	x_u32 i;
	enum keycode key;

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
			else if( (flag & (KBD_LEFT_ALT | KBD_RIGHT_ALT)) )
				key = map[i].alt_key;
			else
				key = map[i].key;

			if(key != KEY_NULL)
			{
				*code = key;
				return TRUE;
			}
			else
				return FALSE;
		}
	}

	return FALSE;
}

static x_bool key_translate(x_u8 data, enum keycode * code)
{
	static enum decode_state ds = DECODE_STATE_MAKE_CODE;
	static x_u32 kbd_flag = KBD_NUM_LOCK;

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
			/* left alt */
			else if(data == 0x11)
			{
				kbd_flag |= KBD_LEFT_ALT;
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
				return get_keycode(kbd_flag, data, code);
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
			/* left alt */
			else if(data == 0x11)
			{
				kbd_flag &= ~KBD_LEFT_ALT;
			}
			/* others */
			else
			{

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

			/* right ctrl */
			if(data == 0x14)
			{
				kbd_flag |= KBD_RIGHT_CTRL;
			}
			/* right alt */
			else if(data == 0x11)
			{
				kbd_flag |= KBD_RIGHT_ALT;
			}
			/* others */
			else
			{
				return get_keycode(kbd_flag, data, code);
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

			/* right ctrl */
			if(data == 0x14)
			{
				kbd_flag &= ~KBD_RIGHT_CTRL;
			}
			/* right alt */
			else if(data == 0x11)
			{
				kbd_flag &= ~KBD_RIGHT_ALT;
			}
			/* others */
			else
			{

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

	return FALSE;
}

static x_bool kmi_write(x_u8 data)
{
	x_s32 timeout = 1000;

	/* wait the tx buffer is empty */
	while((readb(REALVIEW_KEYBOARD_STAT) & REALVIEW_KEYBOARD_STAT_TXEMPTY) == 0 && timeout--);

	if(timeout)
	{
		/* write a command byte */
		writeb(REALVIEW_KEYBOARD_DATA, data);

		/* wait for receiving */
		while((readb(REALVIEW_KEYBOARD_STAT) & REALVIEW_KEYBOARD_STAT_RXFULL) == 0);

		/* make sure ack signal */
		if( readb(REALVIEW_KEYBOARD_DATA) == 0xfa)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

static x_bool kmi_read(x_u8 * data)
{
	if( (readb(REALVIEW_KEYBOARD_STAT) & REALVIEW_KEYBOARD_STAT_RXFULL) )
	{
		*data = readb(REALVIEW_KEYBOARD_DATA);
		return TRUE;
	}

	return FALSE;
}

static void keyboard_init(void)
{
	x_u8 data;
	x_u32 divisor;
	x_u64 kclk;

	/* get keyboard's clock */
	if(! clk_get_rate("kclk", &kclk))
	{
		LOG_E("can't get the clock of \'kclk\'");
		return;
	}
	/* set keyboard's clock divisor */
	divisor = (x_u32)(div64(kclk, 8000000) - 1);
	writeb(REALVIEW_KEYBOARD_CLKDIV, divisor);

	/* enable keyboard controller */
	writeb(REALVIEW_KEYBOARD_CR, REALVIEW_KEYBOARD_CR_EN);

	/* clear a receive buffer */
	kmi_read(&data);

	/* reset keyboard, and wait ack and pass/fail code */
	if(! kmi_write(0xff) )
		return;
	if(! kmi_read(&data))
		return;
	if(data != 0xaa)
		return;

	/* set keyboard's typematic rate/delay */
	kmi_write(0xf3);
	/* 10.9pcs, 500ms */
	kmi_write(0x2b);

	/* scan code set 2 */
	kmi_write(0xf0);
	kmi_write(0x02);

	/* set all keys typematic/make/break */
	kmi_write(0xfa);

	/* set keyboard's num lock, caps lock, and scroll lock */
	kmi_write(0xed);
	kmi_write(0x02);

	/* re-enables keyboard */
	kmi_write(0xf4);
}

static void keyboard_exit(void)
{
	writeb(REALVIEW_KEYBOARD_CR, 0);
}

static x_bool keyboard_read(enum keycode * code)
{
	x_u8 stat = readb(REALVIEW_KEYBOARD_STAT);
	x_u8 data;

	if(stat & REALVIEW_KEYBOARD_STAT_RXFULL)
	{
		data = readb(REALVIEW_KEYBOARD_DATA);
		return key_translate(data, code);
	}

	return FALSE;
}

static x_s32 keyboard_ioctl(x_u32 cmd, void * arg)
{
	return -1;
}

static struct keyboard_driver realview_keyboard = {
	.name		= "kbd",
	.init		= keyboard_init,
	.exit		= keyboard_exit,
	.read		= keyboard_read,
	.ioctl		= keyboard_ioctl,
};

static __init void realview_keyboard_init(void)
{
	if(! clk_get_rate("kclk", 0))
	{
		LOG_E("can't get the clock of \'kclk\'");
		return;
	}

	if(!register_keyboard(&realview_keyboard))
		LOG_E("failed to register keyboard driver '%s'", realview_keyboard.name);
}

static __exit void realview_keyboard_exit(void)
{
	if(!unregister_keyboard(&realview_keyboard))
		LOG_E("failed to unregister keyboard driver '%s'", realview_keyboard.name);
}

module_init(realview_keyboard_init, LEVEL_DRIVER);
module_exit(realview_keyboard_exit, LEVEL_DRIVER);
