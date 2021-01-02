/*
 * driver/vibrator/vibrator-gpio.c
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
#include <vibrator/vibrator.h>

struct vibrate_param_t {
	int state;
	int millisecond;
};

struct vibrator_gpio_pdata_t {
	struct timer_t timer;
	struct queue_t * queue;
	int gpio;
	int gpiocfg;
	int active_low;
	int state;
};

static void vibrator_gpio_set_state(struct vibrator_gpio_pdata_t * pdat, int state)
{
	if(state > 0)
		gpio_set_value(pdat->gpio, pdat->active_low ? 0 : 1);
	else
		gpio_set_value(pdat->gpio, pdat->active_low ? 1 : 0);
}

static void iter_queue_node(struct queue_node_t * node)
{
	if(node && node->data)
		free(node->data);
}

static void vibrator_gpio_set(struct vibrator_t * vib, int state)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;

	if(pdat->state != state)
	{
		vibrator_gpio_set_state(pdat, state);
		pdat->state = state;
	}
}

static int vibrator_gpio_get(struct vibrator_t * vib)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	return pdat->state;
}

static void vibrator_gpio_vibrate(struct vibrator_t * vib, int state, int millisecond)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	struct vibrate_param_t * param;

	if((state == 0) && (millisecond == 0))
	{
		timer_cancel(&pdat->timer);
		queue_clear(pdat->queue, iter_queue_node);
		vibrator_gpio_set(vib, 0);
		return;
	}

	param = malloc(sizeof(struct vibrate_param_t));
	if(!param)
		return;
	param->state = state;
	param->millisecond = millisecond;

	queue_push(pdat->queue, param);
	if(queue_avail(pdat->queue) == 1)
		timer_start_now(&pdat->timer, ms_to_ktime(1));
}

static int vibrator_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct vibrator_t * vib = (struct vibrator_t *)(data);
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	struct vibrate_param_t * param = queue_pop(pdat->queue);

	if(!param)
	{
		vibrator_gpio_set(vib, 0);
		return 0;
	}
	vibrator_gpio_set(vib, param->state);
	timer_forward_now(&pdat->timer, ms_to_ktime(param->millisecond));
	free(param);
	return 1;
}

static struct device_t * vibrator_gpio_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct vibrator_gpio_pdata_t * pdat;
	struct vibrator_t * vib;
	struct device_t * dev;

	if(!gpio_is_valid(dt_read_int(n, "gpio", -1)))
		return NULL;

	pdat = malloc(sizeof(struct vibrator_gpio_pdata_t));
	if(!pdat)
		return NULL;

	vib = malloc(sizeof(struct vibrator_t));
	if(!vib)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, vibrator_gpio_timer_function, vib);
	pdat->queue = queue_alloc();
	pdat->gpio = dt_read_int(n, "gpio", -1);
	pdat->gpiocfg = dt_read_int(n, "gpio-config", -1);
	pdat->active_low = dt_read_bool(n, "active-low", 0);
	pdat->state = -1;

	vib->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	vib->set = vibrator_gpio_set;
	vib->get = vibrator_gpio_get;
	vib->vibrate = vibrator_gpio_vibrate;
	vib->priv = pdat;

	if(pdat->gpiocfg >= 0)
		gpio_set_cfg(pdat->gpio, pdat->gpiocfg);
	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	gpio_set_direction(pdat->gpio, GPIO_DIRECTION_OUTPUT);
	vibrator_gpio_set(vib, 0);

	if(!(dev = register_vibrator(vib, drv)))
	{
		timer_cancel(&pdat->timer);
		queue_free(pdat->queue, iter_queue_node);
		free_device_name(vib->name);
		free(vib->priv);
		free(vib);
		return NULL;
	}
	return dev;
}

static void vibrator_gpio_remove(struct device_t * dev)
{
	struct vibrator_t * vib = (struct vibrator_t *)dev->priv;
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;

	if(vib)
	{
		unregister_vibrator(vib);
		timer_cancel(&pdat->timer);
		queue_free(pdat->queue, iter_queue_node);
		free_device_name(vib->name);
		free(vib->priv);
		free(vib);
	}
}

static void vibrator_gpio_suspend(struct device_t * dev)
{
	struct vibrator_t * vib = (struct vibrator_t *)dev->priv;
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	vibrator_gpio_set_state(pdat, 0);
}

static void vibrator_gpio_resume(struct device_t * dev)
{
	struct vibrator_t * vib = (struct vibrator_t *)dev->priv;
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	vibrator_gpio_set_state(pdat, pdat->state);
}

static struct driver_t vibrator_gpio = {
	.name		= "vibrator-gpio",
	.probe		= vibrator_gpio_probe,
	.remove		= vibrator_gpio_remove,
	.suspend	= vibrator_gpio_suspend,
	.resume		= vibrator_gpio_resume,
};

static __init void vibrator_gpio_driver_init(void)
{
	register_driver(&vibrator_gpio);
}

static __exit void vibrator_gpio_driver_exit(void)
{
	unregister_driver(&vibrator_gpio);
}

driver_initcall(vibrator_gpio_driver_init);
driver_exitcall(vibrator_gpio_driver_exit);
