/*
 * driver/gpio-keypad.c
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
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <time/tick.h>
#include <time/timer.h>
#include <input/keyboard/keyboard.h>
#include <s5pv210/reg-gpio.h>

static struct timer_list button_timer;

static void button_timer_function(u32_t data)
{
	static u32_t key_old = 0x7f;
	u32_t keyup, keydown;
	u32_t key = 0;

	if(readl(S5PV210_GPH2DAT) & (0x1<<0))   /* GPH2_0, SW7 */
	{
		key &= ~(0x1 << 0);
	}
	else
	{
		key |= 0x1 << 0;
	}

	if(readl(S5PV210_GPH0DAT) & (0x1<<3))   /* GPH0_3, SW6 */
	{
		key &= ~(0x1 << 1);
	}
	else
	{
		key |= 0x1 << 1;
	}

	if(readl(S5PV210_GPH0DAT) & (0x1<<2))   /* GPH0_2, SW5 */
	{
		key &= ~(0x1 << 2);
	}
	else
	{
		key |= 0x1 << 2;
	}

	if(readl(S5PV210_GPH2DAT) & (0x1<<1))   /* GPH2_1, SW8 */
	{
		key &= ~(0x1 << 3);
	}
	else
	{
		key |= 0x1 << 3;
	}

	if(readl(S5PV210_GPH2DAT) & (0x1<<3))   /* GPH2_3, SW10 */
	{
		key &= ~(0x1 << 4);
	}
	else
	{
		key |= 0x1 << 4;
	}

	if(readl(S5PV210_GPH2DAT) & (0x1<<2))   /* GPH2_2, SW9 */
	{
		key &= ~(0x1 << 5);
	}
	else
	{
		key |= 0x1 << 5;
	}

	if(readl(S5PV210_GPH0DAT) & (0x1<<1))   /* GPH0_1, SW12 */
	{
		key &= ~(0x1 << 6);
	}
	else
	{
		key |= 0x1 << 6;
	}

	if(key != key_old)
	{
		keyup = (key ^ key_old) & key_old;
		keydown = (key ^ key_old) & key;
		key_old = key;

		if(keyup)
		{
			if(keyup & (0x1<<0))
				input_report(INPUT_KEYBOARD, KEY_UP, KEY_BUTTON_UP);
			else if(keyup & (0x1<<1))
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_UP);
			else if(keyup & (0x1<<2))
				input_report(INPUT_KEYBOARD, KEY_LEFT, KEY_BUTTON_UP);
			else if(keyup & (0x1<<3))
				input_report(INPUT_KEYBOARD, KEY_RIGHT, KEY_BUTTON_UP);
			else if(keyup & (0x1<<4))
				input_report(INPUT_KEYBOARD, KEY_MENU, KEY_BUTTON_UP);
			else if(keyup & (0x1<<5))
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_UP);
			else if(keyup & (0x1<<6))
				input_report(INPUT_KEYBOARD, KEY_POWER, KEY_BUTTON_UP);
		}

		if(keydown)
		{
			if(keydown & (0x1<<0))
				input_report(INPUT_KEYBOARD, KEY_UP, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<1))
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<2))
				input_report(INPUT_KEYBOARD, KEY_LEFT, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<3))
				input_report(INPUT_KEYBOARD, KEY_RIGHT, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<4))
				input_report(INPUT_KEYBOARD, KEY_MENU, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<5))
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<6))
				input_report(INPUT_KEYBOARD, KEY_POWER, KEY_BUTTON_DOWN);
		}

		input_sync(INPUT_KEYBOARD);
	}

	/* mod timer for next 100 ms */
	mod_timer(&button_timer, jiffies + get_system_hz() / 10);
}

static bool_t button_probe(struct input * input)
{
	/* set GPH0_1 intput and pull up */
	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0xf<<4)) | (0x0<<4));
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<2)) | (0x2<<2));

	/* set GPH0_2 intput and pull up */
	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0xf<<8)) | (0x0<<8));
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<4)) | (0x2<<4));

	/* set GPH0_3 intput and pull up */
	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0xf<<12)) | (0x0<<12));
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<6)) | (0x2<<6));

	/* set GPH2_0 intput and pull up */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<0)) | (0x0<<0));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<0)) | (0x2<<0));

	/* set GPH2_1 intput and pull up */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<4)) | (0x0<<4));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<2)) | (0x2<<2));

	/* set GPH2_2 intput and pull up */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<8)) | (0x0<<8));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<4)) | (0x2<<4));

	/* set GPH2_3 intput and pull up */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<12)) | (0x0<<12));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<6)) | (0x2<<6));

	/* setup timer for button */
	setup_timer(&button_timer, button_timer_function, 0);

	/* mod timer for 100 ms */
	mod_timer(&button_timer, jiffies + get_system_hz() / 10);

	return TRUE;
}

static bool_t button_remove(struct input * input)
{
	return TRUE;
}

static int button_ioctl(struct input * input, int cmd, void * arg)
{
	return -1;
}

static struct input gpio_button = {
	.name		= "button",
	.type		= INPUT_KEYBOARD,
	.probe		= button_probe,
	.remove		= button_remove,
	.ioctl		= button_ioctl,
	.priv		= NULL,
};

static __init void gpio_button_init(void)
{
	if(!register_input(&gpio_button))
		LOG_E("failed to register input '%s'", gpio_button.name);
}

static __exit void gpio_button_exit(void)
{
	if(!unregister_input(&gpio_button))
		LOG_E("failed to unregister input '%s'", gpio_button.name);
}

device_initcall(gpio_button_init);
device_exitcall(gpio_button_exit);
