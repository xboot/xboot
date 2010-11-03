/*
 * driver/realview_keyboard.c
 *
 * realview keyboard drivers, the primecell pl050 ps2 controller.
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
#include <div64.h>
#include <io.h>
#include <time/delay.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <input/keyboard/keyboard.h>
#include <realview/reg-keyboard.h>


static void keyboard_interrupt(void)
{
	x_u8 status, data;

	status = readb(REALVIEW_KEYBOARD_IIR);

	while(status & REALVIEW_KEYBOARD_IIR_RXINTR)
	{
		data = readb(REALVIEW_KEYBOARD_DATA);
		printk("0x%x,", data);

		status = readb(REALVIEW_KEYBOARD_IIR);
	}
}

static void keyboard_probe(void)
{
	x_u32 divisor;
	x_u64 kclk;

	if(! clk_get_rate("kclk", &kclk))
	{
		LOG_E("can't get the clock of \'kclk\'");
		return;
	}

	divisor = (x_u32)(div64(kclk, 8000000) - 1);
	writeb(REALVIEW_KEYBOARD_CLKDIV, divisor);

	writeb(REALVIEW_KEYBOARD_CR, REALVIEW_KEYBOARD_CR_EN);

	if(!request_irq("KMI0", keyboard_interrupt))
	{
		LOG_E("can't request irq \'KMI0\'");
		writeb(REALVIEW_KEYBOARD_CR, 0);
		return;
	}

	writeb(REALVIEW_KEYBOARD_CR, REALVIEW_KEYBOARD_CR_EN | REALVIEW_KEYBOARD_CR_RXINTREN);
}

static void keyboard_remove(void)
{
	if(!free_irq("KMI0"))
		LOG_E("can't free irq \'KMI0\'");
	writeb(REALVIEW_KEYBOARD_CR, 0);
}

/*


static x_bool keyboard_read(enum keycode * code)
{
	x_u8 stat = readb(REALVIEW_KEYBOARD_STAT);
	x_u8 data;

	if(stat & REALVIEW_KEYBOARD_STAT_RXFULL)
	{
		data = readb(REALVIEW_KEYBOARD_DATA);
		return key_translate(data, code);
	}

	return FALSE;
}

static x_s32 keyboard_ioctl(x_u32 cmd, void * arg)
{
	return -1;
}

static struct keyboard_driver realview_keyboard = {
	.name		= "kbd",
	.init		= keyboard_init,
	.exit		= keyboard_exit,
	.read		= keyboard_read,
	.ioctl		= keyboard_ioctl,
};*/

static __init void realview_keyboard_init(void)
{
	if(! clk_get_rate("kclk", 0))
	{
		LOG_E("can't get the clock of \'kclk\'");
		return;
	}

/*
	if(!register_keyboard(&realview_keyboard))
		LOG_E("failed to register keyboard driver '%s'", realview_keyboard.name);
*/
}

static __exit void realview_keyboard_exit(void)
{
/*
	if(!unregister_keyboard(&realview_keyboard))
		LOG_E("failed to unregister keyboard driver '%s'", realview_keyboard.name);
*/
}

module_init(realview_keyboard_init, LEVEL_DRIVER);
module_exit(realview_keyboard_exit, LEVEL_DRIVER);
