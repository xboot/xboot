/*
 * driver/realview_keyboard.c
 *
 * realview keyboard drivers, the primecell pl050 ps2 controller.
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <realview-keyboard.h>

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
	u32_t key;
	u32_t caps_key;
	u32_t shift_key;
};

enum key_value_t {
	KEY_BUTTON_UP,
	KEY_BUTTON_DOWN,
};

static const struct keymap map[] = {
	/* code	normal-key					caps-key					shift-key */
	{0x1c,	KEY_a,						KEY_A,						KEY_A},
	{0x32,	KEY_b,						KEY_B,						KEY_B},
	{0x21,	KEY_c,						KEY_C,						KEY_C},
	{0x23,	KEY_d,						KEY_D,						KEY_D},
	{0x24,	KEY_e,						KEY_E,						KEY_E},
	{0x2b,	KEY_f,						KEY_F,						KEY_F},
	{0x34,	KEY_g,						KEY_G,						KEY_G},
	{0x33,	KEY_h,						KEY_H,						KEY_H},
	{0x43,	KEY_i,						KEY_I,						KEY_I},
	{0x3b,	KEY_j,						KEY_J,						KEY_J},
	{0x42,	KEY_k,						KEY_K,						KEY_K},
	{0x4b,	KEY_l,						KEY_L,						KEY_L},
	{0x3a,	KEY_m,						KEY_M,						KEY_M},
	{0x31,	KEY_n,						KEY_N,						KEY_N},
	{0x44,	KEY_o,						KEY_O,						KEY_O},
	{0x4d,	KEY_p,						KEY_P,						KEY_P},
	{0x15,	KEY_q,						KEY_Q,						KEY_Q},
	{0x2d,	KEY_r,						KEY_R,						KEY_R},
	{0x1b,	KEY_s,						KEY_S,						KEY_S},
	{0x2c,	KEY_t,						KEY_T,						KEY_T},
	{0x3c,	KEY_u,						KEY_U,						KEY_U},
	{0x2a,	KEY_v,						KEY_V,						KEY_V},
	{0x1d,	KEY_w,						KEY_W,						KEY_W},
	{0x22,	KEY_x,						KEY_X,						KEY_X},
	{0x35,	KEY_y,						KEY_Y,						KEY_Y},
	{0x1a,	KEY_z,						KEY_Z,						KEY_Z},

	{0x45,	KEY_0,						KEY_0,						KEY_PARENTHESIS_RIGHT},
	{0x16,	KEY_1,						KEY_1,						KEY_EXCLAMATION_MARK},
	{0x1e,	KEY_2,						KEY_2,						KEY_AT},
	{0x26,	KEY_3,						KEY_3,						KEY_POUNDSIGN},
	{0x25,	KEY_4,						KEY_4,						KEY_DOLLAR},
	{0x2e,	KEY_5,						KEY_5,						KEY_PERCENT},
	{0x36,	KEY_6,						KEY_6,						KEY_CIRCUMFLEX_ACCENT},
	{0x3d,	KEY_7,						KEY_7,						KEY_AMPERSAND},
	{0x3e,	KEY_8,						KEY_8,						KEY_ASTERISK},
	{0x46,	KEY_9,						KEY_9,						KEY_PARENTHESIS_LEFT},

	{0x29,	KEY_SPACE,					KEY_SPACE,					KEY_SPACE},
	{0x52,	KEY_APOSTROPHE,				KEY_APOSTROPHE,				KEY_QUOTATION_MARK},
	{0x55,	KEY_EQUAL,					KEY_EQUAL,					KEY_PLUS},
	{0x41,	KEY_COMMA,					KEY_COMMA,					KEY_LESS_THAN},
	{0x4e,	KEY_MINUS,					KEY_MINUS,					KEY_LOW_LINE},
	{0x49,	KEY_FULL_STOP,				KEY_FULL_STOP,				KEY_GREATER_THAN},
	{0x4a,	KEY_SOLIDUS,				KEY_SOLIDUS,				KEY_QUESTION_MARK},
	{0x4c,	KEY_SEMICOLON,				KEY_SEMICOLON,				KEY_COLON},
	{0x54,	KEY_SQUARE_BRACKET_LEFT,	KEY_SQUARE_BRACKET_LEFT,	KEY_CURLY_BRACKET_LEFT},
	{0x5d,	KEY_REVERSE_SOLIDUS,		KEY_REVERSE_SOLIDUS,		KEY_VERTICAL_LINE},
	{0x5b,	KEY_SQUARE_BRACKET_RIGHT,	KEY_SQUARE_BRACKET_RIGHT,	KEY_CURLY_BRACKET_RIGHT},
	{0x0e,	KEY_GRAVE_ACCENT,			KEY_GRAVE_ACCENT,			KEY_TILDE},

	{0x75,	KEY_UP,						KEY_UP,						KEY_UP},
	{0x72,	KEY_DOWN,					KEY_DOWN,					KEY_DOWN},
	{0x6b,	KEY_LEFT,					KEY_LEFT,					KEY_LEFT},
	{0x74,	KEY_RIGHT,					KEY_RIGHT,					KEY_RIGHT},
	{0x0d,	KEY_TAB,					KEY_TAB,					KEY_TAB},
	{0x66,	KEY_BACKSPACE,				KEY_BACKSPACE,				KEY_BACKSPACE},
	{0x5a,	KEY_ENTER,					KEY_ENTER,					KEY_ENTER},
	{0x6c,	KEY_HOME,					KEY_HOME,					KEY_HOME},
	{0x69,	KEY_MENU,					KEY_MENU,					KEY_MENU},
	{0x76,	KEY_BACK,					KEY_BACK,					KEY_BACK},
	{0x37,	KEY_POWER,					KEY_POWER,					KEY_POWER},
};

