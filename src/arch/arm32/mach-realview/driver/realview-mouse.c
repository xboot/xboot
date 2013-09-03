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

#define MOUSE_TO_TOUCHES_EVENT

struct realview_mouse_private_data_t {
	u8_t packet[4];
	u8_t index;
	u8_t btn_old;

	s32_t xpos, ypos;
	s32_t width, height;

	struct realview_mouse_data_t * rdat;
};

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

static void mouse_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct realview_mouse_private_data_t * dat = (struct realview_mouse_private_data_t *)input->priv;
	struct realview_mouse_data_t * rdat = (struct realview_mouse_data_t *)dat->rdat;
	s32_t x, y, relx, rely, delta;
	u32_t btndown, btnup, btn;
	u8_t status;

	status = readb(rdat->regbase + REALVIEW_MOUSE_OFFSET_IIR);
	while(status & REALVIEW_MOUSE_IIR_RXINTR)
	{
		dat->packet[dat->index] = readb(rdat->regbase + REALVIEW_MOUSE_OFFSET_DATA);
		dat->index = (dat->index + 1) & 0x3;

		if(dat->index == 0)
		{
			btn = dat->packet[0] & 0x7;
			btndown = (btn ^ dat->btn_old) & btn;
			btnup = (btn ^ dat->btn_old) & dat->btn_old;
			dat->btn_old = btn;

			if(dat->packet[0] & 0x10)
				relx = 0xffffff00 | dat->packet[1];
			else
				relx = dat->packet[1];

			if(dat->packet[0] & 0x20)
				rely = 0xffffff00 | dat->packet[2];
			else
				rely = dat->packet[2];
			rely = -rely;

			delta = dat->packet[3] & 0xf;
			if(delta == 0xf)
				delta = -1;

			if(relx != 0)
			{
				dat->xpos = dat->xpos + relx;
				if(dat->xpos < 0)
					dat->xpos = 0;
				if(dat->xpos > dat->width - 1)
					dat->xpos = dat->width - 1;
			}
			if(rely != 0)
			{
				dat->ypos = dat->ypos + rely;
				if(dat->ypos < 0)
					dat->ypos = 0;
				if(dat->ypos > dat->height - 1)
					dat->ypos = dat->height - 1;
			}
			x = dat->xpos;
			y = dat->ypos;

#ifdef MOUSE_TO_TOUCHES_EVENT
			if((btn & MOUSE_BUTTON_LEFT) && ((relx != 0) || (rely != 0)))
				push_event_touches_move(input, x, y, 1);

			if(btndown & MOUSE_BUTTON_LEFT)
				push_event_touches_begin(input, x, y, 1);

			if(btnup & MOUSE_BUTTON_LEFT)
				push_event_touches_end(input, x, y, 1);
#else
			if((relx != 0) || (rely != 0))
				push_event_mouse_move(input, x, y);

			if(delta != 0)
				push_event_mouse_wheel(input, x, y, delta);

			if(btndown)
				push_event_mouse_button_down(input, x, y, btndown);

			if(btnup)
				push_event_mouse_button_up(input, x, y, btnup);
#endif
		}

		status = readb(rdat->regbase + REALVIEW_MOUSE_OFFSET_IIR);
	}
}

static void input_init(struct input_t * input)
{
	struct realview_mouse_private_data_t * dat = (struct realview_mouse_private_data_t *)input->priv;
	struct realview_mouse_data_t * rdat = (struct realview_mouse_data_t *)dat->rdat;
	u32_t divisor;
	u64_t kclk;
	u8_t value;

	if(! clk_get_rate("kclk", &kclk))
		return;

	/* Set mouse's clock divisor */
	divisor = (u32_t)(kclk /8000000) - 1;
	writeb(rdat->regbase + REALVIEW_MOUSE_OFFSET_CLKDIV, divisor);

	/* Enable mouse controller */
	writeb(rdat->regbase + REALVIEW_MOUSE_OFFSET_CR, REALVIEW_MOUSE_CR_EN);

	/* Reset mouse, and wait ack and pass/fail code */
	if(! kmi_write(rdat, 0xff) )
		return;
	if(! kmi_read(rdat, &value))
		return;
	if(value != 0xaa)
		return;

	/* Enable scroll wheel */
	kmi_write(rdat, 0xf3);
	kmi_write(rdat, 200);

	kmi_write(rdat, 0xf3);
	kmi_write(rdat, 100);

	kmi_write(rdat, 0xf3);
	kmi_write(rdat, 80);

	kmi_write(rdat, 0xf2);
	kmi_read(rdat, &value);
	kmi_read(rdat, &value);

	/* Set sample rate, 100 samples/sec */
	kmi_write(rdat, 0xf3);
	kmi_write(rdat, 100);

	/* Set resolution, 4 counts per mm, 1:1 scaling */
	kmi_write(rdat, 0xe8);
	kmi_write(rdat, 0x02);
	kmi_write(rdat, 0xe6);

	/* Enable data reporting */
	kmi_write(rdat, 0xf4);

	/* Clear a receive buffer */
	kmi_read(rdat, &value);
	kmi_read(rdat, &value);
	kmi_read(rdat, &value);
	kmi_read(rdat, &value);

	if(!request_irq("KMI1", mouse_interrupt, input))
	{
		LOG("Can't request irq 'KMI1'");
		writeb(rdat->regbase + REALVIEW_MOUSE_OFFSET_CR, 0);
		return;
	}

	/* Re-enables mouse */
	writeb(rdat->regbase + REALVIEW_MOUSE_OFFSET_CR, REALVIEW_MOUSE_CR_EN | REALVIEW_MOUSE_CR_RXINTREN);
}

static void input_exit(struct input_t * input)
{
	struct realview_mouse_private_data_t * dat = (struct realview_mouse_private_data_t *)input->priv;
	struct realview_mouse_data_t * rdat = (struct realview_mouse_data_t *)dat->rdat;

	if(!free_irq("KMI1"))
		LOG("Can't free irq 'KMI1'");
	writeb(rdat->regbase + REALVIEW_MOUSE_OFFSET_CR, 0);
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
	struct realview_mouse_data_t * rdat = (struct realview_mouse_data_t *)res->data;
	struct realview_mouse_private_data_t * dat;
	struct input_t * input;
	char name[64];

	if(! clk_get_rate("kclk", 0))
	{
		LOG("Can't get clock source 'kclk'");
		return FALSE;
	}

	dat = malloc(sizeof(struct realview_mouse_private_data_t));
	if(!dat)
		return FALSE;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->packet[0] = 0;
	dat->packet[1] = 0;
	dat->packet[2] = 0;
	dat->packet[3] = 0;
	dat->index = 0;
	dat->btn_old = 0;
	dat->xpos = 0;
	dat->ypos = 0;
	dat->width = rdat->width;
	dat->height = rdat->height;
	dat->rdat = rdat;

	input->name = strdup(name);
	input->type = INPUT_TYPE_MOUSE;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume = input_resume,
	input->priv = dat;

	if(register_input(input))
		return TRUE;

	free(input->priv);
	free(input->name);
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

	free(input->priv);
	free(input->name);
	free(input);
	return TRUE;
}

static __init void realview_mouse_device_init(void)
{
	resource_for_each_with_name("mouse-pl050", realview_register_mouse);
}

static __exit void realview_mouse_device_exit(void)
{
	resource_for_each_with_name("mouse-pl050", realview_unregister_mouse);
}

device_initcall(realview_mouse_device_init);
device_exitcall(realview_mouse_device_exit);
