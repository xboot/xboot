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
	static u32_t gpm_old = 0x0;
	u32_t gpm = readl(S3C6410_GPMDAT) & 0x1d;
	u32_t keyup, keydown;

	if(gpm != gpm_old)
	{
		keyup = (gpm ^ gpm_old) & gpm_old;
		keydown = (gpm ^ gpm_old) & gpm;
		gpm_old = gpm;

		if(keyup)
		{
			if(keyup & (0x1<<0))				/* gpm0 */
				input_report(INPUT_KEYBOARD, KEY_UP, KEY_BUTTON_UP);
			else if(keyup & (0x1<<3))			/* gpm3 */
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_UP);
			else if(keyup & (0x1<<2))			/* gpm2 */
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_UP);
			else if(keyup & (0x1<<4))			/* gpm4 */
				input_report(INPUT_KEYBOARD, KEY_POWER, KEY_BUTTON_UP);
		}

		if(keydown)
		{
			if(keydown & (0x1<<0))				/* gpm0 */
				input_report(INPUT_KEYBOARD, KEY_UP, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<3))			/* gpm3 */
				input_report(INPUT_KEYBOARD, KEY_DOWN, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<2))			/* gpm2 */
				input_report(INPUT_KEYBOARD, KEY_ENTER, KEY_BUTTON_DOWN);
			else if(keydown & (0x1<<4))			/* gpm4 */
				input_report(INPUT_KEYBOARD, KEY_POWER, KEY_BUTTON_DOWN);
		}

		input_sync(INPUT_KEYBOARD);
	}

	/* mod timer for next 10 ms */
	mod_timer(&keypad_timer, jiffies + get_system_hz() / 100);
}

static bool_t keypad_probe(struct input * input)
{
	/* set gpm0 intput and pull up */
	writel(S3C6410_GPMCON, (readl(S3C6410_GPMCON) & ~(0xf<<0)) | (0x0<<0));
	writel(S3C6410_GPMPUD, (readl(S3C6410_GPMPUD) & ~(0x3<<0)) | (0x2<<0));

	/* set gpm2 intput and pull up */
	writel(S3C6410_GPMCON, (readl(S3C6410_GPMCON) & ~(0xf<<8)) | (0x0<<8));
	writel(S3C6410_GPMPUD, (readl(S3C6410_GPMPUD) & ~(0x3<<4)) | (0x2<<4));

	/* set gpm3 intput and pull up */
	writel(S3C6410_GPMCON, (readl(S3C6410_GPMCON) & ~(0xf<<12)) | (0x0<<12));
	writel(S3C6410_GPMPUD, (readl(S3C6410_GPMPUD) & ~(0x3<<6)) | (0x2<<6));

	/* set gpm4 intput and pull up */
	writel(S3C6410_GPMCON, (readl(S3C6410_GPMCON) & ~(0xf<<16)) | (0x0<<16));
	writel(S3C6410_GPMPUD, (readl(S3C6410_GPMPUD) & ~(0x3<<8)) | (0x2<<8));


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
