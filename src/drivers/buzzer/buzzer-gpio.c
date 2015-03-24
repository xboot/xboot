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

struct buzzer_gpio_private_data_t {
	int frequency;
	struct buzzer_gpio_data_t * rdat;
};

static void buzzer_gpio_init(struct buzzer_t * buzzer)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;
	struct buzzer_gpio_data_t * rdat = (struct buzzer_gpio_data_t *)dat->rdat;

	gpio_set_pull(rdat->gpio, rdat->active_low ? GPIO_PULL_UP :GPIO_PULL_DOWN);
	gpio_direction_output(rdat->gpio, rdat->active_low ? 1 : 0);
}

static void buzzer_gpio_exit(struct buzzer_t * buzzer)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;
	struct buzzer_gpio_data_t * rdat = (struct buzzer_gpio_data_t *)dat->rdat;

	dat->frequency = 0;
	gpio_direction_output(rdat->gpio, rdat->active_low ? 1 : 0);
}

static void buzzer_gpio_set(struct buzzer_t * buzzer, int frequency)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;
	struct buzzer_gpio_data_t * rdat = (struct buzzer_gpio_data_t *)dat->rdat;

	if(dat->frequency != frequency)
	{
		if(frequency > 0)
			gpio_direction_output(rdat->gpio, rdat->active_low ? 0 : 1);
		else
			gpio_direction_output(rdat->gpio, rdat->active_low ? 1 : 0);
		dat->frequency = frequency;
	}
}

static int buzzer_gpio_get(struct buzzer_t * buzzer)
{
	struct buzzer_gpio_private_data_t * dat = (struct buzzer_gpio_private_data_t *)buzzer->priv;
	return dat->frequency;
}

static void buzzer_gpio_suspend(struct buzzer_t * buzzer)
{
}

static void buzzer_gpio_resume(struct buzzer_t * buzzer)
{
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

	dat->frequency = 0;
	dat->rdat = rdat;

	buzzer->name = strdup(name);
	buzzer->init = buzzer_gpio_init;
	buzzer->exit = buzzer_gpio_exit;
	buzzer->set = buzzer_gpio_set,
	buzzer->get = buzzer_gpio_get,
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
