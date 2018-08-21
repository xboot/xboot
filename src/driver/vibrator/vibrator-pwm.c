/*
 * driver/vibrator/vibrator-pwm.c
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
#include <pwm/pwm.h>
#include <vibrator/vibrator.h>

struct vibrate_param_t {
	int state;
	int millisecond;
};

struct vibrator_pwm_pdata_t {
	struct timer_t timer;
	struct queue_t * queue;
	struct pwm_t * pwm;
	int period;
	int polarity;
	int state;
};

static void vibrator_pwm_set_state(struct vibrator_pwm_pdata_t * pdat, int state)
{
	if(state > 0)
	{
		pwm_config(pdat->pwm, pdat->period / 2, pdat->period, pdat->polarity);
		pwm_enable(pdat->pwm);
	}
	else
	{
		pwm_disable(pdat->pwm);
	}
}

static void iter_queue_node(struct queue_node_t * node)
{
	if(node && node->data)
		free(node->data);
}

static void vibrator_pwm_set(struct vibrator_t * vib, int state)
{
	struct vibrator_pwm_pdata_t * pdat = (struct vibrator_pwm_pdata_t *)vib->priv;

	if(pdat->state != state)
	{
		vibrator_pwm_set_state(pdat, state);
		pdat->state = state;
	}
}

static int vibrator_pwm_get(struct vibrator_t * vib)
{
	struct vibrator_pwm_pdata_t * pdat = (struct vibrator_pwm_pdata_t *)vib->priv;
	return pdat->state;
}

static void vibrator_pwm_vibrate(struct vibrator_t * vib, int state, int millisecond)
{
	struct vibrator_pwm_pdata_t * pdat = (struct vibrator_pwm_pdata_t *)vib->priv;
	struct vibrate_param_t * param;

	if((state == 0) && (millisecond == 0))
	{
		timer_cancel(&pdat->timer);
		queue_clear(pdat->queue, iter_queue_node);
		vibrator_pwm_set(vib, 0);
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

static int vibrator_pwm_timer_function(struct timer_t * timer, void * data)
{
	struct vibrator_t * vib = (struct vibrator_t *)(data);
	struct vibrator_pwm_pdata_t * pdat = (struct vibrator_pwm_pdata_t *)vib->priv;
	struct vibrate_param_t * param = queue_pop(pdat->queue);

	if(!param)
	{
		vibrator_pwm_set(vib, 0);
		return 0;
	}
	vibrator_pwm_set(vib, param->state);
	timer_forward_now(&pdat->timer, ms_to_ktime(param->millisecond));
	free(param);
	return 1;
}

static struct device_t * vibrator_pwm_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct vibrator_pwm_pdata_t * pdat;
	struct pwm_t * pwm;
	struct vibrator_t * vib;
	struct device_t * dev;

	if(!(pwm = search_pwm(dt_read_string(n, "pwm-name", NULL))))
		return NULL;

	pdat = malloc(sizeof(struct vibrator_pwm_pdata_t));
	if(!pdat)
		return NULL;

	vib = malloc(sizeof(struct vibrator_t));
	if(!vib)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, vibrator_pwm_timer_function, vib);
	pdat->queue = queue_alloc();
	pdat->pwm = pwm;
	pdat->period = dt_read_int(n, "pwm-period-ns", 2272727);
	pdat->polarity = dt_read_bool(n, "pwm-polarity", 0);
	pdat->state = -1;

	vib->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	vib->set = vibrator_pwm_set;
	vib->get = vibrator_pwm_get;
	vib->vibrate = vibrator_pwm_vibrate;
	vib->priv = pdat;

	vibrator_pwm_set(vib, 0);

	if(!register_vibrator(&dev, vib))
	{
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

static void vibrator_pwm_remove(struct device_t * dev)
{
	struct vibrator_t * vib = (struct vibrator_t *)dev->priv;
	struct vibrator_pwm_pdata_t * pdat = (struct vibrator_pwm_pdata_t *)vib->priv;

	if(vib && unregister_vibrator(vib))
	{
		timer_cancel(&pdat->timer);
		queue_free(pdat->queue, iter_queue_node);

		free_device_name(vib->name);
		free(vib->priv);
		free(vib);
	}
}

static void vibrator_pwm_suspend(struct device_t * dev)
{
	struct vibrator_t * vib = (struct vibrator_t *)dev->priv;
	struct vibrator_pwm_pdata_t * pdat = (struct vibrator_pwm_pdata_t *)vib->priv;
	vibrator_pwm_set_state(pdat, 0);
}

static void vibrator_pwm_resume(struct device_t * dev)
{
	struct vibrator_t * vib = (struct vibrator_t *)dev->priv;
	struct vibrator_pwm_pdata_t * pdat = (struct vibrator_pwm_pdata_t *)vib->priv;
	vibrator_pwm_set_state(pdat, pdat->state);
}

static struct driver_t vibrator_pwm = {
	.name		= "vibrator-pwm",
	.probe		= vibrator_pwm_probe,
	.remove		= vibrator_pwm_remove,
	.suspend	= vibrator_pwm_suspend,
	.resume		= vibrator_pwm_resume,
};

static __init void vibrator_pwm_driver_init(void)
{
	register_driver(&vibrator_pwm);
}

static __exit void vibrator_pwm_driver_exit(void)
{
	unregister_driver(&vibrator_pwm);
}

driver_initcall(vibrator_pwm_driver_init);
driver_exitcall(vibrator_pwm_driver_exit);
