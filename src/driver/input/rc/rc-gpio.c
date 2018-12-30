/*
 * driver/input/rc/rc-gpio.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <input/rc/rc-core.h>

struct rc_gpio_pdata_t {
	struct rc_decoder_t decoder;
	ktime_t last;
	int gpio;
	int gpiocfg;
	int irq;
	int active_low;
};

static void rc_gpio_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct rc_gpio_pdata_t * pdat = (struct rc_gpio_pdata_t *)input->priv;
	ktime_t now = ktime_get();
	int pulse, duration;
	uint32_t key;

	pulse = (gpio_get_value(pdat->gpio) != 0) ? 0 : 1;
	if(pdat->active_low)
		pulse = !pulse;
	duration = ktime_us_delta(now, pdat->last);
	pdat->last = now;

	key = rc_decoder_handle(&pdat->decoder, pulse, duration);
	if(key != 0)
	{
		push_event_key_down(input, key);
		push_event_key_up(input, key);
	}
}

static int rc_gpio_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * rc_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rc_gpio_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct dtnode_t o;
	int gpio = dt_read_int(n, "gpio", -1);
	int irq = gpio_to_irq(gpio);
	int i;

	if(!gpio_is_valid(gpio) || !irq_is_valid(irq))
		return NULL;

	pdat = malloc(sizeof(struct rc_gpio_pdata_t));
	if(!pdat)
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		free(pdat);
		return NULL;
	}

	memset(&pdat->decoder, 0, sizeof(struct rc_decoder_t));
	if((pdat->decoder.size = dt_read_array_length(n, "maps")) > 0)
	{
		pdat->decoder.map = malloc(sizeof(struct rc_map_t) * pdat->decoder.size);
		for(i = 0; i < pdat->decoder.size; i++)
		{
			dt_read_array_object(n, "maps", i, &o);
			pdat->decoder.map[i].scancode = dt_read_int(&o, "scan-code", 0);
			pdat->decoder.map[i].keycode = dt_read_int(&o, "key-code", 0);
		}
	}
	pdat->last = ktime_get();
	pdat->gpio = gpio;
	pdat->gpiocfg = dt_read_int(n, "gpio-config", -1);
	pdat->irq = irq;
	pdat->active_low = dt_read_bool(n, "active-low", 1);

	input->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->type = INPUT_TYPE_KEYBOARD;
	input->ioctl = rc_gpio_ioctl;
	input->priv = pdat;

	if(pdat->gpiocfg >= 0)
		gpio_set_cfg(pdat->gpio, pdat->gpiocfg);
	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP : GPIO_PULL_DOWN);
	gpio_direction_input(pdat->gpio);
	request_irq(pdat->irq, rc_gpio_interrupt, IRQ_TYPE_EDGE_BOTH, input);

	if(!register_input(&dev, input))
	{
		free_irq(pdat->irq);
		if(pdat->decoder.size > 0)
			free(pdat->decoder.map);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void rc_gpio_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rc_gpio_pdata_t * pdat = (struct rc_gpio_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		free_irq(pdat->irq);
		if(pdat->decoder.size > 0)
			free(pdat->decoder.map);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void rc_gpio_suspend(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rc_gpio_pdata_t * pdat = (struct rc_gpio_pdata_t *)input->priv;

	disable_irq(pdat->irq);
}

static void rc_gpio_resume(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct rc_gpio_pdata_t * pdat = (struct rc_gpio_pdata_t *)input->priv;

	enable_irq(pdat->irq);
}

static struct driver_t rc_gpio = {
	.name		= "rc-gpio",
	.probe		= rc_gpio_probe,
	.remove		= rc_gpio_remove,
	.suspend	= rc_gpio_suspend,
	.resume		= rc_gpio_resume,
};

static __init void rc_gpio_driver_init(void)
{
	register_driver(&rc_gpio);
}

static __exit void rc_gpio_driver_exit(void)
{
	unregister_driver(&rc_gpio);
}

driver_initcall(rc_gpio_driver_init);
driver_exitcall(rc_gpio_driver_exit);
