/*
 * driver/input/key-gpio.c
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
#include <input/key-gpio.h>

struct key_gpio_pdata_t {
	int * state;
	struct timer_t timer;
	struct key_gpio_data_t * rdat;
};

static int key_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct key_gpio_pdata_t * pdat = (struct key_gpio_pdata_t *)input->priv;
	struct key_gpio_data_t * rdat = (struct key_gpio_data_t *)pdat->rdat;
	enum event_type_t type;
	int i, val;

	for(i = 0; i < rdat->nbutton; i++)
	{
		val = gpio_get_value(rdat->buttons[i].gpio);
		if(val != pdat->state[i])
		{
			if(rdat->buttons[i].active_low)
				type = val ? EVENT_TYPE_KEY_UP : EVENT_TYPE_KEY_DOWN;
			else
				type = val ? EVENT_TYPE_KEY_DOWN : EVENT_TYPE_KEY_UP;

			if(type == EVENT_TYPE_KEY_DOWN)
				push_event_key_down(input, rdat->buttons[i].key);
			else if(type == EVENT_TYPE_KEY_UP)
				push_event_key_up(input, rdat->buttons[i].key);
		}
		pdat->state[i] = val;
	}

	timer_forward_now(timer, ms_to_ktime(100));
	return 1;
}

static void input_init(struct input_t * input)
{
	struct key_gpio_pdata_t * pdat = (struct key_gpio_pdata_t *)input->priv;
	struct key_gpio_data_t * rdat = (struct key_gpio_data_t *)pdat->rdat;
	int i;

	if(!pdat)
		return;

	pdat->state = malloc(rdat->nbutton * sizeof(int));
	if(!pdat->state)
		return;

	for(i = 0; i < rdat->nbutton; i++)
	{
		gpio_set_pull(rdat->buttons[i].gpio, rdat->buttons[i].active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
		gpio_direction_input(rdat->buttons[i].gpio);
		pdat->state[i] = gpio_get_value(rdat->buttons[i].gpio);
	}

	timer_init(&pdat->timer, key_gpio_timer_function, input);
	timer_start_now(&pdat->timer, ms_to_ktime(100));
}

static void input_exit(struct input_t * input)
{
	struct key_gpio_pdata_t * pdat = (struct key_gpio_pdata_t *)input->priv;

	if(!pdat)
		return;

	if(!pdat->state)
		return;

	free(pdat->state);
	timer_cancel(&pdat->timer);
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

static bool_t gpio_register_keyboard(struct resource_t * res)
{
	struct key_gpio_data_t * rdat = (struct key_gpio_data_t *)res->data;
	struct key_gpio_pdata_t * pdat;
	struct input_t * input;
	char name[64];

	if(!rdat->buttons)
		return FALSE;

	if(rdat->nbutton <= 0)
		return FALSE;

	pdat = malloc(sizeof(struct key_gpio_pdata_t));
	if(!pdat)
		return FALSE;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->state = NULL;
	pdat->rdat = rdat;

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

static bool_t gpio_unregister_keyboard(struct resource_t * res)
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

static __init void key_gpio_device_init(void)
{
	resource_for_each_with_name("key-gpio", gpio_register_keyboard);
}

static __exit void key_gpio_device_exit(void)
{
	resource_for_each_with_name("key-gpio", gpio_unregister_keyboard);
}

device_initcall(key_gpio_device_init);
device_exitcall(key_gpio_device_exit);
