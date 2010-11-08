/*
 * driver/realview_mouse.c
 *
 * realview mouse drivers, the primecell pl050 ps2 controller.
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/irq.h>
#include <xboot/printk.h>
#include <xboot/resource.h>
#include <input/input.h>
#include <realview/reg-mouse.h>

static x_bool kmi_write(x_u8 data)
{
	x_s32 timeout = 1000;

	while((readb(REALVIEW_MOUSE_STAT) & REALVIEW_MOUSE_STAT_TXEMPTY) == 0 && timeout--);

	if(timeout)
	{
		writeb(REALVIEW_MOUSE_DATA, data);

		while((readb(REALVIEW_MOUSE_STAT) & REALVIEW_MOUSE_STAT_RXFULL) == 0);

		if( readb(REALVIEW_MOUSE_DATA) == 0xfa)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

static x_bool kmi_read(x_u8 * data)
{
	if( (readb(REALVIEW_MOUSE_STAT) & REALVIEW_MOUSE_STAT_RXFULL) )
	{
		*data = readb(REALVIEW_MOUSE_DATA);
		return TRUE;
	}

	return FALSE;
}

static void mouse_interrupt(void)
{
	x_u8 status, data;

	status = readb(REALVIEW_MOUSE_IIR);
	printk("dd");

	while(status & REALVIEW_MOUSE_IIR_RXINTR)
	{
		data = readb(REALVIEW_MOUSE_DATA);

		printk("0x%02x\r\n", data);

		status = readb(REALVIEW_MOUSE_IIR);
	}
}

static void mouse_probe(void)
{
	x_u32 divisor;
	x_u64 kclk;
	x_u8 data;

	if(! clk_get_rate("kclk", &kclk))
	{
		LOG_E("can't get the clock of 'kclk'");
		return;
	}

	/* set mouse's clock divisor */
	divisor = (x_u32)(div64(kclk, 8000000) - 1);
	writeb(REALVIEW_MOUSE_CLKDIV, divisor);

	/* enable mouse controller */
	writeb(REALVIEW_MOUSE_CR, REALVIEW_MOUSE_CR_EN);

	kmi_write(0xf6);
	kmi_write(0xf4);

#if 0

	/* clear a receive buffer */
	kmi_read(&data);

	/* reset mouse, and wait ack and pass/fail code */
	if(! kmi_write(0xff) )
		return;
	if(! kmi_read(&data))
		return;
	if(data != 0xaa)
		return;

	/* set mouse's typematic rate/delay */
	kmi_write(0xf3);
	/* 10.9pcs, 500ms */
	kmi_write(0x2b);

	/* scan code set 2 */
	kmi_write(0xf0);
	kmi_write(0x02);

	/* set all keys typematic/make/break */
	kmi_write(0xfa);

	/* set mouse's number lock, caps lock, and scroll lock */
	kmi_write(0xed);
	kmi_write(0x02);
#endif

	if(!request_irq("KMI1", mouse_interrupt))
	{
		LOG_E("can't request irq 'KMI1'");
		writeb(REALVIEW_MOUSE_CR, 0);
		return;
	}

	/* re-enables mouse */
	writeb(REALVIEW_MOUSE_CR, REALVIEW_MOUSE_CR_EN | REALVIEW_MOUSE_CR_RXINTREN);
}

static void mouse_remove(void)
{
	if(!free_irq("KMI1"))
		LOG_E("can't free irq 'KMI1'");
	writeb(REALVIEW_MOUSE_CR, 0);
}

static struct input realview_mouse = {
	.name		= "kbd",
	.type		= INPUT_MOUSE,
	.probe		= mouse_probe,
	.remove		= mouse_remove,
};

static __init void realview_mouse_init(void)
{
	if(! clk_get_rate("kclk", 0))
	{
		LOG_E("can't get the clock of 'kclk'");
		return;
	}

	if(!register_input(&realview_mouse))
		LOG_E("failed to register input '%s'", realview_mouse.name);
}

static __exit void realview_mouse_exit(void)
{
	if(!unregister_input(&realview_mouse))
		LOG_E("failed to unregister input '%s'", realview_mouse.name);
}

module_init(realview_mouse_init, LEVEL_DRIVER);
module_exit(realview_mouse_exit, LEVEL_DRIVER);
