/*
 * resource/res-led-heartbeat.c
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

/*
 * led trigger for heartbeat using gpn15.
 */
static void led_gpn15_init(void)
{
	/* set GPN15 output and pull up */
	writel(S3C6410_GPNCON, (readl(S3C6410_GPNCON) & ~(0x3<<30)) | (0x1<<30));
	writel(S3C6410_GPNPUD, (readl(S3C6410_GPNPUD) & ~(0x3<<30)) | (0x2<<30));
}

static void led_gpn15_set(u8_t brightness)
{
	if(brightness)
		writel(S3C6410_GPNDAT, (readl(S3C6410_GPNDAT) & ~(0x1<<15)) | (0x1<<15));
	else
		writel(S3C6410_GPNDAT, (readl(S3C6410_GPNDAT) & ~(0x1<<15)) | (0x0<<15));
}

static struct led led_gpn15 = {
	.name		= "led-gpn15",
	.init		= led_gpn15_init,
	.set		= led_gpn15_set,
};

/*
 * the led-heartbeat resource.
 */
static struct resource led_heartbeat = {
	.name		= "led-heartbeat",
	.data		= &led_gpn15,
};

static __init void dev_heartbeat_init(void)
{
	if(!register_resource(&led_heartbeat))
		LOG_E("failed to register resource '%s'", led_heartbeat.name);
}

static __exit void dev_heartbeat_exit(void)
{
	if(!unregister_resource(&led_heartbeat))
		LOG_E("failed to unregister resource '%s'", led_heartbeat.name);
}

core_initcall(dev_heartbeat_init);
core_exitcall(dev_heartbeat_exit);
