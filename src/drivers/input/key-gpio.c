/*
 * driver/input/key-gpio.c
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
#include <input/key-gpio.h>

struct key_gpio_private_data_t {
	int * state;
	struct timer_t timer;
	struct key_gpio_data_t * rdat;
};

static void key_gpio_timer_function(u32_t data)
{
	struct input_t * input = (struct input_t *)(data);
	struct key_gpio_private_data_t * dat = (struct key_gpio_private_data_t *)input->priv;
	struct key_gpio_data_t * rdat = (struct key_gpio_data_t *)dat->rdat;
	struct event_t event;
	int i, val;

	for(i = 0; i < rdat->nbutton; i++)
	{
		val = gpio_get_value(rdat->buttons[i].gpio);
		if(val != dat->state[i])
		{
			event.device = input;
			if(rdat->buttons[i].active_low)
				event.type = val ? EVENT_TYPE_KEY_UP : EVENT_TYPE_KEY_DOWN;
			else
				event.type = val ? EVENT_TYPE_KEY_DOWN : EVENT_TYPE_KEY_UP;
			event.e.key.code = rdat->buttons[i].code;
			push_event(&event);
		}
		dat->state[i] = val;
	}

	mod_timer(&(dat->timer), jiffies + msecs_to_jiffies(100));
}

static void input_init(struct input_t * input)
{
	struct key_gpio_private_data_t * dat = (struct key_gpio_private_data_t *)input->priv;
	struct key_gpio_data_t * rdat = (struct key_gpio_data_t *)dat->rdat;
	int i;

	if(!dat)
		return;

	dat->state = malloc(rdat->nbutton * sizeof(int));
	if(!dat->state)
		return;

	for(i = 0; i < rdat->nbutton; i++)
	{
		gpio_set_pull(rdat->buttons[i].gpio, rdat->buttons[i].active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
		gpio_direction_input(rdat->buttons[i].gpio);
		dat->state[i] = gpio_get_value(rdat->buttons[i].gpio);
	}

	setup_timer(&dat->timer, key_gpio_timer_function, (u32_t)input);
	mod_timer(&(dat->timer), jiffies + msecs_to_jiffies(100));
}

static void input_exit(struct input_t * input)
{
	struct key_gpio_private_data_t * dat = (struct key_gpio_private_data_t *)input->priv;

	if(!dat)
		return;

	if(!dat->state)
		return;

	free(dat->state);
	del_timer(&(dat->timer));
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
	struct key_gpio_private_data_t * dat;
	struct input_t * input;
	char name[64];

	if(!rdat->buttons)
		return FALSE;

	if(rdat->nbutton <= 0)
		return FALSE;

	dat = malloc(sizeof(struct key_gpio_private_data_t));
	if(!dat)
		return FALSE;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->state = NULL;
	dat->rdat = rdat;

	input->name = strdup(name);
	input->type = INPUT_TYPE_KEYBOARD;
	input->init = input_init;
	input->exit = input_exit;
	input->ioctl = input_ioctl;
	input->suspend = input_suspend,
	input->resume	= input_resume,
	input->priv = dat;

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
