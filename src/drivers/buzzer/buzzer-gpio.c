/*
 * driver/buzzer/buzzer-gpio.c
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

#include <buzzer/buzzer-gpio.h>

struct beep_param_t {
	int frequency;
	int millisecond;
};

struct buzzer_gpio_private_data_t {
	struct timer_t timer;
	struct queue_t * beep;
	int frequency;
	int gpio;
	int active_low;
};

static void buzzer_gpio_init(struct buzzer_t * buzzer)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;

	gpio_set_pull(dat->gpio, dat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	gpio_direction_output(dat->gpio, dat->active_low ? 1 : 0);
}

static void buzzer_gpio_exit(struct buzzer_t * buzzer)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;

	dat->frequency = 0;
	gpio_direction_output(dat->gpio, dat->active_low ? 1 : 0);
}

static void buzzer_gpio_set(struct buzzer_t * buzzer, int frequency)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;

	if(dat->frequency != frequency)
	{
		if(frequency > 0)
			gpio_direction_output(dat->gpio, dat->active_low ? 0 : 1);
		else
			gpio_direction_output(dat->gpio, dat->active_low ? 1 : 0);
		dat->frequency = frequency;
	}
}

static int buzzer_gpio_get(struct buzzer_t * buzzer)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;
	return dat->frequency;
}

static void buzzer_gpio_beep(struct buzzer_t * buzzer, int frequency, int millisecond)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;
	struct beep_param_t * param;

	if((frequency == 0) && (millisecond == 0))
	{
		timer_cancel(&dat->timer);
		queue_clear(dat->beep, 1);
		buzzer_gpio_set(buzzer, 0);
		return;
	}

	param = malloc(sizeof(struct beep_param_t));
	if(!param)
		return;
	param->frequency = frequency;
	param->millisecond = millisecond;

	queue_push(dat->beep, param);
	if(queue_avail(dat->beep) == 1)
		timer_start_now(&dat->timer, ms_to_ktime(1));
}

static void buzzer_gpio_suspend(struct buzzer_t * buzzer)
{
}

static void buzzer_gpio_resume(struct buzzer_t * buzzer)
{
}

static int buzzer_gpio_timer_function(struct timer_t * timer, void * data)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)(data);
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;
	struct beep_param_t * param = queue_pop(dat->beep);

	if(!param)
	{
		buzzer_gpio_set(buzzer, 0);
		return 0;
	}
	buzzer_gpio_set(buzzer, param->frequency);
	timer_forward_now(&dat->timer, ms_to_ktime(param->millisecond));
	free(param);
	return 1;
}

static bool_t buzzer_gpio_register_buzzer(struct resource_t * res)
{
	struct buzzer_gpio_data_t * rdat = (struct buzzer_gpio_data_t *)res->data;
	struct buzzer_gpio_private_data_t * dat;
	struct buzzer_t * buzzer;
	char name[64];

	dat = malloc(sizeof(struct buzzer_gpio_private_data_t));
	if(!dat)
		return FALSE;

	buzzer = malloc(sizeof(struct buzzer_t));
	if(!buzzer)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	timer_init(&dat->timer, buzzer_gpio_timer_function, buzzer);
	dat->beep = queue_alloc();
	dat->frequency = 0;
	dat->gpio = rdat->gpio;
	dat->active_low = rdat->active_low;

	buzzer->name = strdup(name);
	buzzer->init = buzzer_gpio_init;
	buzzer->exit = buzzer_gpio_exit;
	buzzer->set = buzzer_gpio_set,
	buzzer->get = buzzer_gpio_get,
	buzzer->beep = buzzer_gpio_beep,
	buzzer->suspend = buzzer_gpio_suspend,
	buzzer->resume = buzzer_gpio_resume,
	buzzer->priv = dat;

	if(register_buzzer(buzzer))
		return TRUE;

	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return FALSE;
}

static bool_t buzzer_gpio_unregister_buzzer(struct resource_t * res)
{
	struct buzzer_gpio_private_data_t * dat;
	struct buzzer_t * buzzer;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	buzzer = search_buzzer(name);
	if(!buzzer)
		return FALSE;
	dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;

	if(!unregister_buzzer(buzzer))
		return FALSE;

	timer_cancel(&dat->timer);
	queue_free(dat->beep, 1);
	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return TRUE;
}

static __init void buzzer_gpio_device_init(void)
{
	resource_for_each_with_name("buzzer-gpio", buzzer_gpio_register_buzzer);
}

static __exit void buzzer_gpio_device_exit(void)
{
	resource_for_each_with_name("buzzer-gpio", buzzer_gpio_unregister_buzzer);
}

device_initcall(buzzer_gpio_device_init);
device_exitcall(buzzer_gpio_device_exit);
