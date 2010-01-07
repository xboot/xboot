/*
 * devices/dev-led_communication.c
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
#include <debug.h>
#include <xboot/io.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <led/led.h>
#include <led/led-trigger.h>
#include <xboot/platform_device.h>
#include <s3c6410/reg-gpio.h>

/*
 * led trigger for communication using gpn14.
 */
static void led_gpn14_init(void)
{
	/* set GPN14 output and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<28)) | (0x1<<28));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<28)) | (0x2<<28));
}

static void led_gpn14_set(x_u8 brightness)
{
	if(brightness)
		writel(S3C6410_GPNDAT, (readl(S3C6410_GPNDAT) & ~(0x1<<14)) | (0x1<<14));
	else
		writel(S3C6410_GPNDAT, (readl(S3C6410_GPNDAT) & ~(0x1<<14)) | (0x0<<14));
}

static struct led led_gpn14 = {
	.name		= "led-gpn14",
	.init		= led_gpn14_init,
	.set		= led_gpn14_set,
};

/*
 * the led-communication platform devices.
 */
static struct platform_device led_communication = {
	.name		= "led-communication",
	.data		= &led_gpn14,
};

static __init void dev_communication_init(void)
{
	if(!platform_device_register(&led_communication))
		DEBUG_E("failed to register platform  device '%s'", led_communication.name);
}

static __exit void dev_communication_exit(void)
{
	if(!platform_device_unregister(&led_communication))
		DEBUG_E("failed to unregister platform device '%s'", led_communication.name);
}

module_init(dev_communication_init, LEVEL_MACH_RES);
module_exit(dev_communication_exit, LEVEL_MACH_RES);
