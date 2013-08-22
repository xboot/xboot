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
#include <realview-mouse.h>

static bool_t kmi_write(struct realview_mouse_data_t * dat, u8_t value)
{
	s32_t timeout = 1000;

	while((readb(dat->regbase + REALVIEW_MOUSE_OFFSET_STAT) & REALVIEW_MOUSE_STAT_TXEMPTY) == 0 && timeout--);

	if(timeout)
	{
		writeb(dat->regbase + REALVIEW_MOUSE_OFFSET_DATA, value);

		while((readb(dat->regbase + REALVIEW_MOUSE_OFFSET_STAT) & REALVIEW_MOUSE_STAT_RXFULL) == 0);

		if( readb(dat->regbase + REALVIEW_MOUSE_OFFSET_DATA) == 0xfa)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

static bool_t kmi_read(struct realview_mouse_data_t * dat, u8_t * value)
{
	if( (readb(dat->regbase + REALVIEW_MOUSE_OFFSET_STAT) & REALVIEW_MOUSE_STAT_RXFULL) )
	{
		*value = readb(dat->regbase + REALVIEW_MOUSE_OFFSET_DATA);
		return TRUE;
	}

	return FALSE;
}

static struct input_t * data;
static void mouse_interrupt(void)
{
	struct input_t * input = (struct input_t *)data;
	struct resource_t * res = (struct resource_t *)input->priv;
	struct realview_mouse_data_t * dat = (struct realview_mouse_data_t *)res->data;

	static u8_t packet[4], index = 0;
	static u8_t btn_old = 0;
	s32_t relx, rely, delta;
	u32_t btndown, btnup, btn;
	u8_t status;

	status = readb(dat->regbase + REALVIEW_MOUSE_OFFSET_IIR);
	while(status & REALVIEW_MOUSE_IIR_RXINTR)
	{
		packet[index] = readb(dat->regbase + REALVIEW_MOUSE_OFFSET_DATA);
		index = (index + 1) & 0x3;

		if(index == 0)
		{
			btn = packet[0] & 0x7;

			btndown = (btn ^ btn_old) & btn;
			btnup = (btn ^ btn_old) & btn_old;
			btn_old = btn;

			if(packet[0] & 0x10)
				relx = 0xffffff00 | packet[1];
			else
				relx = packet[1];

			if(packet[0] & 0x20)
				rely = 0xffffff00 | packet[2];
			else
				rely = packet[2];
			rely = -rely;

			delta = packet[3] & 0xf;
			if(delta == 0xf)
				delta = -1;

			push_event_mouse("ps2-mouse", btndown, btnup, relx, rely, delta);
		}

		status = readb(dat->regbase + REALVIEW_MOUSE_OFFSET_IIR);
	}
}

static void input_init(struct input_t * input)
{
	struct resource_t * res = (struct resource_t *)input->priv;
	struct realview_mouse_data_t * dat = (struct realview_mouse_data_t *)res->data;
	u32_t divisor;
	u64_t kclk;
	u8_t data;

	if(! clk_get_rate("kclk", &kclk))
		return;

	/* Set mouse's clock divisor */
	divisor = (u32_t)(kclk /8000000) - 1;
	writeb(dat->regbase + REALVIEW_MOUSE_OFFSET_CLKDIV, divisor);

	/* Enable mouse controller */
	writeb(dat->regbase + REALVIEW_MOUSE_OFFSET_CR, REALVIEW_MOUSE_CR_EN);

	/* Reset mouse, and wait ack and pass/fail code */
	if(! kmi_write(dat, 0xff) )
		return;
	if(! kmi_read(dat, &data))
		return;
	if(data != 0xaa)
		return;

	/* Enable scroll wheel */
	kmi_write(dat, 0xf3);
	kmi_write(dat, 200);

	kmi_write(dat, 0xf3);
	kmi_write(dat, 100);

	kmi_write(dat, 0xf3);
	kmi_write(dat, 80);

	kmi_write(dat, 0xf2);
	kmi_read(dat, &data);
	kmi_read(dat, &data);

	/* Set sample rate, 100 samples/sec */
	kmi_write(dat, 0xf3);
	kmi_write(dat, 100);

	/* Set resolution, 4 counts per mm, 1:1 scaling */
	kmi_write(dat, 0xe8);
	kmi_write(dat, 0x02);
	kmi_write(dat, 0xe6);

	/* Enable data reporting */
	kmi_write(dat, 0xf4);

	/* Clear a receive buffer */
	kmi_read(dat, &data);
	kmi_read(dat, &data);
	kmi_read(dat, &data);
	kmi_read(dat, &data);

	if(!request_irq("KMI1", mouse_interrupt))
	{
		LOG("Can't request irq 'KMI1'");
		writeb(dat->regbase + REALVIEW_MOUSE_OFFSET_CR, 0);
		return;
	}

	/* Re-enables mouse */
	writeb(dat->regbase + REALVIEW_MOUSE_OFFSET_CR, REALVIEW_MOUSE_CR_EN | REALVIEW_MOUSE_CR_RXINTREN);
}

static void input_exit(struct input_t * input)
{
	struct resource_t * res = (struct resource_t *)input->priv;
	struct realview_mouse_data_t * dat = (struct realview_mouse_data_t *)res->data;

	if(!free_irq("KMI1"))
		LOG("Can't free irq 'KMI1'");
	writeb(dat->regbase + REALVIEW_MOUSE_OFFSET_CR, 0);
}

static int input_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static void input_suspend(struct input_t * input)
{
}

static void input_resume(struct input_t * input)
{
}

static bool_t realview_register_mouse(struct resource_t * res)
{
	struct input_t * input;
	char name[64];

	if(! clk_get_rate("kclk", 0))
	{
		LOG("Can't get clock source 'kclk'");
		return FALSE;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);
	input->name = name;
	input->type = INPUT_TYPE_MOUSE;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume	= input_resume,
	input->priv = res;
	data = input;

	if(register_input(input))
		return TRUE;

	free(input);
	return FALSE;
}

static bool_t realview_unregister_mouse(struct resource_t * res)
{
	struct input_t * input;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	input = search_input(name);
	if(!input)
		return FALSE;

	if(!unregister_input(input))
		return FALSE;

	free(input);
	return TRUE;
}

static __init void realview_mouse_device_init(void)
{
	resource_callback_with_name("input.mouse", realview_register_mouse);
}

static __exit void realview_mouse_device_exit(void)
{
	resource_callback_with_name("input.mouse", realview_unregister_mouse);
}

device_initcall(realview_mouse_device_init);
device_exitcall(realview_mouse_device_exit);
