/*
 * driver/input/key-adc.c
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
#include <adc/adc.h>
#include <gpio/gpio.h>
#include <input/input.h>
#include <input/keyboard.h>

struct adc_key_t {
	int min;
	int max;
	int keycode;
};

struct key_adc_pdata_t {
	struct timer_t timer;
	struct adc_t * adc;
	struct adc_key_t * keys;
	int nkeys;
	int channel;
	int interval;
	int keyold;
};

static int key_adc_get_keycode(struct key_adc_pdata_t * pdat)
{
	int voltage;
	int i;

	voltage = adc_read_voltage(pdat->adc, pdat->channel);
	for(i = 0; i < pdat->nkeys; i++)
	{
		if((voltage >= pdat->keys[i].min) && (voltage < pdat->keys[i].max))
			return pdat->keys[i].keycode;
	}
	return 0;
}

static int key_adc_timer_function(struct timer_t * timer, void * data)
{
	struct input_t * input = (struct input_t *)(data);
	struct key_adc_pdata_t * pdat = (struct key_adc_pdata_t *)input->priv;
	int keynew = key_adc_get_keycode(pdat);

	if(keynew != pdat->keyold)
	{
		if(pdat->keyold == 0)
		{
			push_event_key_down(input, keynew);
			pdat->keyold = keynew;
		}
		else if(keynew == 0)
		{
			push_event_key_up(input, pdat->keyold);
			pdat->keyold = keynew;
		}
		else if((pdat->keyold != 0) && (keynew != 0))
		{
			push_event_key_up(input, pdat->keyold);
			pdat->keyold = 0;
		}
		else
		{
			pdat->keyold = keynew;
		}
	}

	timer_forward_now(timer, ms_to_ktime(pdat->interval));
	return 1;
}

static int key_adc_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * key_adc_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct key_adc_pdata_t * pdat;
	struct adc_t * adc;
	struct adc_key_t * keys;
	struct input_t * input;
	struct device_t * dev;
	struct dtnode_t o;
	int nkeys, i;

	if(!(adc = search_adc(dt_read_string(n, "adc-name", NULL))))
		return NULL;

	if((nkeys = dt_read_array_length(n, "keys")) <= 0)
		return NULL;

	pdat = malloc(sizeof(struct key_adc_pdata_t));
	if(!pdat)
		return NULL;

	keys = malloc(sizeof(struct adc_key_t) * nkeys);
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
		keys[i].min = dt_read_int(&o, "min-voltage", 0);
		keys[i].max = dt_read_int(&o, "max-voltage", 0);
		keys[i].keycode = dt_read_int(&o, "key-code", 0);
	}

	timer_init(&pdat->timer, key_adc_timer_function, input);
	pdat->adc = adc;
	pdat->keys = keys;
	pdat->nkeys = nkeys;
	pdat->channel = dt_read_int(n, "adc-channel", 0);
	pdat->interval = dt_read_int(n, "poll-interval-ms", 100);
	pdat->keyold = 0;

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = INPUT_TYPE_KEYBOARD;
	input->ioctl = key_adc_ioctl;
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

static void key_adc_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_adc_pdata_t * pdat = (struct key_adc_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		timer_cancel(&pdat->timer);
		free(pdat->keys);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void key_adc_suspend(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_adc_pdata_t * pdat = (struct key_adc_pdata_t *)input->priv;

	timer_cancel(&pdat->timer);
}

static void key_adc_resume(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct key_adc_pdata_t * pdat = (struct key_adc_pdata_t *)input->priv;

	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));
}

static struct driver_t key_adc = {
	.name		= "key-adc",
	.probe		= key_adc_probe,
	.remove		= key_adc_remove,
	.suspend	= key_adc_suspend,
	.resume		= key_adc_resume,
};

static __init void key_adc_driver_init(void)
{
	register_driver(&key_adc);
}

static __exit void key_adc_driver_exit(void)
{
	unregister_driver(&key_adc);
}

driver_initcall(key_adc_driver_init);
driver_exitcall(key_adc_driver_exit);
