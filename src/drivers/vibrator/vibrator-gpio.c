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

#include <vibrator/vibrator-gpio.h>

struct vibrate_param_t {
	int state;
	int ms;
};

struct vibrator_gpio_pdata_t {
	struct timer_t timer;
	struct queue_t * vibrate;
	int state;
	int gpio;
	int active_low;
};

static void iter_vibrate_param(struct queue_node_t * node)
{
	if(node)
		free(node->data);
}

static void vibrator_gpio_init(struct vibrator_t * vib)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	gpio_set_pull(pdat->gpio, pdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
}

static void vibrator_gpio_exit(struct vibrator_t * vib)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	pdat->state = 0;
	gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
}

static void vibrator_gpio_set(struct vibrator_t * vib, int state)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;

	if(pdat->state != state)
	{
		if(state > 0)
			gpio_direction_output(pdat->gpio, pdat->active_low ? 0 : 1);
		else
			gpio_direction_output(pdat->gpio, pdat->active_low ? 1 : 0);
		pdat->state = state;
	}
}

static int vibrator_gpio_get(struct vibrator_t * vib)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	return pdat->state;
}

static void vibrator_gpio_vibrate(struct vibrator_t * vib, int state, int ms)
{
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	struct vibrate_param_t * param;

	if((state == 0) && (ms == 0))
	{
		timer_cancel(&pdat->timer);
		queue_clear(pdat->vibrate, iter_vibrate_param);
		vibrator_gpio_set(vib, 0);
		return;
	}

	param = malloc(sizeof(struct vibrate_param_t));
	if(!param)
		return;
	param->state = state;
	param->ms = ms;

	queue_push(pdat->vibrate, param);
	if(queue_avail(pdat->vibrate) == 1)
		timer_start_now(&pdat->timer, ms_to_ktime(1));
}

static void vibrator_gpio_suspend(struct vibrator_t * vib)
{
}

static void vibrator_gpio_resume(struct vibrator_t * vib)
{
}

static int vibrator_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct vibrator_t * vib = (struct vibrator_t *)(data);
	struct vibrator_gpio_pdata_t * pdat = (struct vibrator_gpio_pdata_t *)vib->priv;
	struct vibrate_param_t * param = queue_pop(pdat->vibrate);

	if(!param)
	{
		vibrator_gpio_set(vib, 0);
		return 0;
	}
	vibrator_gpio_set(vib, param->state);
	timer_forward_now(&pdat->timer, ms_to_ktime(param->ms));
	free(param);
	return 1;
}

static bool_t vibrator_gpio_register_vibrator(struct resource_t * res)
{
	struct vibrator_gpio_data_t * rdat = (struct vibrator_gpio_data_t *)res->data;
	struct vibrator_gpio_pdata_t * pdat;
	struct vibrator_t * vib;
	char name[64];

	pdat = malloc(sizeof(struct vibrator_gpio_pdata_t));
	if(!pdat)
		return FALSE;

	vib = malloc(sizeof(struct vibrator_t));
	if(!vib)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	timer_init(&pdat->timer, vibrator_gpio_timer_function, vib);
	pdat->vibrate = queue_alloc();
	pdat->state = 0;
	pdat->gpio = rdat->gpio;
	pdat->active_low = rdat->active_low;

	vib->name = strdup(name);
	vib->init = vibrator_gpio_init;
	vib->exit = vibrator_gpio_exit;
	vib->set = vibrator_gpio_set,
	vib->get = vibrator_gpio_get,
	vib->vibrate = vibrator_gpio_vibrate,
	vib->suspend = vibrator_gpio_suspend,
	vib->resume = vibrator_gpio_resume,
	vib->priv = pdat;

	if(register_vibrator(vib))
		return TRUE;

	free(vib->priv);
	free(vib->name);
	free(vib);
	return FALSE;
}

static bool_t vibrator_gpio_unregister_vibrator(struct resource_t * res)
{
	struct vibrator_gpio_pdata_t * pdat;
	struct vibrator_t * vib;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	vib = search_vibrator(name);
	if(!vib)
		return FALSE;
	pdat = (struct vibrator_gpio_pdata_t *)vib->priv;

	if(!unregister_vibrator(vib))
		return FALSE;

	timer_cancel(&pdat->timer);
	queue_free(pdat->vibrate, iter_vibrate_param);
	free(vib->priv);
	free(vib->name);
	free(vib);
	return TRUE;
}

static __init void vibrator_gpio_device_init(void)
{
	resource_for_each("vibrator-gpio", vibrator_gpio_register_vibrator);
}

static __exit void vibrator_gpio_device_exit(void)
{
	resource_for_each("vibrator-gpio", vibrator_gpio_unregister_vibrator);
}

device_initcall(vibrator_gpio_device_init);
device_exitcall(vibrator_gpio_device_exit);
