/*
 * driver/gpio-keypad.c
 *
 * gpio key drivers.
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
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/io.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <input/keyboard/keyboard.h>
#include <s5pc100/reg-gpio.h>


static void keypad_init(void)
{
	/* set GPH1_3 intput and pull up */
	writel(S5PC100_GPH1CON, (readl(S5PC100_GPH1CON) & ~(0xf<<12)) | (0x0<<12));
	writel(S5PC100_GPH1PUD, (readl(S5PC100_GPH1PUD) & ~(0x3<<6)) | (0x2<<6));

	/* set GPH3_7 intput and pull up */
	writel(S5PC100_GPH3CON, (readl(S5PC100_GPH3CON) & ~(0xf<<28)) | (0x0<<28));
	writel(S5PC100_GPH3PUD, (readl(S5PC100_GPH3PUD) & ~(0x3<<14)) | (0x2<<14));
}

static void keypad_exit(void)
{
	return;
}

static x_bool keypad_read(enum keycode * code)
{
	static x_u32 key_old = 0x3;
	x_u32 key = 0;

	if(readl(S5PC100_GPH1DAT) & (0x1<<3))
	{
		key |= 0x1 << 0;
	}
	else
	{
		key &= ~(0x1 << 0);
	}

	if(readl(S5PC100_GPH3DAT) & (0x1<<7))
	{
		key |= 0x1 << 1;
	}
	else
	{
		key &= ~(0x1 << 1);
	}

	if(key != key_old)
	{
		key_old = key;

		if(!(key & (0x1<<0)))
		{
			*code = KEY_DOWN;
			return TRUE;
		}
		else if(!(key & (0x1<<1)))
		{
			*code = KEY_ENTER;
			return TRUE;
		}
		else
			return FALSE;
	}

	return FALSE;
}

static x_s32 keypad_ioctl(x_u32 cmd, void * arg)
{
	return -1;
}

static struct keyboard_driver gpio_keypad = {
	.name		= "keypad",
	.init		= keypad_init,
	.exit		= keypad_exit,
	.read		= keypad_read,
	.ioctl		= keypad_ioctl,
};

static __init void gpio_keypad_init(void)
{
	if(!register_keyboard(&gpio_keypad))
		LOG_E("failed to register keyboard driver '%s'", gpio_keypad.name);
}

static __exit void gpio_keypad_exit(void)
{
	if(!unregister_keyboard(&gpio_keypad))
		LOG_E("failed to unregister keyboard driver '%s'", gpio_keypad.name);
}

module_init(gpio_keypad_init, LEVEL_DRIVER);
module_exit(gpio_keypad_exit, LEVEL_DRIVER);
