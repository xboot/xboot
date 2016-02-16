/*
 * driver/led/ledtrig-heartbeat.c
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

#include <led/ledtrig.h>

struct ledtrig_heartbeat_pdata_t {
	struct timer_t timer;
	int phase;
	int period;
};

static int heartbeat_timer_function(struct timer_t * timer, void * data)
{
	struct ledtrig_t * trigger = (struct ledtrig_t *)(data);
	struct ledtrig_heartbeat_pdata_t * pdat = (struct ledtrig_heartbeat_pdata_t *)trigger->priv;
	struct led_t * led = (struct led_t *)(trigger->led);
	int brightness = 0;
	int delay = 0;

	/*
	 * Acts like an actual heart beat -- thump-thump-pause ...
	 */
	switch(pdat->phase)
	{
	case 0:
		pdat->period = 1260;
		delay = 70;
		pdat->phase++;
		brightness = CONFIG_MAX_BRIGHTNESS;
		break;

	case 1:
		delay = pdat->period / 4 - 70;
		pdat->phase++;
		brightness = 0;
		break;

	case 2:
		delay = 70;
		pdat->phase++;
		brightness = CONFIG_MAX_BRIGHTNESS;
		break;

	default:
		delay = pdat->period - pdat->period / 4 - 70;
		pdat->phase = 0;
		brightness = 0;
		break;
	}

	led_set_brightness(led, brightness);
	timer_forward_now(timer, ms_to_ktime(delay));
	return 1;
}

static void ledtrig_heartbeat_init(struct ledtrig_t * trigger)
{
	struct ledtrig_heartbeat_pdata_t * pdat = (struct ledtrig_heartbeat_pdata_t *)trigger->priv;

	if(pdat)
	{
		pdat->phase = 0;
		timer_init(&pdat->timer, heartbeat_timer_function, trigger);
		timer_start_now(&pdat->timer, ms_to_ktime(10));
	}
}

static void ledtrig_heartbeat_exit(struct ledtrig_t * trigger)
{
	struct ledtrig_heartbeat_pdata_t * pdat = (struct ledtrig_heartbeat_pdata_t *)trigger->priv;

	if(pdat)
		timer_cancel(&pdat->timer);
}

static void ledtrig_heartbeat_activity(struct ledtrig_t * trigger)
{
}

static bool_t ledtrig_register_heartbeat(struct resource_t * res)
{
	struct ledtrig_data_t * rdat = (struct ledtrig_data_t *)res->data;
	struct ledtrig_heartbeat_pdata_t * pdat;
	struct ledtrig_t * trigger;
	struct led_t * led;
	char name[64];

	led = search_led(rdat->led);
	if(!led)
		return FALSE;

	pdat = malloc(sizeof(struct ledtrig_heartbeat_pdata_t));
	if(!pdat)
		return FALSE;

	trigger = malloc(sizeof(struct ledtrig_t));
	if(!trigger)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->phase = 0;
	pdat->period = 0;

	trigger->name = strdup(name);
	trigger->init = ledtrig_heartbeat_init;
	trigger->exit = ledtrig_heartbeat_exit;
	trigger->activity = ledtrig_heartbeat_activity;
	trigger->led = led;
	trigger->priv = pdat;

	if(register_ledtrig(trigger))
		return TRUE;

	free(trigger->priv);
	free(trigger->name);
	free(trigger);
	return FALSE;
}

static bool_t ledtrig_unregister_heartbeat(struct resource_t * res)
{
	struct ledtrig_t * trigger;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	trigger = search_ledtrig(name);
	if(!trigger)
		return FALSE;

	if(!unregister_ledtrig(trigger))
		return FALSE;

	free(trigger->priv);
	free(trigger->name);
	free(trigger);
	return TRUE;
}

static __init void ledtrig_heartbeat_device_init(void)
{
	resource_for_each_with_name("ledtrig-heartbeat", ledtrig_register_heartbeat);
}

static __exit void ledtrig_heartbeat_device_exit(void)
{
	resource_for_each_with_name("ledtrig-heartbeat", ledtrig_unregister_heartbeat);
}

postdevice_initcall(ledtrig_heartbeat_device_init);
postdevice_exitcall(ledtrig_heartbeat_device_exit);
