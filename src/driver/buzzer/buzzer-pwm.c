/*
 * driver/buzzer/buzzer-pwm.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <buzzer/buzzer.h>

struct beep_param_t {
	int frequency;
	int millisecond;
};

struct buzzer_pwm_pdata_t {
	struct timer_t timer;
	struct queue_t * queue;
	struct pwm_t * pwm;
	int polarity;
	int frequency;
};

static void buzzer_pwm_set_frequency(struct buzzer_pwm_pdata_t * pdat, int frequency)
{
	if(frequency > 0)
	{
		int period = 1000000000ULL / frequency;
		pwm_config(pdat->pwm, period / 2, period, pdat->polarity);
		pwm_enable(pdat->pwm);
	}
	else
	{
		pwm_disable(pdat->pwm);
	}
}

static void queue_node_callback(struct queue_node_t * node)
{
	if(node && node->data)
		free(node->data);
}

static void buzzer_pwm_set(struct buzzer_t * buzzer, int frequency)
{
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;

	if(pdat->frequency != frequency)
	{
		buzzer_pwm_set_frequency(pdat, frequency);
		pdat->frequency = frequency;
	}
}

static int buzzer_pwm_get(struct buzzer_t * buzzer)
{
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	return pdat->frequency;
}

static void buzzer_pwm_beep(struct buzzer_t * buzzer, int frequency, int millisecond)
{
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	struct beep_param_t * param;

	if((frequency == 0) && (millisecond == 0))
	{
		timer_cancel(&pdat->timer);
		queue_clear(pdat->queue, queue_node_callback);
		buzzer_pwm_set(buzzer, 0);
		return;
	}

	param = malloc(sizeof(struct beep_param_t));
	if(!param)
		return;
	param->frequency = frequency;
	param->millisecond = millisecond;

	queue_push(pdat->queue, param);
	if(queue_avail(pdat->queue) == 1)
		timer_start_now(&pdat->timer, ms_to_ktime(1));
}

static int buzzer_pwm_timer_function(struct timer_t * timer, void * data)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)(data);
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	struct beep_param_t * param = queue_pop(pdat->queue);

	if(!param)
	{
		buzzer_pwm_set(buzzer, 0);
		return 0;
	}
	buzzer_pwm_set(buzzer, param->frequency);
	timer_forward_now(&pdat->timer, ms_to_ktime(param->millisecond));
	free(param);
	return 1;
}

static struct device_t * buzzer_pwm_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct buzzer_pwm_pdata_t * pdat;
	struct pwm_t * pwm;
	struct buzzer_t * buzzer;
	struct device_t * dev;

	if(!(pwm = search_pwm(dt_read_string(n, "pwm-name", NULL))))
		return NULL;

	pdat = malloc(sizeof(struct buzzer_pwm_pdata_t));
	if(!pdat)
		return NULL;

	buzzer = malloc(sizeof(struct buzzer_t));
	if(!buzzer)
	{
		free(pdat);
		return NULL;
	}

	timer_init(&pdat->timer, buzzer_pwm_timer_function, buzzer);
	pdat->queue = queue_alloc();
	pdat->pwm = pwm;
	pdat->polarity = dt_read_bool(n, "pwm-polarity", 0);
	pdat->frequency = -1;

	buzzer->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	buzzer->set = buzzer_pwm_set;
	buzzer->get = buzzer_pwm_get;
	buzzer->beep = buzzer_pwm_beep;
	buzzer->priv = pdat;

	buzzer_pwm_set(buzzer, 0);

	if(!(dev = register_buzzer(buzzer, drv)))
	{
		timer_cancel(&pdat->timer);
		queue_free(pdat->queue, queue_node_callback);
		free_device_name(buzzer->name);
		free(buzzer->priv);
		free(buzzer);
		return NULL;
	}
	return dev;
}

static void buzzer_pwm_remove(struct device_t * dev)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)dev->priv;
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;

	if(buzzer)
	{
		unregister_buzzer(buzzer);
		timer_cancel(&pdat->timer);
		queue_free(pdat->queue, queue_node_callback);
		free_device_name(buzzer->name);
		free(buzzer->priv);
		free(buzzer);
	}
}

static void buzzer_pwm_suspend(struct device_t * dev)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)dev->priv;
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	buzzer_pwm_set_frequency(pdat, 0);
}

static void buzzer_pwm_resume(struct device_t * dev)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)dev->priv;
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	buzzer_pwm_set_frequency(pdat, pdat->frequency);
}

static struct driver_t buzzer_pwm = {
	.name		= "buzzer-pwm",
	.probe		= buzzer_pwm_probe,
	.remove		= buzzer_pwm_remove,
	.suspend	= buzzer_pwm_suspend,
	.resume		= buzzer_pwm_resume,
};

static __init void buzzer_pwm_driver_init(void)
{
	register_driver(&buzzer_pwm);
}

static __exit void buzzer_pwm_driver_exit(void)
{
	unregister_driver(&buzzer_pwm);
}

driver_initcall(buzzer_pwm_driver_init);
driver_exitcall(buzzer_pwm_driver_exit);
