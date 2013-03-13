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
#include <s5pv210/reg-gpio.h>

/*
 * led trigger for heartbeat using gpj0_4.
 */
static void led_init(void)
{
	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<16)) | (0x1<<16));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<8)) | (0x2<<8));
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x1<<4));
}

static void led_set(u8_t brightness)
{
	if(brightness)
		writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x0<<4));
	else
		writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<4)) | (0x1<<4));
}

static struct led led = {
	.name		= "led-gpj0_4",
	.init		= led_init,
	.set		= led_set,
};

/*
 * the led-heartbeat resource.
 */
static struct resource led_heartbeat = {
	.name		= "led-heartbeat",
	.data		= &led,
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
