/*
 * driver/sandbox-buzzer.c
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

#include <sandbox-buzzer.h>

struct beep_param_t {
	int frequency;
	int millisecond;
};

struct sandbox_buzzer_pdata_t {
	struct timer_t timer;
	struct queue_t * beep;
	int frequency;
	char * path;
};

static void iteration_beep_param(struct queue_node_t * node)
{
	if(node)
		free(node->data);
}

static void sandbox_buzzer_init(struct buzzer_t * buzzer)
{
}

static void sandbox_buzzer_exit(struct buzzer_t * buzzer)
{
}

static void sandbox_buzzer_set(struct buzzer_t * buzzer, int frequency)
{
	struct sandbox_buzzer_pdata_t * pdat = (struct sandbox_buzzer_pdata_t *)buzzer->priv;

	if(pdat->frequency != frequency)
	{
		sandbox_sysfs_write_value(pdat->path, frequency);
		pdat->frequency = frequency;
	}
}

static int sandbox_buzzer_get(struct buzzer_t * buzzer)
{
	struct sandbox_buzzer_pdata_t * pdat = (struct sandbox_buzzer_pdata_t *)buzzer->priv;
	return pdat->frequency;
}

static void sandbox_buzzer_beep(struct buzzer_t * buzzer, int frequency, int millisecond)
{
	struct sandbox_buzzer_pdata_t * pdat = (struct sandbox_buzzer_pdata_t *)buzzer->priv;
	struct beep_param_t * param;

	if((frequency == 0) && (millisecond == 0))
	{
		timer_cancel(&pdat->timer);
		queue_clear(pdat->beep, iteration_beep_param);
		sandbox_buzzer_set(buzzer, 0);
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

static void sandbox_buzzer_suspend(struct buzzer_t * buzzer)
{
}

static void sandbox_buzzer_resume(struct buzzer_t * buzzer)
{
}

static int sandbox_buzzer_timer_function(struct timer_t * timer, void * data)
{
	struct buzzer_t * buzzer = (struct buzzer_t *)(data);
	struct sandbox_buzzer_pdata_t * pdat = (struct sandbox_buzzer_pdata_t *)buzzer->priv;
	struct beep_param_t * param = queue_pop(pdat->beep);

	if(!param)
	{
		sandbox_buzzer_set(buzzer, 0);
		return 0;
	}
	sandbox_buzzer_set(buzzer, param->frequency);
	timer_forward_now(&pdat->timer, ms_to_ktime(param->millisecond));
	free(param);
	return 1;
}

static bool_t sandbox_buzzer_register_buzzer(struct resource_t * res)
{
	struct sandbox_buzzer_data_t * rdat = (struct sandbox_buzzer_data_t *)res->data;
	struct sandbox_buzzer_pdata_t * pdat;
	struct buzzer_t * buzzer;
	char name[64];

	pdat = malloc(sizeof(struct sandbox_buzzer_pdata_t));
	if(!pdat)
		return FALSE;

	buzzer = malloc(sizeof(struct buzzer_t));
	if(!buzzer)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	timer_init(&pdat->timer, sandbox_buzzer_timer_function, buzzer);
	pdat->beep = queue_alloc();
	pdat->frequency = 0;
	pdat->path = strdup(rdat->path);

	buzzer->name = strdup(name);
	buzzer->init = sandbox_buzzer_init;
	buzzer->exit = sandbox_buzzer_exit;
	buzzer->set = sandbox_buzzer_set,
	buzzer->get = sandbox_buzzer_get,
	buzzer->beep = sandbox_buzzer_beep,
	buzzer->suspend = sandbox_buzzer_suspend,
	buzzer->resume = sandbox_buzzer_resume,
	buzzer->priv = pdat;

	if(register_buzzer(buzzer))
		return TRUE;

	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return FALSE;
}

static bool_t sandbox_buzzer_unregister_buzzer(struct resource_t * res)
{
	struct sandbox_buzzer_pdata_t * pdat;
	struct buzzer_t * buzzer;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	buzzer = search_buzzer(name);
	if(!buzzer)
		return FALSE;
	pdat = (struct sandbox_buzzer_pdata_t *)buzzer->priv;

	if(!unregister_buzzer(buzzer))
		return FALSE;

	timer_cancel(&pdat->timer);
	queue_free(pdat->beep, iteration_beep_param);
	free(pdat->path);
	free(buzzer->priv);
	free(buzzer->name);
	free(buzzer);
	return TRUE;
}

static __init void sandbox_buzzer_device_init(void)
{
	resource_for_each_with_name("sandbox-buzzer", sandbox_buzzer_register_buzzer);
}

static __exit void sandbox_buzzer_device_exit(void)
{
	resource_for_each_with_name("sandbox-buzzer", sandbox_buzzer_unregister_buzzer);
}

device_initcall(sandbox_buzzer_device_init);
device_exitcall(sandbox_buzzer_device_exit);
