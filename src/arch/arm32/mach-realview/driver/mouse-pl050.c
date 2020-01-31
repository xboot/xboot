/*
 * driver/mouse-pl050.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <clk/clk.h>
#include <interrupt/interrupt.h>
#include <input/input.h>

enum {
	MOUSE_CR		= 0x00,
	MOUSE_STAT		= 0x04,
	MOUSE_DATA 		= 0x08,
	MOUSE_CLKDIV	= 0x0c,
	MOUSE_IIR		= 0x10,
};

struct mouse_pl050_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int irq;
	int xmax, ymax;
	int sensitivity;
	int xpos, ypos;
	int touchevent;
	unsigned char packet[4];
	int index;
	int obtn;
};

static bool_t kmi_write(struct mouse_pl050_pdata_t * pdat, u8_t value)
{
	int timeout = 1000;

	while((read8(pdat->virt + MOUSE_STAT) & (1 << 6)) == 0 && timeout--);

	if(timeout)
	{
		write8(pdat->virt + MOUSE_DATA, value);
		while((read8(pdat->virt + MOUSE_STAT) & (1 << 4)) == 0);

		if(read8(pdat->virt + MOUSE_DATA) == 0xfa)
			return TRUE;
	}
	return FALSE;
}

static bool_t kmi_read(struct mouse_pl050_pdata_t * pdat, u8_t * value)
{
	if((read8(pdat->virt + MOUSE_STAT) & (1 << 4)))
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
	int btndown, btnup, btn;
	int status;

	status = read8(pdat->virt + MOUSE_IIR);
	while(status & (1 << 0))
	{
		pdat->packet[pdat->index] = read8(pdat->virt + MOUSE_DATA);
		pdat->index = (pdat->index + 1) & 0x3;

		if(pdat->index == 0)
		{
			btn = pdat->packet[0] & 0x7;
			btndown = (btn ^ pdat->obtn) & btn;
			btnup = (btn ^ pdat->obtn) & pdat->obtn;
			pdat->obtn = btn;

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

			if(pdat->touchevent)
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

static int mouse_pl050_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	struct mouse_pl050_pdata_t * pdat = (struct mouse_pl050_pdata_t *)input->priv;
	int * p = arg;

	switch(shash(cmd))
	{
	case 0x431aa221: /* "mouse-set-range" */
		if(p)
		{
			pdat->xmax = p[0];
			pdat->ymax = p[1];
			return 0;
		}
		break;
	case 0xcd455615: /* "mouse-get-range" */
		if(p)
		{
			p[0] = pdat->xmax;
			p[1] = pdat->ymax;
			return 0;
		}
		break;
	case 0xe818d6df: /* "mouse-set-sensitivity" */
		if(p)
		{
			pdat->sensitivity = clamp(p[0], 1, 11);
			return 0;
		}
		break;
	case 0x40bfb1d3: /* "mouse-get-sensitivity" */
		if(p)
		{
			p[0] = pdat->sensitivity;
			return 0;
		}
		break;
	default:
		break;
	}
	return -1;
}

static struct device_t * mouse_pl050_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct mouse_pl050_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));
	u64_t rate;
	u8_t value;

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x050)
		return NULL;

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
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
	pdat->clk = strdup(clk);
	pdat->irq = irq;
	pdat->xmax = dt_read_int(n, "pointer-range-x", 640);
	pdat->ymax = dt_read_int(n, "pointer-range-y", 480);
	pdat->sensitivity = dt_read_int(n, "pointer-sensitivity", 6);
	pdat->touchevent = dt_read_bool(n, "simulate-touch-event", 0);
	pdat->xpos = pdat->xmax / 2;
	pdat->ypos = pdat->ymax / 2;
	pdat->packet[0] = 0;
	pdat->packet[1] = 0;
	pdat->packet[2] = 0;
	pdat->packet[3] = 0;
	pdat->index = 0;
	pdat->obtn = 0;

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->ioctl = mouse_pl050_ioctl;
	input->priv = pdat;

	request_irq(pdat->irq, mouse_pl050_interrupt, IRQ_TYPE_NONE, input);
	clk_enable(pdat->clk);
	rate = clk_get_rate(pdat->clk);
	write8(pdat->virt + MOUSE_CLKDIV, (u32_t)(rate / 8000000) - 1);
	write8(pdat->virt + MOUSE_CR, (1 << 2));
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
	write8(pdat->virt + MOUSE_CR, (1 << 2) | (1 << 4));

	if(!(dev = register_input(input, drv)))
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
	return dev;
}

static void mouse_pl050_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct mouse_pl050_pdata_t * pdat = (struct mouse_pl050_pdata_t *)input->priv;

	if(input)
	{
		unregister_input(input);
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

static struct driver_t mouse_pl050 = {
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
