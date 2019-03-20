/*
 * driver/keyboard-pl050.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <clk/clk.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <input/keyboard.h>

enum{
	KEYBOARD_CR		= 0x00,
	KEYBOARD_STAT	= 0x04,
	KEYBOARD_DATA	= 0x08,
	KEYBOARD_CLKDIV	= 0x0c,
	KEYBOARD_IIR	= 0x10,
};

enum decode_state {
	DECODE_STATE_MAKE_CODE,
	DECODE_STATE_BREAK_CODE,
	DECODE_STATE_LONG_MAKE_CODE,
	DECODE_STATE_LONG_BREAK_CODE
};

struct keyboard_pl050_pdata_t
{
	virtual_addr_t virt;
	char * clk;
	int irq;
	enum decode_state ds;
	u32_t flag;
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

enum {
	KBD_LEFT_SHIFT 	= (0x1 << 0),
	KBD_RIGHT_SHIFT = (0x1 << 1),
	KBD_LEFT_CTRL 	= (0x1 << 2),
	KBD_RIGHT_CTRL	= (0x1 << 3),
	KBD_CAPS_LOCK 	= (0x1 << 6),
	KBD_NUM_LOCK 	= (0x1 << 7),
	KBD_SCROLL_LOCK	= (0x1 << 8),
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
	{0x66,	KEY_DELETE,					KEY_DELETE,					KEY_DELETE},
	{0x5a,	KEY_ENTER,					KEY_ENTER,					KEY_ENTER},
	{0x6c,	KEY_VOLUME_DOWN,			KEY_VOLUME_DOWN,			KEY_VOLUME_DOWN},
	{0x69,	KEY_VOLUME_UP,				KEY_VOLUME_UP,				KEY_VOLUME_UP},
	{0x76,	KEY_EXIT,					KEY_EXIT,					KEY_EXIT},
	{0x37,	KEY_POWER,					KEY_POWER,					KEY_POWER},
};

static void keyboard_report_event(void * device, u32_t flag, u8_t data, enum key_value_t press)
{
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

static bool_t kmi_write(struct keyboard_pl050_pdata_t * pdat, u8_t value)
{
	int timeout = 1000;

	while((read8(pdat->virt + KEYBOARD_STAT) & (1 << 6)) == 0 && timeout--);

	if(timeout)
	{
		write8(pdat->virt + KEYBOARD_DATA, value);
		while((read8(pdat->virt + KEYBOARD_STAT) & (1 << 4)) == 0);

		if(read8(pdat->virt + KEYBOARD_DATA) == 0xfa)
			return TRUE;
	}
	return FALSE;
}

static bool_t kmi_read(struct keyboard_pl050_pdata_t * pdat, u8_t * value)
{
	if((read8(pdat->virt + KEYBOARD_STAT) & (1 << 4)))
	{
		*value = read8(pdat->virt + KEYBOARD_DATA);
		return TRUE;
	}
	return FALSE;
}

static void keyboard_pl050_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct keyboard_pl050_pdata_t * pdat = (struct keyboard_pl050_pdata_t *)input->priv;
	u8_t status = read8(pdat->virt + KEYBOARD_IIR);
	u8_t value;

	while(status & (1 << 0))
	{
		value = read8(pdat->virt + KEYBOARD_DATA);

		switch(pdat->ds)
		{
		case DECODE_STATE_MAKE_CODE:
			/* break code */
			if(value == 0xf0)
			{
				pdat->ds = DECODE_STATE_BREAK_CODE;
			}
			/* long make code */
			else if(value == 0xe0)
			{
				pdat->ds = DECODE_STATE_LONG_MAKE_CODE;
			}
			else
			{
				pdat->ds = DECODE_STATE_MAKE_CODE;

				/* left shift */
				if(value == 0x12)
				{
					pdat->flag |= KBD_LEFT_SHIFT;
				}
				/* right shift */
				else if(value == 0x59)
				{
					pdat->flag |= KBD_RIGHT_SHIFT;
				}
				/* left ctrl */
				else if(value == 0x14)
				{
					pdat->flag |= KBD_LEFT_CTRL;
				}
				/* caps lock */
				else if(value == 0x58)
				{
					if(pdat->flag & KBD_CAPS_LOCK)
						pdat->flag &= ~KBD_CAPS_LOCK;
					else
						pdat->flag |= KBD_CAPS_LOCK;
				}
				/* scroll lock */
				else if(value == 0x7e)
				{
					if(pdat->flag & KBD_SCROLL_LOCK)
						pdat->flag &= ~KBD_SCROLL_LOCK;
					else
						pdat->flag |= KBD_SCROLL_LOCK;
				}
				/* num lock */
				else if(value == 0x77)
				{
					if(pdat->flag & KBD_NUM_LOCK)
						pdat->flag &= ~KBD_NUM_LOCK;
					else
						pdat->flag |= KBD_NUM_LOCK;
				}
				/* others */
				else
				{
					keyboard_report_event(input, pdat->flag, value, KEY_BUTTON_DOWN);
				}
			}
			break;

		case DECODE_STATE_BREAK_CODE:
			if( (value != 0xf0) && (value != 0xe0))
			{
				pdat->ds = DECODE_STATE_MAKE_CODE;

				/* left shift */
				if(value == 0x12)
				{
					pdat->flag &= ~KBD_LEFT_SHIFT;
				}
				/* right shift */
				else if(value == 0x59)
				{
					pdat->flag &= ~KBD_RIGHT_SHIFT;
				}
				/* left ctrl */
				else if(value == 0x14)
				{
					pdat->flag &= ~KBD_LEFT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(input, pdat->flag, value, KEY_BUTTON_UP);
				}
			}
			else
			{
				pdat->ds = DECODE_STATE_BREAK_CODE;
			}
			break;

		case DECODE_STATE_LONG_MAKE_CODE:
			if( value != 0xf0 && value!= 0xe0)
			{
				pdat->ds = DECODE_STATE_MAKE_CODE;

				/* left ctrl */
				if(value == 0x14)
				{
					pdat->flag |= KBD_RIGHT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(input, pdat->flag, value, KEY_BUTTON_DOWN);
				}
			}
			else
			{
				pdat->ds = DECODE_STATE_LONG_BREAK_CODE;
			}
			break;

		case DECODE_STATE_LONG_BREAK_CODE:
			if( (value != 0xf0) && (value != 0xe0))
			{
				pdat->ds = DECODE_STATE_MAKE_CODE;

				/* left ctrl */
				if(value == 0x14)
				{
					pdat->flag &= ~KBD_RIGHT_CTRL;
				}
				/* others */
				else
				{
					keyboard_report_event(input, pdat->flag, value, KEY_BUTTON_UP);
				}
			}
			else
			{
				pdat->ds = DECODE_STATE_LONG_BREAK_CODE;
			}
			break;

		default:
			pdat->ds = DECODE_STATE_MAKE_CODE;
			break;
		}

		status = read8(pdat->virt + KEYBOARD_IIR);
	}
}

