/*
 * driver/realview_mouse.c
 *
 * realview mouse drivers, the primecell pl050 ps2 controller.
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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

#define MOUSE_TO_TOUCH_EVENT

struct realview_mouse_pdata_t {
	u8_t packet[4];
	u8_t index;
	u8_t btn_old;

	s32_t xpos, ypos;
	s32_t width, height;

	struct realview_mouse_data_t * rdat;
};

static bool_t kmi_write(struct realview_mouse_data_t * pdat, u8_t value)
{
	s32_t timeout = 1000;

	while((read8(phys_to_virt(pdat->regbase + MOUSE_STAT)) & MOUSE_STAT_TXEMPTY) == 0 && timeout--);

	if(timeout)
	{
		write8(phys_to_virt(pdat->regbase + MOUSE_DATA), value);

		while((read8(phys_to_virt(pdat->regbase + MOUSE_STAT)) & MOUSE_STAT_RXFULL) == 0);

		if( read8(phys_to_virt(pdat->regbase + MOUSE_DATA)) == 0xfa)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

static bool_t kmi_read(struct realview_mouse_data_t * pdat, u8_t * value)
{
	if( (read8(phys_to_virt(pdat->regbase + MOUSE_STAT)) & MOUSE_STAT_RXFULL) )
	{
		*value = read8(phys_to_virt(pdat->regbase + MOUSE_DATA));
		return TRUE;
	}

	return FALSE;
}

static void mouse_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct realview_mouse_pdata_t * pdat = (struct realview_mouse_pdata_t *)input->priv;
	struct realview_mouse_data_t * rdat = (struct realview_mouse_data_t *)pdat->rdat;
	s32_t x, y, relx, rely, delta;
	u32_t btndown, btnup, btn;
	u8_t status;

	status = read8(phys_to_virt(rdat->regbase + MOUSE_IIR));
	while(status & MOUSE_IIR_RXINTR)
	{
		pdat->packet[pdat->index] = read8(phys_to_virt(rdat->regbase + MOUSE_DATA));
		pdat->index = (pdat->index + 1) & 0x3;

		if(pdat->index == 0)
		{
			btn = pdat->packet[0] & 0x7;
			btndown = (btn ^ pdat->btn_old) & btn;
			btnup = (btn ^ pdat->btn_old) & pdat->btn_old;
			pdat->btn_old = btn;

			if(pdat->packet[0] & 0x10)
				relx = 0xffffff00 | pdat->packet[1];
			else
				relx = pdat->packet[1];

			if(pdat->packet[0] & 0x20)
				rely = 0xffffff00 | pdat->packet[2];
			else
				rely = pdat->packet[2];
			rely = -rely;

			delta = pdat->packet[3] & 0xf;
			if(delta == 0xf)
				delta = -1;

			if(relx != 0)
			{
				pdat->xpos = pdat->xpos + relx;
				if(pdat->xpos < 0)
					pdat->xpos = 0;
				if(pdat->xpos > pdat->width - 1)
					pdat->xpos = pdat->width - 1;
			}
			if(rely != 0)
			{
				pdat->ypos = pdat->ypos + rely;
				if(pdat->ypos < 0)
					pdat->ypos = 0;
				if(pdat->ypos > pdat->height - 1)
					pdat->ypos = pdat->height - 1;
			}
			x = pdat->xpos;
			y = pdat->ypos;

#ifdef MOUSE_TO_TOUCH_EVENT
			if((btn & (0x01 << 0)) && ((relx != 0) || (rely != 0)))
				push_event_touch_move(input, x, y, 0);

			if(btndown & (0x01 << 0))
				push_event_touch_begin(input, x, y, 0);

			if(btnup & (0x01 << 0))
				push_event_touch_end(input, x, y, 0);
#else
			if((relx != 0) || (rely != 0))
				push_event_mouse_move(input, x, y);

			if(delta != 0)
				push_event_mouse_wheel(input, 0, delta);

			if(btndown)
			{
				if(btndown & (0x01 << 0))
					push_event_mouse_button_down(input, x, y, MOUSE_BUTTON_LEFT);
				else if(btndown & (0x01 << 1))
					push_event_mouse_button_down(input, x, y, MOUSE_BUTTON_RIGHT);
				else if(btndown & (0x01 << 2))
					push_event_mouse_button_down(input, x, y, MOUSE_BUTTON_MIDDLE);
			}

			if(btnup)
			{
				if(btnup & (0x01 << 0))
					push_event_mouse_button_up(input, x, y, MOUSE_BUTTON_LEFT);
				else if(btnup & (0x01 << 1))
					push_event_mouse_button_up(input, x, y, MOUSE_BUTTON_RIGHT);
				else if(btnup & (0x01 << 2))
					push_event_mouse_button_up(input, x, y, MOUSE_BUTTON_MIDDLE);
			}
#endif
		}

		status = read8(phys_to_virt(rdat->regbase + MOUSE_IIR));
	}
}

static void input_init(struct input_t * input)
{
	struct realview_mouse_pdata_t * pdat = (struct realview_mouse_pdata_t *)input->priv;
	struct realview_mouse_data_t * rdat = (struct realview_mouse_data_t *)pdat->rdat;
	u32_t divisor;
	u64_t kclk;
	u8_t value;

	clk_enable("kclk");
	kclk = clk_get_rate("kclk");
	if(!kclk)
		return;

	/* Set mouse's clock divisor */
	divisor = (u32_t)(kclk /8000000) - 1;
	write8(phys_to_virt(rdat->regbase + MOUSE_CLKDIV), divisor);

	/* Enable mouse controller */
	write8(phys_to_virt(rdat->regbase + MOUSE_CR), MOUSE_CR_EN);

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

	if(!request_irq(REALVIEW_IRQ_KMI1, mouse_interrupt, IRQ_TYPE_NONE, input))
	{
		write8(phys_to_virt(rdat->regbase + MOUSE_CR), 0);
		return;
	}

	/* Re-enables mouse */
	write8(phys_to_virt(rdat->regbase + MOUSE_CR), MOUSE_CR_EN | MOUSE_CR_RXINTREN);
}

static void input_exit(struct input_t * input)
{
	struct realview_mouse_pdata_t * pdat = (struct realview_mouse_pdata_t *)input->priv;
	struct realview_mouse_data_t * rdat = (struct realview_mouse_data_t *)pdat->rdat;

	clk_disable("kclk");
	free_irq(REALVIEW_IRQ_KMI1);
	write8(phys_to_virt(rdat->regbase + MOUSE_CR), 0);
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
	struct realview_mouse_pdata_t * pdat;
	struct input_t * input;
	char name[64];

	pdat = malloc(sizeof(struct realview_mouse_pdata_t));
	if(!pdat)
		return FALSE;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->packet[0] = 0;
	pdat->packet[1] = 0;
	pdat->packet[2] = 0;
	pdat->packet[3] = 0;
	pdat->index = 0;
	pdat->btn_old = 0;
	pdat->xpos = 0;
	pdat->ypos = 0;
	pdat->width = rdat->width;
	pdat->height = rdat->height;
	pdat->rdat = rdat;

	input->name = strdup(name);
	input->type = INPUT_TYPE_MOUSE;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume = input_resume,
	input->priv = pdat;

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
	resource_for_each_with_name("realview-mouse", realview_register_mouse);
}

static __exit void realview_mouse_device_exit(void)
{
	resource_for_each_with_name("realview-mouse", realview_unregister_mouse);
}

device_initcall(realview_mouse_device_init);
device_exitcall(realview_mouse_device_exit);
