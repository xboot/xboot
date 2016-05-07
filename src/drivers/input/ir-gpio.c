/*
 * driver/input/ir-gpio.c
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
#include <input/ir-decoder-nec.h>
#include <input/ir-gpio.h>

struct ir_gpio_pdata_t {
	int gpio;
	int active_low;
	int irq;
	ktime_t last;
	struct ir_nec_decoder_t nec;
};

static void input_irq(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ir_gpio_pdata_t * pdat = (struct ir_gpio_pdata_t *)input->priv;
	ktime_t now = ktime_get();
	int pulse, duration;
	uint32_t code;

	pulse = (gpio_get_value(pdat->gpio) != 0) ? 0 : 1;
	if(pdat->active_low)
		pulse = !pulse;
	duration = ktime_us_delta(now, pdat->last);
	pdat->last = now;

	if((code = ir_nec_decoder_handle(&pdat->nec, pulse, duration)) != 0)
	{
		push_event_key_down(input, code);
		push_event_key_up(input, code);
	}
}

static void input_init(struct input_t * input)
{
	struct ir_gpio_pdata_t * pdat = (struct ir_gpio_pdata_t *)input->priv;

	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP : GPIO_PULL_DOWN);
	gpio_direction_input(pdat->gpio);
	request_irq(pdat->irq, input_irq, IRQ_TYPE_EDGE_BOTH, input);
}

static void input_exit(struct input_t * input)
{
	struct ir_gpio_pdata_t * pdat = (struct ir_gpio_pdata_t *)input->priv;

	if(!pdat)
		return;
	free_irq(pdat->irq);
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

static bool_t register_ir_gpio(struct resource_t * res)
{
	struct ir_gpio_data_t * rdat = (struct ir_gpio_data_t *)res->data;
	struct ir_gpio_pdata_t * pdat;
	struct input_t * input;
	char name[64];

	if(!gpio_is_valid(rdat->gpio) || !irq_is_valid(gpio_to_irq(rdat->gpio)))
		return FALSE;

	pdat = malloc(sizeof(struct ir_gpio_pdata_t));
	if(!pdat)
		return FALSE;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->gpio = rdat->gpio;
	pdat->active_low = rdat->active_low;
	pdat->irq = gpio_to_irq(rdat->gpio);
	pdat->last = ktime_get();
	pdat->nec.state = IR_NEC_STATE_INACTIVE;

	input->name = strdup(name);
	input->type = INPUT_TYPE_KEYBOARD;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume	= input_resume,
	input->priv = pdat;

	if(register_input(input))
		return TRUE;

	free(input->priv);
	free(input->name);
	free(input);
	return FALSE;
}

static bool_t unregister_ir_gpio(struct resource_t * res)
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

static __init void ir_gpio_device_init(void)
{
	resource_for_each("ir-gpio", register_ir_gpio);
}

static __exit void ir_gpio_device_exit(void)
{
	resource_for_each("ir-gpio", unregister_ir_gpio);
}

device_initcall(ir_gpio_device_init);
device_exitcall(ir_gpio_device_exit);
