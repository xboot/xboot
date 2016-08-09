/*
 * driver/mouse-pl050.c
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
#include <input/input.h>

#define MOUSE_CR	 		(0x00)
#define MOUSE_STAT	 		(0x04)
#define MOUSE_DATA	 		(0x08)
#define MOUSE_CLKDIV 		(0x0c)
#define MOUSE_IIR	 		(0x10)

#define MOUSE_CR_TYPE	 	(1 << 5)
#define MOUSE_CR_RXINTREN	(1 << 4)
#define MOUSE_CR_TXINTREN	(1 << 3)
#define MOUSE_CR_EN			(1 << 2)
#define MOUSE_CR_FD			(1 << 1)
#define MOUSE_CR_FC			(1 << 0)

#define MOUSE_STAT_TXEMPTY	(1 << 6)
#define MOUSE_STAT_TXBUSY	(1 << 5)
#define MOUSE_STAT_RXFULL	(1 << 4)
#define MOUSE_STAT_RXBUSY	(1 << 3)
#define MOUSE_STAT_RXPARITY	(1 << 2)
#define MOUSE_STAT_IC		(1 << 1)
#define MOUSE_STAT_ID		(1 << 0)

#define MOUSE_IIR_TXINTR	(1 << 1)
#define MOUSE_IIR_RXINTR	(1 << 0)

struct mouse_pl050_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int irq;
	int xmax, ymax;
	int xpos, ypos;
	int totouch;
	u8_t packet[4];
	u8_t index;
	u8_t btnold;
};

static bool_t kmi_write(struct mouse_pl050_pdata_t * pdat, u8_t value)
{
	int timeout = 1000;

	while((read8(pdat->virt + MOUSE_STAT) & MOUSE_STAT_TXEMPTY) == 0 && timeout--);

	if(timeout)
	{
		write8(pdat->virt + MOUSE_DATA, value);

		while((read8(pdat->virt + MOUSE_STAT) & MOUSE_STAT_RXFULL) == 0);
		if(read8(pdat->virt + MOUSE_DATA) == 0xfa)
			return TRUE;
	}
	return FALSE;
}

static bool_t kmi_read(struct mouse_pl050_pdata_t * pdat, u8_t * value)
{
	if((read8(pdat->virt + MOUSE_STAT) & MOUSE_STAT_RXFULL))
	{
		*value = read8(pdat->virt + MOUSE_DATA);
		return TRUE;
	}
	return FALSE;
}

static void mouse_pl050_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct mouse_pl050_pdata_t * pdat = (struct mouse_pl050_pdata_t *)input->priv;
	int x, y, relx, rely, delta;
	u32_t btndown, btnup, btn;
	u8_t status;

	status = read8(pdat->virt + MOUSE_IIR);
	while(status & MOUSE_IIR_RXINTR)
	{
		pdat->packet[pdat->index] = read8(pdat->virt + MOUSE_DATA);
		pdat->index = (pdat->index + 1) & 0x3;

		if(pdat->index == 0)
		{
			btn = pdat->packet[0] & 0x7;
			btndown = (btn ^ pdat->btnold) & btn;
			btnup = (btn ^ pdat->btnold) & pdat->btnold;
			pdat->btnold = btn;

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
				if(pdat->xpos > pdat->xmax - 1)
					pdat->xpos = pdat->xmax - 1;
			}
			if(rely != 0)
			{
				pdat->ypos = pdat->ypos + rely;
				if(pdat->ypos < 0)
					pdat->ypos = 0;
				if(pdat->ypos > pdat->ymax - 1)
					pdat->ypos = pdat->ymax - 1;
			}
			x = pdat->xpos;
			y = pdat->ypos;

			if(pdat->totouch)
			{
				if((btn & (0x01 << 0)) && ((relx != 0) || (rely != 0)))
					push_event_touch_move(input, x, y, 0);

				if(btndown & (0x01 << 0))
					push_event_touch_begin(input, x, y, 0);

				if(btnup & (0x01 << 0))
					push_event_touch_end(input, x, y, 0);
			}
			else
			{
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
			}
		}

		status = read8(pdat->virt + MOUSE_IIR);
	}
}

static int mouse_pl050_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * mouse_pl050_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct mouse_pl050_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	u64_t clk;
	u8_t value;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = ((read8(virt + 0xfec) << 24) |
				(read8(virt + 0xfe8) << 16) |
				(read8(virt + 0xfe4) <<  8) |
				(read8(virt + 0xfe0) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x050)
		return NULL;

	if(!clk_search(dt_read_string(n, "clock", NULL)))
		return NULL;

	if(!irq_is_valid(dt_read_int(n, "interrupt", -1)))
		return NULL;

	pdat = malloc(sizeof(struct mouse_pl050_pdata_t));
	if(!pdat)
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(dt_read_string(n, "clock", NULL));
	pdat->irq = dt_read_int(n, "interrupt", -1);
	pdat->xmax = dt_read_int(n, "xmax", 640);
	pdat->ymax = dt_read_int(n, "ymax", 480);
	pdat->totouch = dt_read_bool(n, "to-touch-event", 0);
	pdat->xpos = pdat->xmax / 2;
	pdat->ypos = pdat->ymax / 2;
	pdat->packet[0] = 0;
	pdat->packet[1] = 0;
	pdat->packet[2] = 0;
	pdat->packet[3] = 0;
	pdat->index = 0;
	pdat->btnold = 0;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->type = INPUT_TYPE_KEYBOARD;
	input->ioctl = mouse_pl050_ioctl;
	input->priv = pdat;

	request_irq(pdat->irq, mouse_pl050_interrupt, IRQ_TYPE_NONE, input);
	clk_enable(pdat->clk);
	clk = clk_get_rate(pdat->clk);
	write8(pdat->virt + MOUSE_CLKDIV, (u32_t)(clk / 8000000) - 1);
	write8(pdat->virt + MOUSE_CR, MOUSE_CR_EN);
	kmi_write(pdat, 0xff);
	kmi_read(pdat, &value);
	kmi_write(pdat, 0xf3);
	kmi_write(pdat, 200);
	kmi_write(pdat, 0xf3);
	kmi_write(pdat, 100);
	kmi_write(pdat, 0xf3);
	kmi_write(pdat, 80);
	kmi_write(pdat, 0xf2);
	kmi_read(pdat, &value);
	kmi_read(pdat, &value);
	kmi_write(pdat, 0xf3);
	kmi_write(pdat, 100);
	kmi_write(pdat, 0xe8);
	kmi_write(pdat, 0x02);
	kmi_write(pdat, 0xe6);
	kmi_write(pdat, 0xf4);
	kmi_read(pdat, &value);
	kmi_read(pdat, &value);
	kmi_read(pdat, &value);
	kmi_read(pdat, &value);
	write8(pdat->virt + MOUSE_CR, MOUSE_CR_EN | MOUSE_CR_RXINTREN);

	if(!register_input(&dev, input))
	{
		write8(pdat->virt + MOUSE_CR, 0);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void mouse_pl050_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct mouse_pl050_pdata_t * pdat = (struct mouse_pl050_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		write8(pdat->virt + MOUSE_CR, 0);
		clk_disable(pdat->clk);
		free_irq(pdat->irq);
		free(pdat->clk);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void mouse_pl050_suspend(struct device_t * dev)
{
}

static void mouse_pl050_resume(struct device_t * dev)
{
}

struct driver_t mouse_pl050 = {
	.name		= "mouse-pl050",
	.probe		= mouse_pl050_probe,
	.remove		= mouse_pl050_remove,
	.suspend	= mouse_pl050_suspend,
	.resume		= mouse_pl050_resume,
};

static __init void mouse_pl050_driver_init(void)
{
	register_driver(&mouse_pl050);
}

static __exit void mouse_pl050_driver_exit(void)
{
	unregister_driver(&mouse_pl050);
}

driver_initcall(mouse_pl050_driver_init);
driver_exitcall(mouse_pl050_driver_exit);
