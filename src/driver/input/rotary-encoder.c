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
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>
#include <input/keyboard.h>

struct rotary_encoder_pdata_t {
	int a;
	int acfg;
	int b;
	int bcfg;
	int c;
	int ccfg;
	int irqa;
	int irqb;
	int irqc;
	int inva;
	int invb;
	int invc;
	int state;
	int dir;
};

static int rotary_encoder_get_state(struct rotary_encoder_pdata_t * pdat)
{
	int a = !!gpio_get_value(pdat->a);
	int b = !!gpio_get_value(pdat->b);

	a ^= pdat->inva;
	b ^= pdat->invb;

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

static void rotary_encoder_c_irq(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;
	int c1, c2, c3;

	do {
		c1 = !!gpio_get_value(pdat->c);
		mdelay(1);
		c2 = !!gpio_get_value(pdat->c);
		mdelay(1);
		c3 = !!gpio_get_value(pdat->c);
	} while((c1 != c2) || (c1 != c3));

	push_event_rotary_switch(input, (c1 ^ pdat->invc) ? 0 : 1);
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
	int a = dt_read_int(n, "a-gpio", -1);
	int b = dt_read_int(n, "b-gpio", -1);

	if(!gpio_is_valid(a) || !gpio_is_valid(b)
			|| !irq_is_valid(gpio_to_irq(a))
			|| !irq_is_valid(gpio_to_irq(b)))
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

	pdat->a = a;
	pdat->acfg = dt_read_int(n, "a-gpio-config", -1);
	pdat->b = b;
	pdat->bcfg = dt_read_int(n, "b-gpio-config", -1);
	pdat->c = dt_read_int(n, "c-gpio", -1);
	pdat->ccfg = dt_read_int(n, "c-gpio-config", -1);
	pdat->irqa = gpio_to_irq(pdat->a);
	pdat->irqb = gpio_to_irq(pdat->b);
	pdat->irqc = gpio_to_irq(pdat->c);
	pdat->inva = dt_read_bool(n, "a-inverted", 0);
	pdat->invb = dt_read_bool(n, "b-inverted", 0);
	pdat->invc = dt_read_bool(n, "c-inverted", 0);

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = INPUT_TYPE_ROTARY;
	input->ioctl = rotary_encoder_ioctl;
	input->priv = pdat;

	if(pdat->acfg >= 0)
		gpio_set_cfg(pdat->a, pdat->acfg);
	gpio_set_pull(pdat->a, pdat->inva ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	gpio_direction_input(pdat->a);

	if(pdat->bcfg >= 0)
		gpio_set_cfg(pdat->b, pdat->bcfg);
	gpio_set_pull(pdat->b, pdat->invb ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	gpio_direction_input(pdat->b);

	switch(dt_read_int(n, "step-per-period", 1))
	{
	case 4:
		request_irq(pdat->irqa, rotary_encoder_quarter_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irqb, rotary_encoder_quarter_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = rotary_encoder_get_state(pdat);
		break;

	case 2:
		request_irq(pdat->irqa, rotary_encoder_half_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irqb, rotary_encoder_half_period_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = rotary_encoder_get_state(pdat);
		break;

	case 1:
		request_irq(pdat->irqa, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irqb, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = 0;
		break;

	default:
		request_irq(pdat->irqa, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		request_irq(pdat->irqb, rotary_encoder_irq, IRQ_TYPE_EDGE_BOTH, input);
		pdat->state = 0;
		break;
	}

	if(gpio_is_valid(pdat->c) && irq_is_valid(pdat->irqc))
	{
		if(pdat->ccfg >= 0)
			gpio_set_cfg(pdat->c, pdat->ccfg);
		gpio_set_pull(pdat->c, pdat->invc ? GPIO_PULL_DOWN : GPIO_PULL_UP);
		gpio_direction_input(pdat->c);
		request_irq(pdat->irqc, rotary_encoder_c_irq, IRQ_TYPE_EDGE_BOTH, input);
	}

	if(!register_input(&dev, input))
	{
		free_irq(pdat->irqa);
		free_irq(pdat->irqb);
		if(gpio_is_valid(pdat->c) && irq_is_valid(pdat->irqc))
			free_irq(pdat->irqc);

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
		free_irq(pdat->irqa);
		free_irq(pdat->irqb);
		if(gpio_is_valid(pdat->c) && irq_is_valid(pdat->irqc))
			free_irq(pdat->irqc);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void rotary_encoder_suspend(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;

	disable_irq(pdat->irqa);
	disable_irq(pdat->irqb);
	if(gpio_is_valid(pdat->c) && irq_is_valid(pdat->irqc))
		disable_irq(pdat->irqc);
}

static void rotary_encoder_resume(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;

	enable_irq(pdat->irqa);
	enable_irq(pdat->irqb);
	if(gpio_is_valid(pdat->c) && irq_is_valid(pdat->irqc))
		enable_irq(pdat->irqc);
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
