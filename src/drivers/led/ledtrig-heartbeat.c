/*
 * driver/led/ledtrig-heartbeat.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <led/ledtrig.h>

struct led_trigger_heartbeat_data_t {
	struct timer_t timer;
	u32_t phase;
	u32_t period;

	struct led_trigger_data_t * rdat;
};

static void heartbeat_timer_function(u32_t data)
{
	struct led_trigger_t * trigger = (struct led_trigger_t *)(data);
	struct led_trigger_heartbeat_data_t * dat = (struct led_trigger_heartbeat_data_t *)trigger->priv;
	struct led_t * led = (struct led_t *)(trigger->led);
	u32_t color = 0;
	u32_t delay = 0;

	/*
	 * Acts like an actual heart beat -- thump-thump-pause ...
	 */
	switch(dat->phase)
	{
	case 0:
		dat->period = 1260 * get_system_hz() / 1000;
		delay = 70 * get_system_hz() / 1000;
		dat->phase++;
		color = 0x00ffffff;
		break;

	case 1:
		delay = dat->period / 4 - (70 * get_system_hz() / 1000);
		dat->phase++;
		color = 0;
		break;

	case 2:
		delay = 70 * get_system_hz() / 1000;
		dat->phase++;
		color = 0x00ffffff;
		break;

	default:
		delay = dat->period - dat->period / 4 - (70 * get_system_hz() / 1000);
		dat->phase = 0;
		color = 0;
		break;
	}

	led->set(led, color);
	mod_timer(&(dat->timer), jiffies + delay);
}

static void ledtrig_heartbeat_init(struct led_trigger_t * trigger)
{
	struct led_trigger_heartbeat_data_t * dat = (struct led_trigger_heartbeat_data_t *)trigger->priv;

	if(dat)
	{
		setup_timer(&dat->timer, heartbeat_timer_function, (u32_t)trigger);
		dat->phase = 0;
		heartbeat_timer_function(dat->timer.data);
	}
}

static void ledtrig_heartbeat_exit(struct led_trigger_t * trigger)
{
	struct led_trigger_heartbeat_data_t * dat = (struct led_trigger_heartbeat_data_t *)trigger->priv;

	if(dat)
		del_timer(&(dat->timer));
}

static void ledtrig_heartbeat_activity(struct led_trigger_t * trigger)
{
}

static bool_t ledtrig_register_heartbeat(struct resource_t * res)
{
	struct led_trigger_data_t * rdat = (struct led_trigger_data_t *)res->data;
	struct led_trigger_heartbeat_data_t * dat;
	struct led_trigger_t * trigger;
	struct led_t * led;
	char name[64];

	led = search_led(rdat->led);
	if(!led)
		return FALSE;

	dat = malloc(sizeof(struct led_trigger_heartbeat_data_t));
	if(!dat)
		return FALSE;

	trigger = malloc(sizeof(struct led_trigger_t));
	if(!trigger)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%s", res->name, rdat->name);

	dat->phase = 0;
	dat->period = 0;
	dat->rdat = rdat;

	trigger->name = strdup(name);
	trigger->init = ledtrig_heartbeat_init;
	trigger->exit = ledtrig_heartbeat_exit;
	trigger->activity = ledtrig_heartbeat_activity;
	trigger->led = led;
	trigger->priv = dat;

	if(register_led_trigger(trigger))
		return TRUE;

	free(dat);
	free(trigger->name);
	free(trigger);
	return FALSE;
}

static bool_t ledtrig_unregister_heartbeat(struct resource_t * res)
{
	struct led_trigger_data_t * rdat = (struct led_trigger_data_t *)res->data;
	struct led_trigger_t * trigger;
	char name[64];

	snprintf(name, sizeof(name), "%s.%s", res->name, rdat->name);

	trigger = search_led_trigger(name);
	if(!trigger)
		return FALSE;

	if(!unregister_led_trigger(trigger))
		return FALSE;

	free(trigger->priv);
	free(trigger->name);
	free(trigger);
	return TRUE;
}

static __init void ledtrig_heartbeat_device_init(void)
{
	resource_callback_with_name("ledtrig.heartbeat", ledtrig_register_heartbeat);
}

static __exit void ledtrig_heartbeat_device_exit(void)
{
	resource_callback_with_name("ledtrig.heartbeat", ledtrig_unregister_heartbeat);
}

xxx1_initcall(ledtrig_heartbeat_device_init);
xxx1_exitcall(ledtrig_heartbeat_device_exit);