static int keyboard_pl050_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * keyboard_pl050_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct keyboard_pl050_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	virtual_addr_t virt = (dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));
	u64_t rate;
	u8_t value;

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x050)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	pdat = malloc(sizeof(struct keyboard_pl050_pdata_t));
	if(!pdat)
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->irq = irq;
	pdat->ds = DECODE_STATE_MAKE_CODE;
	pdat->flag = KBD_NUM_LOCK;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = keyboard_pl050_ioctl;
	input->priv = pdat;

	request_irq(pdat->irq, keyboard_pl050_interrupt, IRQ_TYPE_NONE, input);
	clk_enable(pdat->clk);
	rate = clk_get_rate(pdat->clk);
	write8(pdat->virt + KEYBOARD_CLKDIV, (u32_t)(rate / 8000000) - 1);
	write8(pdat->virt + KEYBOARD_CR, (1 << 2));
	kmi_read(pdat, &value);
	kmi_write(pdat, 0xff);
	kmi_read(pdat, &value);
	kmi_write(pdat, 0xf3);
	kmi_write(pdat, 0x2b);
	kmi_write(pdat, 0xf0);
	kmi_write(pdat, 0x02);
	kmi_write(pdat, 0xfa);
	kmi_write(pdat, 0xed);
	kmi_write(pdat, 0x02);
	write8(pdat->virt + KEYBOARD_CR, (1 << 2) | (1 << 4));

	if(!register_input(&dev, input))
	{
		write8(pdat->virt + KEYBOARD_CR, 0);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void keyboard_pl050_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct keyboard_pl050_pdata_t * pdat = (struct keyboard_pl050_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		write8(pdat->virt + KEYBOARD_CR, 0);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void keyboard_pl050_suspend(struct device_t * dev)
{
}

static void keyboard_pl050_resume(struct device_t * dev)
{
}

static struct driver_t keyboard_pl050 = {
	.name		= "keyboard-pl050",
	.probe		= keyboard_pl050_probe,
	.remove		= keyboard_pl050_remove,
	.suspend	= keyboard_pl050_suspend,
	.resume		= keyboard_pl050_resume,
};

static __init void keyboard_pl050_driver_init(void)
{
	register_driver(&keyboard_pl050);
}

static __exit void keyboard_pl050_driver_exit(void)
{
	unregister_driver(&keyboard_pl050);
}

driver_initcall(keyboard_pl050_driver_init);
driver_exitcall(keyboard_pl050_driver_exit);
