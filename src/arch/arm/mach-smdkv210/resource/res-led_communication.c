/*
 * resource/res-led_communication.c
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
#include <io.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <led/led.h>
#include <led/led-trigger.h>
#include <xboot/resource.h>
#include <s5pv210/reg-gpio.h>

/*
 * led trigger for communication using gph2_5.
 */
static void led_gph2_5_init(void)
{
	/* set GPH2_5 output and pull up */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<20)) | (0x1<<20));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<10)) | (0x2<<10));
}

static void led_gph2_5_set(x_u8 brightness)
{
	if(brightness)
		writel(S5PV210_GPH2DAT, (readl(S5PV210_GPH2DAT) & ~(0x1<<5)) | (0x1<<5));
	else
		writel(S5PV210_GPH2DAT, (readl(S5PV210_GPH2DAT) & ~(0x1<<5)) | (0x0<<5));
}

static struct led led_gph2_5 = {
	.name		= "led-gph2_5",
	.init		= led_gph2_5_init,
	.set		= led_gph2_5_set,
};

/*
 * the led-communication resource.
 */
static struct resource led_communication = {
	.name		= "led-communication",
	.data		= &led_gph2_5,
};

static __init void dev_communication_init(void)
{
	if(!register_resource(&led_communication))
		LOG_E("failed to register resource '%s'", led_communication.name);
}

static __exit void dev_communication_exit(void)
{
	if(!unregister_resource(&led_communication))
		LOG_E("failed to unregister resource '%s'", led_communication.name);
}

module_init(dev_communication_init, LEVEL_MACH_RES);
module_exit(dev_communication_exit, LEVEL_MACH_RES);
