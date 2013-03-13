/*
 * drivers/led/led-file.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <time/tick.h>
#include <time/timer.h>
#include <led/trigger.h>

static bool_t valid = FALSE;
static u32_t activity;
static u32_t last_activity;
static struct timer_list file_trigger_timer;

void led_file_trigger_activity(void)
{
	if(valid)
	{
		activity++;
		if(!timer_pending(&file_trigger_timer))
			mod_timer(&file_trigger_timer, jiffies + 1);
	}
}

static void file_trigger_function(u32_t data)
{
	struct trigger * trigger = (struct trigger *)(data);
	struct led * led = (struct led *)(trigger->led);

	if(last_activity != activity)
	{
		last_activity = activity;
		led->set(LED_BRIGHTNESS_FULL);
		mod_timer(&file_trigger_timer, jiffies + 1);
	}
	else
	{
		led->set(LED_BRIGHTNESS_OFF);
	}
}

static struct trigger file_trigger = {
	.name     		= "led-file",
	.activate 		= NULL,
	.deactivate		= NULL,
	.led			= NULL,
	.priv			= NULL,
};

static __init void file_trigger_init(void)
{
	struct led * led;

	led = (struct led *)resource_get_data(file_trigger.name);
	if(led && led->set)
	{
		if(led->init)
			(led->init)();

		file_trigger.led = led;
		if(trigger_register(&file_trigger))
		{
			setup_timer(&file_trigger_timer, file_trigger_function, (u32_t)(&file_trigger));
			valid = TRUE;
			return;
		}
	}

	valid = FALSE;
}

static __exit void file_trigger_exit(void)
{
	trigger_unregister(&file_trigger);
}

device_initcall(file_trigger_init);
device_exitcall(file_trigger_exit);
