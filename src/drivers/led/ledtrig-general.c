/*
 * driver/led/ledtrig-general.c
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

#include <xboot.h>
#include <led/ledtrig.h>

struct ledtrig_general_data_t {
	struct timer_t timer;
	u32_t activity;
	u32_t last_activity;

	struct ledtrig_data_t * rdat;
};

static void general_timer_function(u32_t data)
{
	struct ledtrig_t * trigger = (struct ledtrig_t *)(data);
	struct ledtrig_general_data_t * dat = (struct ledtrig_general_data_t *)trigger->priv;
	struct led_t * led = (struct led_t *)(trigger->led);

	if(dat->last_activity != dat->activity)
	{
		dat->last_activity = dat->activity;
		led_set_brightness(led, CONFIG_MAX_BRIGHTNESS);
		mod_timer(&(dat->timer), jiffies + 1);
	}
	else
	{
		led_set_brightness(led, 0);
	}
}

static void ledtrig_general_init(struct ledtrig_t * trigger)
{
	struct ledtrig_general_data_t * dat = (struct ledtrig_general_data_t *)trigger->priv;

	if(dat)
	{
		setup_timer(&dat->timer, general_timer_function, (u32_t)trigger);
		dat->activity = 0;
		dat->last_activity = 0;
	}
}

static void ledtrig_general_exit(struct ledtrig_t * trigger)
{
	struct ledtrig_general_data_t * dat = (struct ledtrig_general_data_t *)trigger->priv;

	if(dat)
		del_timer(&(dat->timer));
}

static void ledtrig_general_activity(struct ledtrig_t * trigger)
{
	struct ledtrig_general_data_t * dat = (struct ledtrig_general_data_t *)trigger->priv;

	dat->activity++;
	if(!timer_pending(&(dat->timer)))
		mod_timer(&(dat->timer), jiffies + 1);
}

static bool_t ledtrig_register_general(struct resource_t * res)
{
	struct ledtrig_data_t * rdat = (struct ledtrig_data_t *)res->data;
	struct ledtrig_general_data_t * dat;
	struct ledtrig_t * trigger;
	struct led_t * led;
	char name[64];

	led = search_led(rdat->led);
	if(!led)
		return FALSE;

	dat = malloc(sizeof(struct ledtrig_general_data_t));
	if(!dat)
		return FALSE;

	trigger = malloc(sizeof(struct ledtrig_t));
	if(!trigger)
	{
		free(dat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	dat->activity = 0;
	dat->last_activity = 0;
	dat->rdat = rdat;

	trigger->name = strdup(name);
	trigger->init = ledtrig_general_init;
	trigger->exit = ledtrig_general_exit;
	trigger->activity = ledtrig_general_activity;
	trigger->led = led;
	trigger->priv = dat;

	if(register_ledtrig(trigger))
		return TRUE;

	free(trigger->priv);
	free(trigger->name);
	free(trigger);
	return FALSE;
}

static bool_t ledtrig_unregister_general(struct resource_t * res)
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

static __init void ledtrig_general_device_init(void)
{
	resource_for_each_with_name("ledtrig-general", ledtrig_register_general);
}

static __exit void ledtrig_general_device_exit(void)
{
	resource_for_each_with_name("ledtrig-general", ledtrig_unregister_general);
}

postdevice_initcall(ledtrig_general_device_init);
postdevice_exitcall(ledtrig_general_device_exit);
