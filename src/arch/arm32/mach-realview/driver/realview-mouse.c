/*
 * driver/realview_mouse.c
 *
 * realview mouse drivers, the primecell pl050 ps2 controller.
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
#include <spinlock.h>
#include <xboot/irq.h>
#include <input/input.h>
#include <realview/reg-mouse.h>

static bool_t kmi_write(u8_t data)
{
	s32_t timeout = 1000;

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

static bool_t kmi_read(u8_t * data)
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
	static u8_t packet[4], index = 0;
	static u8_t btn_old = 0;
	struct event_t event;
	s32_t x, y, z;
	u8_t btndown, btnup, btn;
	u8_t status;

	status = readb(REALVIEW_MOUSE_IIR);
	while(status & REALVIEW_MOUSE_IIR_RXINTR)
	{
		packet[index] = readb(REALVIEW_MOUSE_DATA);
		index = (index + 1) & 0x3;

		if(index == 0)
		{
			btn = packet[0] & 0x7;

			btndown = (btn ^ btn_old) & btn;
			btnup = (btn ^ btn_old) & btn_old;
			btn_old = btn;

			if(packet[0] & 0x10)
				x = 0xffffff00 | packet[1];
			else
				x = packet[1];

			if(packet[0] & 0x20)
				y = 0xffffff00 | packet[2];
			else
				y = packet[2];

			z = packet[3] & 0xf;
			if(z == 0xf)
				z = -1;

			event.type = EVENT_TYPE_MOUSE_RAW;
			event.e.mouse_raw.btndown = btndown;
			event.e.mouse_raw.btnup = btnup;
			event.e.mouse_raw.xrel = x;
			event.e.mouse_raw.yrel = y;
			event.e.mouse_raw.zrel = z;

			event_send(&event);
		}

		status = readb(REALVIEW_MOUSE_IIR);
	}
}

static bool_t mouse_probe(struct input * input)
{
	u32_t divisor;
	u64_t kclk;
	u8_t data;

	if(! clk_get_rate("kclk", &kclk))
	{
		LOG_E("can't get the clock of 'kclk'");
		return FALSE;
	}

	/* set mouse's clock divisor */
	divisor = (u32_t)(div64(kclk, 8000000) - 1);
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

static bool_t mouse_remove(struct input * input)
{
	if(!free_irq("KMI1"))
		LOG_E("can't free irq 'KMI1'");
	writeb(REALVIEW_MOUSE_CR, 0);

	return TRUE;
}

static int mouse_ioctl(struct input * input, int cmd, void * arg)
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

device_initcall(realview_mouse_init);
device_exitcall(realview_mouse_exit);
