/*
 * drivers/led/ledtrig.c
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

static ssize_t ledtrig_write_activity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct ledtrig_t * ledtrig = (struct ledtrig_t *)kobj->priv;

	ledtrig_activity(ledtrig);
	return size;
}

struct ledtrig_t * search_ledtrig(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_LEDTRIG);
	if(!dev)
		return NULL;

	return (struct ledtrig_t *)dev->priv;
}

bool_t register_ledtrig(struct device_t ** device, struct ledtrig_t * ledtrig)
{
	struct device_t * dev;

	if(!ledtrig || !ledtrig->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(ledtrig->name);
	dev->type = DEVICE_TYPE_LEDTRIG;
	dev->priv = ledtrig;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "activity", NULL, ledtrig_write_activity, ledtrig);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

bool_t unregister_ledtrig(struct ledtrig_t * ledtrig)
{
	struct device_t * dev;

	if(!ledtrig || !ledtrig->name)
		return FALSE;

	dev = search_device_with_type(ledtrig->name, DEVICE_TYPE_LEDTRIG);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void ledtrig_activity(struct ledtrig_t * ledtrig)
{
	if(ledtrig && ledtrig->activity)
		ledtrig->activity(ledtrig);
}
