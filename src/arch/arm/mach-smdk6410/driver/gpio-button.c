/*
 * driver/gpio-button.c
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
#include <s3c6410/reg-gpio.h>


static void button_init(void)
{
	/* set GPN10 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<20)) | (0x0<<20));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<20)) | (0x2<<20));

	/* set GPN11 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<22)) | (0x0<<22));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<22)) | (0x2<<22));

	/* set GPN9 intput and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<18)) | (0x0<<18));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<18)) | (0x2<<18));
}

static void button_exit(void)
{
	return;
}

static x_bool button_read(enum keycode * code)
{
	static x_u32 gpn_old = 0x7;
	x_u32 gpn = readl(S3C6410_GPNDAT) & (0x7<<9);

	if(gpn != gpn_old)
	{
		gpn_old = gpn;

		if(!(gpn & (0x1<<10)))
		{
			*code = KEY_UP;
			return TRUE;
		}
		else if(!(gpn & (0x1<<11)))
		{
			*code = KEY_DOWN;
			return TRUE;
		}
		else if(!(gpn & (0x1<<9)))
		{
			*code = KEY_ENTER;
			return TRUE;
		}
		else
			return FALSE;
	}

	return FALSE;
}

static void button_flush(void)
{
	return;
}

static x_s32 button_ioctl(x_u32 cmd, x_u32 arg)
{
	return -1;
}

static struct keyboard_driver gpio_button = {
	.name		= "button",
	.init		= button_init,
	.exit		= button_exit,
	.read		= button_read,
	.flush		= button_flush,
	.ioctl		= button_ioctl,
};

static __init void gpio_button_init(void)
{
	if(!register_keyboard(&gpio_button))
		LOG_E("failed to register keyboard driver '%s'", gpio_button.name);
}

static __exit void gpio_button_exit(void)
{
	if(!unregister_keyboard(&gpio_button))
		LOG_E("failed to unregister keyboard driver '%s'", gpio_button.name);
}

module_init(gpio_button_init, LEVEL_DRIVER);
module_exit(gpio_button_exit, LEVEL_DRIVER);
