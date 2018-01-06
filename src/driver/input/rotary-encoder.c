/*
 * driver/input/rotary-encoder.c
 *                   _____       _____       _____
 *                  |     |     |     |     |     |
 *   Channel A  ____|     |_____|     |_____|     |____
 *
 *                  :  :  :  :  :  :  :  :  :  :  :  :
 *             __       _____       _____       _____
 *               |     |     |     |     |     |     |
 *   Channel B   |_____|     |_____|     |_____|     |__
 *
 *                  :  :  :  :  :  :  :  :  :  :  :  :
 *   Event          a  b  c  d  a  b  c  d  a  b  c  d
 *
 *                 |<-------->|
 * 	          one step
 *
 *                 |<-->|
 * 	          one step (half-period mode)
 *
 *                 |<>|
 * 	          one step (quarter-period mode)
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <input/keyboard.h>

struct rotary_encoder_pdata_t {
	int gpio_a;
	int gpio_b;
	int gpio_c;
	int irq_a;
	int irq_b;
	int irq_c;
	int inverted_a;
	int inverted_b;
	int inverted_c;
	int state;
	int dir;
};

static int rotary_encoder_get_state(struct rotary_encoder_pdata_t * pdat)
{
	int a = !!gpio_get_value(pdat->gpio_a);
	int b = !!gpio_get_value(pdat->gpio_b);

	a ^= pdat->inverted_a;
	b ^= pdat->inverted_b;

	return ((a << 1) | b);
}

static void rotary_encoder_irq(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;
	int state = rotary_encoder_get_state(pdat);

	switch(state)
	{
	case 0x0:
		if(pdat->state)
		{
			push_event_rotary_turn(input, pdat->dir ? 1 : -1);
			pdat->state = 0;
		}
		break;

	case 0x1:
	case 0x2:
		if(pdat->state)
			pdat->dir = state - 1;
		break;

	case 0x3:
		pdat->state = 1;
		break;

	default:
		break;
	}
}

static void rotary_encoder_half_period_irq(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;
	int state = rotary_encoder_get_state(pdat);

	switch(state)
	{
	case 0x00:
	case 0x03:
		if(state != pdat->state)
		{
			push_event_rotary_turn(input, pdat->dir ? 1 : -1);
			pdat->state = state;
		}
		break;

	case 0x01:
	case 0x02:
		pdat->dir = (pdat->state + state) & 0x01;
		break;

	default:
		break;
	}
}

static void rotary_encoder_quarter_period_irq(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;
	int state = rotary_encoder_get_state(pdat);
	int sum = ((pdat->state << 4) + state) & 0xff;

	pdat->state = state;
	switch(sum)
	{
	case 0x31:
	case 0x10:
	case 0x02:
	case 0x23:
		pdat->dir = 0;
		break;

	case 0x13:
	case 0x01:
	case 0x20:
	case 0x32:
		pdat->dir = 1;
		break;

	default:
		return;
	}
	push_event_rotary_turn(input, pdat->dir ? 1 : -1);
}

static void rotary_encoder_gpio_c_irq(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;
	int c1, c2, c3;

	do {
		c1 = !!gpio_get_value(pdat->gpio_c);
		mdelay(1);
		c2 = !!gpio_get_value(pdat->gpio_c);
		mdelay(1);
		c3 = !!gpio_get_value(pdat->gpio_c);
	} while((c1 != c2) || (c1 != c3));

	push_event_rotary_switch(input, (c1 ^ pdat->inverted_c) ? 0 : 1);
}

static int rotary_encoder_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * rotary_encoder_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rotary_encoder_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	int gpio_a, gpio_b;

	gpio_a = dt_read_int(n, "gpio-a", -1);
	gpio_b = dt_read_int(n, "gpio-b", -1);

	if(!gpio_is_valid(gpio_a) || !gpio_is_valid(gpio_b)
			|| !irq_is_valid(gpio_to_irq(gpio_a))
			|| !irq_is_valid(gpio_to_irq(gpio_b)))
		return NULL;

	pdat = malloc(sizeof(struct rotary_encoder_pdata_t));
	if(!pdat)
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return NULL;
	}

	pdat->gpio_a = gpio_a;
	pdat->gpio_b = gpio_b;
	pdat->gpio_c = dt_read_int(n, "gpio-c", -1);
	pdat->irq_a = gpio_to_irq(pdat->gpio_a);
	pdat->irq_b = gpio_to_irq(pdat->gpio_b);
	pdat->irq_c = gpio_to_irq(pdat->gpio_c);
	pdat->inverted_a = dt_read_bool(n, "inverted-a", 0);
	pdat->inverted_b = dt_read_bool(n, "inverted-b", 0);
	pdat->inverted_c = dt_read_bool(n, "inverted-c", 0);

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = INPUT_TYPE_ROTARY;
	input->ioctl = rotary_encoder_ioctl;
	input->priv = pdat;

	gpio_set_pull(pdat->gpio_a, pdat->inverted_a ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	gpio_direction_input(pdat->gpio_a);
	gpio_set_pull(pdat->gpio_b, pdat->inverted_b ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	gpio_direction_input(pdat->gpio_b);

	switch(dt_read_int(n, "step-per-period", 1))
	{
	case 4:
		request_irq(pdat->irq_a, rotary_encoder_quarter_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irq_b, rotary_encoder_quarter_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = rotary_encoder_get_state(pdat);
		break;

	case 2:
		request_irq(pdat->irq_a, rotary_encoder_half_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irq_b, rotary_encoder_half_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = rotary_encoder_get_state(pdat);
		break;

	case 1:
		request_irq(pdat->irq_a, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irq_b, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = 0;
		break;

	default:
		request_irq(pdat->irq_a, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irq_b, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = 0;
		break;
	}

	if(gpio_is_valid(pdat->gpio_c) && irq_is_valid(pdat->irq_c))
	{
		gpio_set_pull(pdat->gpio_c, pdat->inverted_c ? GPIO_PULL_DOWN : GPIO_PULL_UP);
		gpio_direction_input(pdat->gpio_c);
		request_irq(pdat->irq_c, rotary_encoder_gpio_c_irq, IRQ_TYPE_EDGE_BOTH, input);
	}

	if(!register_input(&dev, input))
	{
		free_irq(pdat->irq_a);
		free_irq(pdat->irq_b);
		if(gpio_is_valid(pdat->gpio_c) && irq_is_valid(pdat->irq_c))
			free_irq(pdat->irq_c);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void rotary_encoder_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		free_irq(pdat->irq_a);
		free_irq(pdat->irq_b);
		if(gpio_is_valid(pdat->gpio_c) && irq_is_valid(pdat->irq_c))
			free_irq(pdat->irq_c);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void rotary_encoder_suspend(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;

	disable_irq(pdat->irq_a);
	disable_irq(pdat->irq_b);
	if(gpio_is_valid(pdat->gpio_c) && irq_is_valid(pdat->irq_c))
		disable_irq(pdat->irq_c);
}

static void rotary_encoder_resume(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;

	enable_irq(pdat->irq_a);
	enable_irq(pdat->irq_b);
	if(gpio_is_valid(pdat->gpio_c) && irq_is_valid(pdat->irq_c))
		enable_irq(pdat->irq_c);
}

static struct driver_t rotary_encoder = {
	.name		= "rotary-encoder",
	.probe		= rotary_encoder_probe,
	.remove		= rotary_encoder_remove,
	.suspend	= rotary_encoder_suspend,
	.resume		= rotary_encoder_resume,
};

static __init void rotary_encoder_driver_init(void)
{
	register_driver(&rotary_encoder);
}

static __exit void rotary_encoder_driver_exit(void)
{
	unregister_driver(&rotary_encoder);
}

driver_initcall(rotary_encoder_driver_init);
driver_exitcall(rotary_encoder_driver_exit);
