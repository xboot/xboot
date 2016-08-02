/*
 * driver/vibrator/vibrator-gpio.c
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
	int active_low;
	int state;
};

static void vibrator_gpio_set_state(struct vibrator_gpio_pdata_t * pdat, int state)
{
	if(state > 0)
		gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
	else
		gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
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
	pdat->active_low = dt_read_bool(n, "active-low", 0);
	pdat->state = 0;

	vib->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	vib->set = vibrator_gpio_set,
	vib->get = vibrator_gpio_get,
	vib->vibrate = vibrator_gpio_vibrate,
	vib->priv = pdat;

	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	vibrator_gpio_set_state(pdat, pdat->state);

	if(!register_vibrator(&dev, vib))
	{
		vibrator_gpio_set_state(pdat, 0);
		timer_cancel(&pdat->timer);
		queue_free(pdat->queue, iter_queue_node);

		free_device_name(vib->name);
		free(vib->priv);
		free(vib);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void vibrator_gpio_remove(struct device_t * dev)
{
	struct vibrator_t * vib = (struct vibrator_t *)dev->priv;
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;

	if(vib && unregister_vibrator(vib))
	{
		vibrator_gpio_set_state(pdat, 0);
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

struct driver_t vibrator_gpio = {
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
