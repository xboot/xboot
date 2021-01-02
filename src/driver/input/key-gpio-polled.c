/*
 * driver/input/key-gpio-polled.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <input/input.h>
#include <input/keyboard.h>

struct gpio_key_t {
	int gpio;
	int gpiocfg;
	int active_low;
	int keycode;
	int state;
};

struct key_gpio_polled_pdata_t {
	struct timer_t timer;
	struct gpio_key_t * keys;
	int nkeys;
	int interval;
};

static int key_gpio_polled_timer_function(struct timer_t * timer, void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct key_gpio_polled_pdata_t * pdat = (struct key_gpio_polled_pdata_t *)input->priv;
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
			pdat->keys[i].state = val;
		}
	}

	timer_forward_now(timer, ms_to_ktime(pdat->interval));
	return 1;
}

static int key_gpio_polled_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	return -1;
}

static struct device_t * key_gpio_polled_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct key_gpio_polled_pdata_t * pdat;
	struct gpio_key_t * keys;
	struct input_t * input;
	struct device_t * dev;
	struct dtnode_t o;
	int nkeys, i;

	if((nkeys = dt_read_array_length(n, "keys")) <= 0)
		return NULL;

	pdat = malloc(sizeof(struct key_gpio_polled_pdata_t));
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
		keys[i].gpiocfg = dt_read_int(&o, "gpio-config", -1);
		keys[i].active_low = dt_read_bool(&o, "active-low", 0);
		keys[i].keycode = dt_read_int(&o, "key-code", 0);

		if(keys[i].gpiocfg >= 0)
			gpio_set_cfg(keys[i].gpio, keys[i].gpiocfg);
		gpio_set_pull(keys[i].gpio, keys[i].active_low ? GPIO_PULL_UP : GPIO_PULL_DOWN);
		gpio_set_direction(keys[i].gpio, GPIO_DIRECTION_INPUT);
		keys[i].state = gpio_get_value(keys[i].gpio);
	}

	timer_init(&pdat->timer, key_gpio_polled_timer_function, input);
	pdat->keys = keys;
	pdat->nkeys = nkeys;
	pdat->interval = dt_read_int(n, "poll-interval-ms", 100);

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->ioctl = key_gpio_polled_ioctl;
	input->priv = pdat;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));

	if(!(dev = register_input(input, drv)))
	{
		timer_cancel(&pdat->timer);
		free(pdat->keys);
		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	return dev;
}

static void key_gpio_polled_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_polled_pdata_t * pdat = (struct key_gpio_polled_pdata_t *)input->priv;

	if(input)
	{
		unregister_input(input);
		timer_cancel(&pdat->timer);
		free(pdat->keys);
		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void key_gpio_polled_suspend(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_polled_pdata_t * pdat = (struct key_gpio_polled_pdata_t *)input->priv;

	timer_cancel(&pdat->timer);
}

static void key_gpio_polled_resume(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_gpio_polled_pdata_t * pdat = (struct key_gpio_polled_pdata_t *)input->priv;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));
}

static struct driver_t key_gpio_polled = {
	.name		= "key-gpio-polled",
	.probe		= key_gpio_polled_probe,
	.remove		= key_gpio_polled_remove,
	.suspend	= key_gpio_polled_suspend,
	.resume		= key_gpio_polled_resume,
};

static __init void key_gpio_polled_driver_init(void)
{
	register_driver(&key_gpio_polled);
}

static __exit void key_gpio_polled_driver_exit(void)
{
	unregister_driver(&key_gpio_polled);
}

driver_initcall(key_gpio_polled_driver_init);
driver_exitcall(key_gpio_polled_driver_exit);