static void keyboard_report_event(void * device, u32_t flag, u8_t data, enum key_value_t press)
{
	struct event_t event;
	u32_t key;
	u32_t i;

	for(i = 0; i < ARRAY_SIZE(map); i++)
	{
		if(map[i].data == data)
		{
			if( (flag & KBD_CAPS_LOCK) )
				key = map[i].caps_key;
			else if( (flag & (KBD_LEFT_SHIFT | KBD_RIGHT_SHIFT)) )
				key = map[i].shift_key;
			else if( (flag & (KBD_LEFT_CTRL | KBD_RIGHT_CTRL)) )
				key = 0;
			else
				key = map[i].key;

			if(key != 0)
			{
				if(press == KEY_BUTTON_DOWN)
					push_event_key_down(device, key);
				else if(press == KEY_BUTTON_UP)
					push_event_key_up(device, key);
			}
		}
	}
}

static bool_t kmi_write(struct realview_keyboard_data_t * dat, u8_t data)
{
	s32_t timeout = 1000;

	while((readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_STAT) & REALVIEW_KEYBOARD_STAT_TXEMPTY) == 0 && timeout--);

	if(timeout)
	{
		writeb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_DATA, data);

		while((readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_STAT) & REALVIEW_KEYBOARD_STAT_RXFULL) == 0);

		if( readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_DATA) == 0xfa)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

static bool_t kmi_read(struct realview_keyboard_data_t * dat, u8_t * data)
{
	if( (readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_STAT) & REALVIEW_KEYBOARD_STAT_RXFULL) )
	{
		*data = readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_DATA);
		return TRUE;
	}

	return FALSE;
}

static void keyboard_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct resource_t * res = (struct resource_t *)input->priv;
	struct realview_keyboard_data_t * dat = (struct realview_keyboard_data_t *)res->data;

	static enum decode_state ds = DECODE_STATE_MAKE_CODE;
	static u32_t kbd_flag = KBD_NUM_LOCK;
	u8_t status, value;

	status = readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_IIR);

	while(status & REALVIEW_KEYBOARD_IIR_RXINTR)
	{
		value = readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_DATA);

		switch(ds)
		{
		case DECODE_STATE_MAKE_CODE:
			/* break code */
			if(value == 0xf0)
			{
				ds = DECODE_STATE_BREAK_CODE;
			}
			/* long make code */
			else if(value == 0xe0)
			{
				ds = DECODE_STATE_LONG_MAKE_CODE;
			}
			else
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left shift */
				if(value == 0x12)
				{
					kbd_flag |= KBD_LEFT_SHIFT;
				}
				/* right shift */
				else if(value == 0x59)
				{
					kbd_flag |= KBD_RIGHT_SHIFT;
				}
				/* left ctrl */
				else if(value == 0x14)
				{
					kbd_flag |= KBD_LEFT_CTRL;
				}
				/* caps lock */
				else if(value == 0x58)
				{
					if(kbd_flag & KBD_CAPS_LOCK)
						kbd_flag &= ~KBD_CAPS_LOCK;
					else
						kbd_flag |= KBD_CAPS_LOCK;
				}
				/* scroll lock */
				else if(value == 0x7e)
				{
					if(kbd_flag & KBD_SCROLL_LOCK)
						kbd_flag &= ~KBD_SCROLL_LOCK;
					else
						kbd_flag |= KBD_SCROLL_LOCK;
				}
				/* num lock */
				else if(value == 0x77)
				{
					if(kbd_flag & KBD_NUM_LOCK)
						kbd_flag &= ~KBD_NUM_LOCK;
					else
						kbd_flag |= KBD_NUM_LOCK;
				}
				/* others */
				else
				{
					keyboard_report_event(input, kbd_flag, value, KEY_BUTTON_DOWN);
				}
			}
			break;

		case DECODE_STATE_BREAK_CODE:
			if( (value != 0xf0) && (value != 0xe0))
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left shift */
				if(value == 0x12)
				{
					kbd_flag &= ~KBD_LEFT_SHIFT;
				}
				/* right shift */
				else if(value == 0x59)
				{
					kbd_flag &= ~KBD_RIGHT_SHIFT;
				}
				/* left ctrl */
				else if(value == 0x14)
				{
					kbd_flag &= ~KBD_LEFT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(input, kbd_flag, value, KEY_BUTTON_UP);
				}
			}
			else
			{
				ds = DECODE_STATE_BREAK_CODE;
			}
			break;

		case DECODE_STATE_LONG_MAKE_CODE:
			if( value != 0xf0 && value!= 0xe0)
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left ctrl */
				if(value == 0x14)
				{
					kbd_flag |= KBD_RIGHT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(input, kbd_flag, value, KEY_BUTTON_DOWN);
				}
			}
			else
			{
				ds = DECODE_STATE_LONG_BREAK_CODE;
			}
			break;

		case DECODE_STATE_LONG_BREAK_CODE:
			if( (value != 0xf0) && (value != 0xe0))
			{
				ds = DECODE_STATE_MAKE_CODE;

				/* left ctrl */
				if(value == 0x14)
				{
					kbd_flag &= ~KBD_RIGHT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(input, kbd_flag, value, KEY_BUTTON_UP);
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

		status = readb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_IIR);
	}
}

