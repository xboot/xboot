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
	static u32_t key_old = 0x3;
	u32_t keyup, keydown;
	u32_t key = 0;

	if(readl(S5PV210_GPH0DAT) & (0x1<<4))
	{
		key |= 0x1 << 0;
	}
	else
	{
		key &= ~(0x1 << 0);
	}

	if(readl(S5PV210_GPH3DAT) & (0x1<<7))
	{
		key |= 0x1 << 1;
	}
	else
	{
		key &= ~(0x1 << 1);
	}

	if(key != key_old)
	{
		keyup = (key ^ key_old) & key;
		keydown = (key ^ key_old) & key_old;
		key_old = key;

		if(keyup)
		{
			if(keyup & (0x1<<0))				/* GPH0_4 */
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_UP);
			else if(keyup & (0x1<<1))			/* GPH3_7 */
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_UP);
		}

		if(keydown)
		{
			if(keydown & (0x1<<0))				/* GPH0_4 */
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<1))			/* GPH3_7 */
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_DOWN);
		}

		input_sync(INPUT_KEYBOARD);
	}

	/* mod timer for next 100 ms */
	mod_timer(&button_timer, jiffies + get_system_hz() / 10);
}

static bool_t button_probe(struct input * input)
{
	/* set GPH0_4 intput and pull up */
	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0xf<<16)) | (0x0<<16));
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<8)) | (0x2<<8));

	/* set GPH3_7 intput and pull up */
	writel(S5PV210_GPH3CON, (readl(S5PV210_GPH3CON) & ~(0xf<<28)) | (0x0<<28));
	writel(S5PV210_GPH3PUD, (readl(S5PV210_GPH3PUD) & ~(0x3<<14)) | (0x2<<14));

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
