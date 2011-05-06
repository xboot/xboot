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
#include <stddef.h>
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
#include <input/mouse/mouse.h>
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
	static x_u8 packet[4];
	static x_u8 index = 0;
	x_u8 status, data;
	x_s32 value;

	status = readb(REALVIEW_MOUSE_IIR);

	while(status & REALVIEW_MOUSE_IIR_RXINTR)
	{
		data = readb(REALVIEW_MOUSE_DATA);

		packet[index] = data;
		index = (index + 1) & 0x3;

		if(index == 0)
		{
			if(packet[0] & 0x1)
				input_report(INPUT_MOUSE, MOUSE_LEFT, MOUSE_BUTTON_DOWN);
			else
				input_report(INPUT_MOUSE, MOUSE_LEFT, MOUSE_BUTTON_UP);

			if(packet[0] & 0x2)
				input_report(INPUT_MOUSE, MOUSE_RIGHT, MOUSE_BUTTON_DOWN);
			else
				input_report(INPUT_MOUSE, MOUSE_RIGHT, MOUSE_BUTTON_UP);

			if(packet[0] & 0x4)
				input_report(INPUT_MOUSE, MOUSE_MIDDLE, MOUSE_BUTTON_DOWN);
			else
				input_report(INPUT_MOUSE, MOUSE_MIDDLE, MOUSE_BUTTON_UP);

			if(packet[0] & 0x10)
				value = 0xffffff00 | packet[1];
			else
				value = packet[1];
			input_report(INPUT_MOUSE, MOUSE_REL_X, value);

			if(packet[0] & 0x20)
				value = 0xffffff00 | packet[2];
			else
				value = packet[2];
			input_report(INPUT_MOUSE, MOUSE_REL_Y, value);

			value = packet[3] & 0xf;
			switch(value)
			{
			case 0x0:
				break;

			case 0x1:
				input_report(INPUT_MOUSE, MOUSE_REL_Z, 1);
				break;

			case 0xf:
				input_report(INPUT_MOUSE, MOUSE_REL_Z, -1);
				break;

			default:
				break;
			}

			input_sync(INPUT_MOUSE);
		}

		status = readb(REALVIEW_MOUSE_IIR);
	}
}

static x_bool mouse_probe(struct input * input)
{
	x_u32 divisor;
	x_u64 kclk;
	x_u8 data;

	if(! clk_get_rate("kclk", &kclk))
	{
		LOG_E("can't get the clock of 'kclk'");
		return FALSE;
	}

	/* set mouse's clock divisor */
	divisor = (x_u32)(div64(kclk, 8000000) - 1);
	writeb(REALVIEW_MOUSE_CLKDIV, divisor);

	/* enable mouse controller */
	writeb(REALVIEW_MOUSE_CR, REALVIEW_MOUSE_CR_EN);

	/* reset mouse, and wait ack and pass/fail code */
	if(! kmi_write(0xff) )
		return FALSE;
	if(! kmi_read(&data))
		return FALSE;
	if(data != 0xaa)
		return FALSE;

	/* enable scroll wheel */
	kmi_write(0xf3);
	kmi_write(200);

	kmi_write(0xf3);
	kmi_write(100);

	kmi_write(0xf3);
	kmi_write(80);

	kmi_write(0xf2);
	kmi_read(&data);
	kmi_read(&data);

	/* set sample rate, 100 samples/sec */
	kmi_write(0xf3);
	kmi_write(100);

	/* set resolution, 4 counts per mm, 1:1 scaling */
	kmi_write(0xe8);
	kmi_write(0x02);
	kmi_write(0xe6);

	/* enable data reporting */
	kmi_write(0xf4);

	/* clear a receive buffer */
	kmi_read(&data);
	kmi_read(&data);
	kmi_read(&data);
	kmi_read(&data);

	if(!request_irq("KMI1", mouse_interrupt))
	{
		LOG_E("can't request irq 'KMI1'");
		writeb(REALVIEW_MOUSE_CR, 0);
		return FALSE;
	}

	/* re-enables mouse */
	writeb(REALVIEW_MOUSE_CR, REALVIEW_MOUSE_CR_EN | REALVIEW_MOUSE_CR_RXINTREN);

	return TRUE;
}

static x_bool mouse_remove(struct input * input)
{
	if(!free_irq("KMI1"))
		LOG_E("can't free irq 'KMI1'");
	writeb(REALVIEW_MOUSE_CR, 0);

	return TRUE;
}

static x_s32 mouse_ioctl(struct input * input, x_u32 cmd, void * arg)
{
	return -1;
}

static struct input realview_mouse = {
	.name		= "mouse",
	.type		= INPUT_MOUSE,
	.probe		= mouse_probe,
	.remove		= mouse_remove,
	.ioctl		= mouse_ioctl,
	.priv		= NULL,
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
