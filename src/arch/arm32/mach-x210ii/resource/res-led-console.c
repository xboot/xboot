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
#include <s5pv210/reg-gpio.h>

/*
 * led trigger for console using gpj0_3.
 */
static void led_init(void)
{
	writel(S5PV210_GPJ0CON, (readl(S5PV210_GPJ0CON) & ~(0xf<<12)) | (0x1<<12));
	writel(S5PV210_GPJ0PUD, (readl(S5PV210_GPJ0PUD) & ~(0x3<<6)) | (0x2<<6));
	writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x1<<3));
}

static void led_set(u8_t brightness)
{
	if(brightness)
		writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x0<<3));
	else
		writel(S5PV210_GPJ0DAT, (readl(S5PV210_GPJ0DAT) & ~(0x1<<3)) | (0x1<<3));
}

static struct led led = {
	.name		= "led-gpj0_3",
	.init		= led_init,
	.set		= led_set,
};

/*
 * the led-console resource.
 */
static struct resource led_console = {
	.name		= "led-console",
	.data		= &led,
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
