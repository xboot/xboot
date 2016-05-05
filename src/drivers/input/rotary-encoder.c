/*
 * driver/input/rotary-encoder.c
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
#include <input/rotary-encoder.h>

struct rotary_encoder_pdata_t {
	int gpio_a;
	int gpio_b;
	int gpio_c;
	int inverted_a;
	int inverted_b;
	int inverted_c;
	int step;
	int irq_a;
	int irq_b;
	int irq_c;
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

static void rotary_encoder_report_event(struct rotary_encoder_pdata_t * pdat)
{
	printf("dir = %d\r\n", pdat->dir);
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
			rotary_encoder_report_event(pdat);
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
			rotary_encoder_report_event(pdat);
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
	rotary_encoder_report_event(pdat);
}

static void rotary_encoder_gpio_c_irq(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;
	int c = !!gpio_get_value(pdat->gpio_c);
	enum event_type_t type;

	if(pdat->inverted_c)
		type = c ? EVENT_TYPE_KEY_DOWN : EVENT_TYPE_KEY_UP;
	else
		type = c ? EVENT_TYPE_KEY_UP : EVENT_TYPE_KEY_DOWN;

	printf("type = %d\r\n", type);
}

static void input_init(struct input_t * input)
{
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;

	if(!pdat)
		return;

	gpio_set_pull(pdat->gpio_a, pdat->inverted_a ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	gpio_direction_input(pdat->gpio_a);

	gpio_set_pull(pdat->gpio_b, pdat->inverted_b ? GPIO_PULL_DOWN : GPIO_PULL_UP);
	gpio_direction_input(pdat->gpio_b);

	switch(pdat->step)
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
		break;
	}

	if(gpio_is_valid(pdat->gpio_c) && irq_is_valid(pdat->irq_c))
	{
		gpio_set_pull(pdat->gpio_c, pdat->inverted_c ? GPIO_PULL_DOWN : GPIO_PULL_UP);
		gpio_direction_input(pdat->gpio_c);
		request_irq(pdat->irq_c, rotary_encoder_gpio_c_irq, IRQ_TYPE_EDGE_BOTH, input);
	}
}

static void input_exit(struct input_t * input)
{
	struct rotary_encoder_pdata_t * pdat = (struct rotary_encoder_pdata_t *)input->priv;

	if(pdat)
	{
		free_irq(pdat->irq_a);
		free_irq(pdat->irq_b);

		if(gpio_is_valid(pdat->gpio_c) && irq_is_valid(pdat->irq_c))
			free_irq(pdat->irq_c);
	}
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

static bool_t rotary_encoder_register_keyboard(struct resource_t * res)
{
	struct rotary_encoder_data_t * rdat = (struct rotary_encoder_data_t *)res->data;
	struct rotary_encoder_pdata_t * pdat;
	struct input_t * input;
	char name[64];

	if(!gpio_is_valid(rdat->gpio_a) || !gpio_is_valid(rdat->gpio_b)
			|| !irq_is_valid(gpio_to_irq(rdat->gpio_a))
			|| !irq_is_valid(gpio_to_irq(rdat->gpio_b)))
		return FALSE;

	pdat = malloc(sizeof(struct rotary_encoder_pdata_t));
	if(!pdat)
		return FALSE;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->gpio_a = rdat->gpio_a;
	pdat->gpio_b = rdat->gpio_b;
	pdat->gpio_c = rdat->gpio_c;
	pdat->inverted_a = rdat->inverted_a ? 1 : 0;
	pdat->inverted_b = rdat->inverted_b ? 1 : 0;
	pdat->inverted_c = rdat->inverted_c ? 1 : 0;
	switch(rdat->step)
	{
	case 4:
	case 2:
	case 1:
		pdat->step = rdat->step;
		break;
	default:
		pdat->step = 1;
		break;
	}
	pdat->irq_a = gpio_to_irq(rdat->gpio_a);
	pdat->irq_b = gpio_to_irq(rdat->gpio_b);
	pdat->irq_c = gpio_to_irq(rdat->gpio_c);

	input->name = strdup(name);
	input->type = INPUT_TYPE_KEYBOARD;
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

static bool_t rotary_encoder_unregister_keyboard(struct resource_t * res)
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

static __init void rotary_encoder_device_init(void)
{
	resource_for_each("rotary-encoder", rotary_encoder_register_keyboard);
}

static __exit void rotary_encoder_device_exit(void)
{
	resource_for_each("rotary-encoder", rotary_encoder_unregister_keyboard);
}

device_initcall(rotary_encoder_device_init);
device_exitcall(rotary_encoder_device_exit);