static void input_init(struct input_t * input)
{
	struct resource_t * res = (struct resource_t *)input->priv;
	struct realview_keyboard_data_t * dat = (struct realview_keyboard_data_t *)res->data;
	u32_t divisor;
	u64_t kclk;
	u8_t value;

	clk_enable("kclk");
	kclk = clk_get_rate("kclk");
	if(!kclk)
		return;

	/* Set keyboard's clock divisor */
	divisor = (u32_t)(kclk / 8000000) - 1;
	writeb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_CLKDIV, divisor);

	/* Enable keyboard controller */
	writeb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_CR, REALVIEW_KEYBOARD_CR_EN);

	/* Clear a receive buffer */
	kmi_read(dat, &value);

	/* Reset keyboard, and wait ack and pass/fail code */
	if(! kmi_write(dat, 0xff) )
		return;
	if(! kmi_read(dat, &value))
		return;
	if(value != 0xaa)
		return;

	/* Set keyboard's typematic rate/delay */
	kmi_write(dat, 0xf3);
	/* 10.9pcs, 500ms */
	kmi_write(dat, 0x2b);

	/* Scan code set 2 */
	kmi_write(dat, 0xf0);
	kmi_write(dat, 0x02);

	/* Set all keys typematic/make/break */
	kmi_write(dat, 0xfa);

	/* Set keyboard's number lock, caps lock, and scroll lock */
	kmi_write(dat, 0xed);
	kmi_write(dat, 0x02);

	if(!request_irq("KMI0", keyboard_interrupt, input))
	{
		LOG("Can't request irq 'KMI0'");
		writeb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_CR, 0);
		return;
	}

	/* Re-enables keyboard */
	writeb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_CR, REALVIEW_KEYBOARD_CR_EN | REALVIEW_KEYBOARD_CR_RXINTREN);
}

static void input_exit(struct input_t * input)
{
	struct resource_t * res = (struct resource_t *)input->priv;
	struct realview_keyboard_data_t * dat = (struct realview_keyboard_data_t *)res->data;

	clk_disable("kclk");
	if(!free_irq("KMI0"))
		LOG("Can't free irq 'KMI0'");
	writeb(dat->regbase + REALVIEW_KEYBOARD_OFFSET_CR, 0);
}

static int input_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static void input_suspend(struct input_t * input)
{
}

static void input_resume(struct input_t * input)
{
}

static bool_t realview_register_keyboard(struct resource_t * res)
{
	struct input_t * input;
	char name[64];

	input = malloc(sizeof(struct input_t));
	if(!input)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	input->name = strdup(name);
	input->type = INPUT_TYPE_KEYBOARD;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume	= input_resume,
	input->priv = res;

	if(register_input(input))
		return TRUE;

	free(input->name);
	free(input);
	return FALSE;
}

static bool_t realview_unregister_keyboard(struct resource_t * res)
{
	struct input_t * input;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	input = search_input(name);
	if(!input)
		return FALSE;

	if(!unregister_input(input))
		return FALSE;

	free(input->name);
	free(input);
	return TRUE;
}

static __init void realview_keyboard_device_init(void)
{
	resource_for_each_with_name("realview-keyboard", realview_register_keyboard);
}

static __exit void realview_keyboard_device_exit(void)
{
	resource_for_each_with_name("realview-keyboard", realview_unregister_keyboard);
}

device_initcall(realview_keyboard_device_init);
device_exitcall(realview_keyboard_device_exit);
