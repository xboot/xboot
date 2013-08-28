/*
 * drivers/led/ledtrig.c
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

static void led_trigger_suspend(struct device_t * dev)
{
}

static void led_trigger_resume(struct device_t * dev)
{
}

struct led_trigger_t * search_led_trigger(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_LED_TRIGGER);
	if(!dev)
		return NULL;

	return (struct led_trigger_t *)dev->driver;
}

bool_t register_led_trigger(struct led_trigger_t * trigger)
{
	struct device_t * dev;

	if(!trigger || !trigger->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(trigger->name);
	dev->type = DEVICE_TYPE_LED_TRIGGER;
	dev->suspend = led_trigger_suspend;
	dev->resume = led_trigger_resume;
	dev->driver = trigger;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(trigger->init)
		(trigger->init)(trigger);

	return TRUE;
}

bool_t unregister_led_trigger(struct led_trigger_t * trigger)
{
	struct device_t * dev;
	struct led_trigger_t * driver;

	if(!trigger || !trigger->name)
		return FALSE;

	dev = search_device_with_type(trigger->name, DEVICE_TYPE_LED_TRIGGER);
	if(!dev)
		return FALSE;

	driver = (struct led_trigger_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)(trigger);

	if(!unregister_device(dev))
		return FALSE;

	free(dev->name);
	free(dev);
	return TRUE;
}

void led_trigger_activity(struct led_trigger_t * trigger)
{
	if(trigger && trigger->activity)
		trigger->activity(trigger);
}
