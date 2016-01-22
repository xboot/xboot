/*
 * drivers/watchdog/watchdog.c
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

#include <xboot.h>
#include <watchdog/watchdog.h>

static void watchdog_suspend(struct device_t * dev)
{
	struct watchdog_t * watchdog;

	if(!dev || dev->type != DEVICE_TYPE_WATCHDOG)
		return;

	watchdog = (struct watchdog_t *)(dev->driver);
	if(!watchdog)
		return;

	if(watchdog->suspend)
		watchdog->suspend(watchdog);
}

static void watchdog_resume(struct device_t * dev)
{
	struct watchdog_t * watchdog;

	if(!dev || dev->type != DEVICE_TYPE_WATCHDOG)
		return;

	watchdog = (struct watchdog_t *)(dev->driver);
	if(!watchdog)
		return;

	if(watchdog->resume)
		watchdog->resume(watchdog);
}

static ssize_t watchdog_read_timeout(struct kobj_t * kobj, void * buf, size_t size)
{
	struct watchdog_t * watchdog = (struct watchdog_t *)kobj->priv;
	int timeout;

	timeout = watchdog_get_timeout(watchdog);
	return sprintf(buf, "%d", timeout);
}

static ssize_t watchdog_write_timeout(struct kobj_t * kobj, void * buf, size_t size)
{
	struct watchdog_t * watchdog = (struct watchdog_t *)kobj->priv;
	int timeout = strtol(buf, NULL, 0);

	watchdog_set_timeout(watchdog, timeout);
	return size;
}

struct watchdog_t * search_watchdog(const char * name)
{
	struct device_t * dev;

	dev = search_device_with_type(name, DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return NULL;

	return (struct watchdog_t *)dev->driver;
}

struct watchdog_t * search_first_watchdog(void)
{
	struct device_t * dev;

	dev = search_first_device_with_type(DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return NULL;

	return (struct watchdog_t *)dev->driver;
}

bool_t register_watchdog(struct watchdog_t * watchdog)
{
	struct device_t * dev;

	if(!watchdog || !watchdog->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(watchdog->name);
	dev->type = DEVICE_TYPE_WATCHDOG;
	dev->suspend = watchdog_suspend;
	dev->resume = watchdog_resume;
	dev->driver = watchdog;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "timeout", watchdog_read_timeout, watchdog_write_timeout, watchdog);

	if(watchdog->set)
		(watchdog->set)(watchdog, 0);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	return TRUE;
}

bool_t unregister_watchdog(struct watchdog_t * watchdog)
{
	struct device_t * dev;
	struct watchdog_t * driver;

	if(!watchdog || !watchdog->name)
		return FALSE;

	dev = search_device_with_type(watchdog->name, DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	driver = (struct watchdog_t *)(dev->driver);
	if(driver && driver->set)
		(driver->set)(watchdog, 0);

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void watchdog_set_timeout(struct watchdog_t * watchdog, int timeout)
{
	if(watchdog && watchdog->set)
	{
		if(timeout < 0)
			timeout = 0;
		watchdog->set(watchdog, timeout);
	}
}

int watchdog_get_timeout(struct watchdog_t * watchdog)
{
	if(watchdog && watchdog->get)
		return watchdog->get(watchdog);
	return 0;
}
