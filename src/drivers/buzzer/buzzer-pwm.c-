/*
 * driver/buzzer/buzzer-pwm.c
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

#include <buzzer/buzzer-pwm.h>

struct beep_param_t {
	int frequency;
	int millisecond;
};

struct buzzer_pwm_pdata_t {
	struct timer_t timer;
	struct queue_t * beep;
	int frequency;
	int polarity;
	struct pwm_t * pwm;
};

static void iter_beep_param(struct queue_node_t * node)
{
	if(node)
		free(node->data);
}

static void buzzer_pwm_init(struct buzzer_t * buzzer)
{
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	pdat->frequency = 0;
	pwm_disable(pdat->pwm);
}

static void buzzer_pwm_exit(struct buzzer_t * buzzer)
{
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	pdat->frequency = 0;
	pwm_disable(pdat->pwm);
}

static void buzzer_pwm_set(struct buzzer_t * buzzer, int frequency)
{
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	u32_t period;

	if(pdat->frequency != frequency)
	{
		if(frequency > 0)
		{
			period = 1000000000ULL / frequency;
			pwm_config(pdat->pwm, period / 2, period, pdat->polarity ? TRUE : FALSE);
			pwm_enable(pdat->pwm);
		}
		else
		{
			pwm_disable(pdat->pwm);
		}
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
		queue_clear(pdat->beep, iter_beep_param);
		buzzer_pwm_set(buzzer, 0);
		return;
	}

	param = malloc(sizeof(struct beep_param_t));
	if(!param)
		return;
	param->frequency = frequency;
	param->millisecond = millisecond;

	queue_push(pdat->beep, param);
	if(queue_avail(pdat->beep) == 1)
		timer_start_now(&pdat->timer, ms_to_ktime(1));
}

static void buzzer_pwm_suspend(struct buzzer_t * buzzer)
{
}

static void buzzer_pwm_resume(struct buzzer_t * buzzer)
{
}

static int buzzer_pwm_timer_function(struct timer_t * timer, void * data)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)(data);
	struct buzzer_pwm_pdata_t * pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;
	struct beep_param_t * param = queue_pop(pdat->beep);
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

static bool_t buzzer_pwm_register_buzzer(struct resource_t * res)
{
	struct buzzer_pwm_data_t * rdat = (struct buzzer_pwm_data_t *)res->data;
	struct buzzer_pwm_pdata_t * pdat;
	struct buzzer_t * buzzer;
	struct pwm_t * pwm;
	char name[64];

	pwm = search_pwm(rdat->pwm);
	if(!pwm)
		return FALSE;

	pdat = malloc(sizeof(struct buzzer_pwm_pdata_t));
	if(!pdat)
		return FALSE;

	buzzer = malloc(sizeof(struct buzzer_t));
	if(!buzzer)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	timer_init(&pdat->timer, buzzer_pwm_timer_function, buzzer);
	pdat->beep = queue_alloc();
	pdat->frequency = 0;
	pdat->polarity = rdat->polarity;
	pdat->pwm = pwm;

	buzzer->name = strdup(name);
	buzzer->init = buzzer_pwm_init;
	buzzer->exit = buzzer_pwm_exit;
	buzzer->set = buzzer_pwm_set,
	buzzer->get = buzzer_pwm_get,
	buzzer->beep = buzzer_pwm_beep,
	buzzer->suspend = buzzer_pwm_suspend,
	buzzer->resume = buzzer_pwm_resume,
	buzzer->priv = pdat;

	if(register_buzzer(buzzer))
		return TRUE;

	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return FALSE;
}

static bool_t buzzer_pwm_unregister_buzzer(struct resource_t * res)
{
	struct buzzer_pwm_pdata_t * pdat;
	struct buzzer_t * buzzer;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	buzzer = search_buzzer(name);
	if(!buzzer)
		return FALSE;
	pdat = (struct buzzer_pwm_pdata_t *)buzzer->priv;

	if(!unregister_buzzer(buzzer))
		return FALSE;

	timer_cancel(&pdat->timer);
	queue_free(pdat->beep, iter_beep_param);
	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return TRUE;
}

static __init void buzzer_pwm_device_init(void)
{
	resource_for_each("buzzer-pwm", buzzer_pwm_register_buzzer);
}

static __exit void buzzer_pwm_device_exit(void)
{
	resource_for_each("buzzer-pwm", buzzer_pwm_unregister_buzzer);
}

device_initcall(buzzer_pwm_device_init);
device_exitcall(buzzer_pwm_device_exit);
