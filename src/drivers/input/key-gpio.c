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
#include <input/input.h>
#include <input/keyboard.h>

struct gpio_key_t {
	int gpio;
	int active_low;
	int keycode;
	int state;
};

struct key_gpio_pdata_t {
	struct timer_t timer;
	struct gpio_key_t * keys;
	int nkeys;
	int interval;
};

static int key_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct key_gpio_pdata_t * pdat = (struct key_gpio_pdata_t *)input->priv;
	enum event_type_t type;
	int i, val;

	for(i = 0; i < pdat->nkeys; i++)
	{
		val = gpio_get_value(pdat->keys[i].gpio);
		if(val != pdat->keys[i].state)
		{
			if(pdat->keys[i].active_low)
				type = val ? EVENT_TYPE_KEY_UP : EVENT_TYPE_KEY_DOWN;
			else
				type = val ? EVENT_TYPE_KEY_DOWN : EVENT_TYPE_KEY_UP;

			if(type == EVENT_TYPE_KEY_DOWN)
				push_event_key_down(input, pdat->keys[i].keycode);
			else if(type == EVENT_TYPE_KEY_UP)
				push_event_key_up(input, pdat->keys[i].keycode);
		}
		pdat->keys[i].state = val;
	}

	timer_forward_now(timer, ms_to_ktime(pdat->interval));
	return 1;
}

static int key_gpio_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * key_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct key_gpio_pdata_t * pdat;
	struct gpio_key_t * keys;
	struct input_t * input;
	struct device_t * dev;
	struct dtnode_t o;
	int nkeys, i;

	if((nkeys = dt_read_array_length(n, "keys")) <= 0)
		return NULL;

	pdat = malloc(sizeof(struct key_gpio_pdata_t));
	if(!pdat)
		return NULL;

	keys = malloc(sizeof(struct gpio_key_t) * nkeys);
	if(!keys)
	{
		free(pdat);
		return NULL;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		free(keys);
		return NULL;
	}

	for(i = 0; i < nkeys; i++)
	{
		dt_read_array_object(n, "keys", i, &o);
		keys[i].gpio = dt_read_int(&o, "gpio", -1);
		keys[i].active_low = dt_read_bool(&o, "active-low", 0);
		keys[i].keycode = dt_read_int(&o, "key-code", 0);

		gpio_set_pull(keys[i].gpio, keys[i].active_low ? GPIO_PULL_UP : GPIO_PULL_DOWN);
		gpio_direction_input(keys[i].gpio);
		keys[i].state = gpio_get_value(keys[i].gpio);
	}

	timer_init(&pdat->timer, key_gpio_timer_function, input);
	pdat->keys = keys;
	pdat->nkeys = nkeys;
	pdat->interval = dt_read_int(&o, "poll-interval", 100);

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = INPUT_TYPE_KEYBOARD;
	input->ioctl = key_gpio_ioctl;
	input->priv = pdat;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));

	if(!register_input(&dev, input))
	{
		timer_cancel(&pdat->timer);
		free(pdat->keys);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void key_gpio_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_pdata_t * pdat = (struct key_gpio_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		timer_cancel(&pdat->timer);
		free(pdat->keys);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void key_gpio_suspend(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_pdata_t * pdat = (struct key_gpio_pdata_t *)input->priv;

	timer_cancel(&pdat->timer);
}

static void key_gpio_resume(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_pdata_t * pdat = (struct key_gpio_pdata_t *)input->priv;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));
}

struct driver_t key_gpio = {
	.name		= "key-gpio",
	.probe		= key_gpio_probe,
	.remove		= key_gpio_remove,
	.suspend	= key_gpio_suspend,
	.resume		= key_gpio_resume,
};

static __init void key_gpio_driver_init(void)
{
	register_driver(&key_gpio);
}

static __exit void key_gpio_driver_exit(void)
{
	unregister_driver(&key_gpio);
}

driver_initcall(key_gpio_driver_init);
driver_exitcall(key_gpio_driver_exit);
