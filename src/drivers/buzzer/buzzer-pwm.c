/*
 * driver/buzzer/buzzer-pwm.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

struct buzzer_pwm_private_data_t {
	int frequency;
	bool_t polarity;
	struct pwm_t * pwm;
};

static void buzzer_pwm_init(struct buzzer_t * buzzer)
{
	struct buzzer_pwm_private_data_t * dat = (struct buzzer_pwm_private_data_t *)buzzer->priv;

	dat->frequency = 0;
	pwm_disable(dat->pwm);
}

static void buzzer_pwm_exit(struct buzzer_t * buzzer)
{
	struct buzzer_pwm_private_data_t * dat = (struct buzzer_pwm_private_data_t *)buzzer->priv;

	dat->frequency = 0;
	pwm_disable(dat->pwm);
}

static void buzzer_pwm_set(struct buzzer_t * buzzer, int frequency)
{
	struct buzzer_pwm_private_data_t * dat = (struct buzzer_pwm_private_data_t *)buzzer->priv;
	u32_t period;

	if(dat->frequency != frequency)
	{
		if(frequency > 0)
		{
			period = 1000000000ULL / dat->frequency;
			pwm_config(dat->pwm, period / 2, period, dat->polarity);
			pwm_enable(dat->pwm);
		}
		else
		{
			pwm_disable(dat->pwm);
		}
		dat->frequency = frequency;
	}
}

static int buzzer_pwm_get(struct buzzer_t * buzzer)
{
	struct buzzer_pwm_private_data_t * dat = (struct buzzer_pwm_private_data_t *)buzzer->priv;
	return dat->frequency;
}

static void buzzer_pwm_suspend(struct buzzer_t * buzzer)
{
}

static void buzzer_pwm_resume(struct buzzer_t * buzzer)
{
}

static bool_t buzzer_pwm_register_buzzer(struct resource_t * res)
{
	struct buzzer_pwm_data_t * rdat = (struct buzzer_pwm_data_t *)res->data;
	struct buzzer_pwm_private_data_t * dat;
	struct buzzer_t * buzzer;
	struct pwm_t * pwm;
	char name[64];

	pwm = search_pwm(rdat->pwm);
	if(!pwm)
		return FALSE;

	dat = malloc(sizeof(struct buzzer_pwm_private_data_t));
	if(!dat)
		return FALSE;

	buzzer = malloc(sizeof(struct buzzer_t));
	if(!buzzer)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->frequency = 0;
	dat->polarity = rdat->polarity;
	dat->pwm = pwm;

	buzzer->name = strdup(name);
	buzzer->init = buzzer_pwm_init;
	buzzer->exit = buzzer_pwm_exit;
	buzzer->set = buzzer_pwm_set,
	buzzer->get = buzzer_pwm_get,
	buzzer->suspend = buzzer_pwm_suspend,
	buzzer->resume = buzzer_pwm_resume,
	buzzer->priv = dat;

	if(register_buzzer(buzzer))
		return TRUE;

	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return FALSE;
}

static bool_t buzzer_pwm_unregister_buzzer(struct resource_t * res)
{
	struct buzzer_t * buzzer;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	buzzer = search_buzzer(name);
	if(!buzzer)
		return FALSE;

	if(!unregister_buzzer(buzzer))
		return FALSE;

	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return TRUE;
}

static __init void buzzer_pwm_device_init(void)
{
	resource_for_each_with_name("buzzer-pwm", buzzer_pwm_register_buzzer);
}

static __exit void buzzer_pwm_device_exit(void)
{
	resource_for_each_with_name("buzzer-pwm", buzzer_pwm_unregister_buzzer);
}

device_initcall(buzzer_pwm_device_init);
device_exitcall(buzzer_pwm_device_exit);
