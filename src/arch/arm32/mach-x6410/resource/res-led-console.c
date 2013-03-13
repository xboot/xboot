/*
 * resource/res-led-console.c
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
#include <io.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <led/led.h>
#include <led/trigger.h>
#include <xboot/resource.h>
#include <s3c6410/reg-gpio.h>

static void led_gpm1_init(void)
{
	/* set GPM1 output and pull up */
	writel(S3C6410_GPMCON, (readl(S3C6410_GPMCON) & ~(0xf<<4)) | (0x1<<4));
	writel(S3C6410_GPMPUD, (readl(S3C6410_GPMPUD) & ~(0x3<<2)) | (0x2<<2));
}

static void led_gpm1_set(u8_t brightness)
{
	if(brightness)
		writel(S3C6410_GPMDAT, (readl(S3C6410_GPMDAT) & ~(0x1<<1)) | (0x0<<1));
	else
		writel(S3C6410_GPMDAT, (readl(S3C6410_GPMDAT) & ~(0x1<<1)) | (0x1<<1));
}

static struct led led_gpm1 = {
	.name		= "led-gpm1",
	.init		= led_gpm1_init,
	.set		= led_gpm1_set,
};

/*
 * the led-console resource.
 */
static struct resource led_console = {
	.name		= "led-console",
	.data		= &led_gpm1,
};

static __init void dev_console_init(void)
{
	if(!register_resource(&led_console))
		LOG_E("failed to register resource '%s'", led_console.name);
}

static __exit void dev_console_exit(void)
{
	if(!unregister_resource(&led_console))
		LOG_E("failed to unregister resource '%s'", led_console.name);
}

core_initcall(dev_console_init);
core_exitcall(dev_console_exit);
