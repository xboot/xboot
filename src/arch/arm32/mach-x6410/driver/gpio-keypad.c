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
#include <s3c6410/reg-gpio.h>

static struct timer_list keypad_timer;

static void keypad_timer_function(u32_t data)
{
	static u32_t gpn_old = 0x3f;
	u32_t gpn = readl(S3C6410_GPNDAT) & 0x3f;
	u32_t keyup, keydown;

	if(gpn != gpn_old)
	{
		keyup = (gpn ^ gpn_old) & gpn;
		keydown = (gpn ^ gpn_old) & gpn_old;
		gpn_old = gpn;

		if(keyup)
		{
			if(keyup & (0x1<<0))				/* gpn0 */
				input_report(INPUT_KEYBOARD, KEY_LEFT, KEY_BUTTON_UP);
			else if(keyup & (0x1<<1))			/* gpn1 */
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_UP);
			else if(keyup & (0x1<<2))			/* gpn2 */
				input_report(INPUT_KEYBOARD, KEY_UP, KEY_BUTTON_UP);
			else if(keyup & (0x1<<3))			/* gpn3 */
				input_report(INPUT_KEYBOARD, KEY_RIGHT, KEY_BUTTON_UP);
			else if(keyup & (0x1<<4))			/* gpn4 */
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_UP);
			else if(keyup & (0x1<<5))			/* gpn5 */
				input_report(INPUT_KEYBOARD, KEY_BACK, KEY_BUTTON_UP);
		}

		if(keydown)
		{
			if(keydown & (0x1<<0))				/* gpn0 */
				input_report(INPUT_KEYBOARD, KEY_LEFT, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<1))			/* gpn1 */
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<2))			/* gpn2 */
				input_report(INPUT_KEYBOARD, KEY_UP, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<3))			/* gpn3 */
				input_report(INPUT_KEYBOARD, KEY_RIGHT, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<4))			/* gpn4 */
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<5))			/* gpn5 */
				input_report(INPUT_KEYBOARD, KEY_BACK, KEY_BUTTON_DOWN);
		}

		input_sync(INPUT_KEYBOARD);
	}

	/* mod timer for next 10 ms */
	mod_timer(&keypad_timer, jiffies + get_system_hz() / 100);
}

static bool_t keypad_probe(struct input * input)
{
	/* set gpn0 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<0)) | (0x0<<0));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<0)) | (0x2<<0));

	/* set gpn1 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<2)) | (0x0<<2));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<2)) | (0x2<<2));

	/* set gpn2 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<4)) | (0x0<<4));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<4)) | (0x2<<4));

	/* set gpn3 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<6)) | (0x0<<6));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<6)) | (0x2<<6));

	/* set gpn4 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<8)) | (0x0<<8));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<8)) | (0x2<<8));

	/* set gpn5 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<10)) | (0x0<<10));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<10)) | (0x2<<10));

	/* setup timer for keypad */
	setup_timer(&keypad_timer, keypad_timer_function, 0);

	/* mod timer for 10 ms */
	mod_timer(&keypad_timer, jiffies + get_system_hz() / 100);

	return TRUE;
}

static bool_t keypad_remove(struct input * input)
{
	return TRUE;
}

static int keypad_ioctl(struct input * input, int cmd, void * arg)
{
	return -1;
}

static struct input gpio_keypad = {
	.name		= "keypad",
	.type		= INPUT_KEYBOARD,
	.probe		= keypad_probe,
	.remove		= keypad_remove,
	.ioctl		= keypad_ioctl,
	.priv		= NULL,
};

static __init void gpio_keypad_init(void)
{
	if(!register_input(&gpio_keypad))
		LOG_E("failed to register input '%s'", gpio_keypad.name);
}

static __exit void gpio_keypad_exit(void)
{
	if(!unregister_input(&gpio_keypad))
		LOG_E("failed to unregister input '%s'", gpio_keypad.name);
}

device_initcall(gpio_keypad_init);
device_exitcall(gpio_keypad_exit);
