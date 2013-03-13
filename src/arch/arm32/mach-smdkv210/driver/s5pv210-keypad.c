/*
 * driver/s5pv210-keypad.c
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
#include <string.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/ioctl.h>
#include <xboot/irq.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/keyboard/keyboard.h>
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-keypad.h>
#include <s5pv210-keypad.h>

static struct timer_list timer;
static u32_t keymask[8];
static u32_t prevmask[8];

static void keypad_timer_function(u32_t data)
{
	struct s5pv210_keypad * keypad = (struct s5pv210_keypad *)data;
	u32_t press_mask;
	u32_t release_mask;
	u32_t col, i;

	for(col = 0; col < keypad->cols; col++)
	{
		writel(S5PV210_KEYPAD_COL, 0xffff & ~(0x101 << col));
		keymask[col] = ~(readl(S5PV210_KEYPAD_ROW) & ((0x1 << keypad->rows) - 1));
	}

	for(col = 0; col < keypad->cols; col++)
	{
		press_mask = ((keymask[col] ^ prevmask[col]) & keymask[col]);
		release_mask = ((keymask[col] ^ prevmask[col]) & prevmask[col]);

		i = col * 14;
		while(press_mask)
		{
			if(press_mask & 0x1)
			{
				input_report(INPUT_KEYBOARD, keypad->keycode[i], KEY_BUTTON_DOWN);
				input_sync(INPUT_KEYBOARD);
			}
			press_mask >>= 1;
			i++;
		}

		i = col * 14;
		while(release_mask)
		{
			if(release_mask & 0x1)
			{
				input_report(INPUT_KEYBOARD, keypad->keycode[i], KEY_BUTTON_UP);
				input_sync(INPUT_KEYBOARD);
			}
			release_mask >>= 1;
			i++;
		}

		prevmask[col] = keymask[col];
	}

	mod_timer(&timer, jiffies + get_system_hz() / 10);
}

static bool_t keypad_probe(struct input * input)
{
	struct s5pv210_keypad * keypad = (struct s5pv210_keypad *)(input->priv);
	u32_t i;

	/* set GPJ1_5 for KP_COL0, and pull none */
	writel(S5PV210_GPJ1CON, (readl(S5PV210_GPJ1CON) & ~(0xf<<20)) | (0x3<<20));
	writel(S5PV210_GPJ1PUD, (readl(S5PV210_GPJ1PUD) & ~(0x3<<10)) | (0x0<<10));

	/* set GPJ2 for KP_COL1 ~ KP_COL7 and KP_ROW0, and pull none*/
	writel(S5PV210_GPJ2CON, 0x33333333);
	writel(S5PV210_GPJ2PUD, 0x00000000);

	/* set GPJ3 for KP_ROW1 ~ KP_ROW8, and pull none*/
	writel(S5PV210_GPJ3CON, 0x33333333);
	writel(S5PV210_GPJ3PUD, 0x00000000);

	/* set GPJ4_0 to GPJ4_5 for KP_ROW9 ~ KP_ROW13, and pull none*/
	writel(S5PV210_GPJ4CON, (readl(S5PV210_GPJ4CON) & ~(0x000fffff)) | (0x00033333));
	writel(S5PV210_GPJ4PUD, (readl(S5PV210_GPJ4PUD) & ~(0x000003ff)) | (0x00000000));

	writel(S5PV210_KEYPAD_CON, 0);
	writel(S5PV210_KEYPAD_FC, 0);
	writel(S5PV210_KEYPAD_STSCLR, 0x3fffffff);
	writel(S5PV210_KEYPAD_COL, (readl(S5PV210_KEYPAD_COL) & ~0xffff));

	for(i = 0; i < 8; i++)
	{
		prevmask[i] = keymask[i] = ~((0x1 << keypad->rows) - 1);
	}

	setup_timer(&timer, keypad_timer_function, (u32_t)keypad);
	mod_timer(&timer, jiffies + get_system_hz() / 10);

	return TRUE;
}

static bool_t keypad_remove(struct input * input)
{
	del_timer(&timer);

	return TRUE;
}

static int keypad_ioctl(struct input * input, int cmd, void * arg)
{
	return -1;
}

static struct input matrix_keypad = {
	.name		= "keypad",
	.type		= INPUT_KEYBOARD,
	.probe		= keypad_probe,
	.remove		= keypad_remove,
	.ioctl		= keypad_ioctl,
	.priv		= NULL,
};

static __init void matrix_keypad_init(void)
{
	matrix_keypad.priv = resource_get_data(matrix_keypad.name);
	if(! matrix_keypad.priv)
	{
		LOG_W("can't get the resource of \'%s\'", matrix_keypad.name);
		return;
	}

	if(!register_input(&matrix_keypad))
		LOG_E("failed to register input '%s'", matrix_keypad.name);
}

static __exit void matrix_keypad_exit(void)
{
	if(!unregister_input(&matrix_keypad))
		LOG_E("failed to unregister input '%s'", matrix_keypad.name);
}

device_initcall(matrix_keypad_init);
device_exitcall(matrix_keypad_exit);
