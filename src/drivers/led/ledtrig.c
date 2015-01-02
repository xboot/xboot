/*
 * drivers/led/ledtrig.c
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

static void ledtrig_suspend(struct device_t * dev)
{
}

static void ledtrig_resume(struct device_t * dev)
{
}

static ssize_t ledtrig_read_bind_led_name(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledtrig_t * trigger = (struct ledtrig_t *)kobj->priv;
	return sprintf(buf, "%s", trigger->led->name);
}

static ssize_t ledtrig_write_activity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledtrig_t * trigger = (struct ledtrig_t *)kobj->priv;

	ledtrig_activity(trigger);
	return size;
}

struct ledtrig_t * search_ledtrig(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_LEDTRIG);
	if(!dev)
		return NULL;

	return (struct ledtrig_t *)dev->driver;
}

bool_t register_ledtrig(struct ledtrig_t * trigger)
{
	struct device_t * dev;

	if(!trigger || !trigger->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(trigger->name);
	dev->type = DEVICE_TYPE_LEDTRIG;
	dev->suspend = ledtrig_suspend;
	dev->resume = ledtrig_resume;
	dev->driver = trigger;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "led", ledtrig_read_bind_led_name, NULL, trigger);
	kobj_add_regular(dev->kobj, "activity", NULL, ledtrig_write_activity, trigger);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(trigger->init)
		(trigger->init)(trigger);

	return TRUE;
}

bool_t unregister_ledtrig(struct ledtrig_t * trigger)
{
	struct device_t * dev;
	struct ledtrig_t * driver;

	if(!trigger || !trigger->name)
		return FALSE;

	dev = search_device_with_type(trigger->name, DEVICE_TYPE_LEDTRIG);
	if(!dev)
		return FALSE;

	driver = (struct ledtrig_t *)(dev->driver);
	if(driver && driver->exit)
		(driver->exit)(trigger);

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void ledtrig_activity(struct ledtrig_t * trigger)
{
	if(trigger && trigger->activity)
		trigger->activity(trigger);
}
