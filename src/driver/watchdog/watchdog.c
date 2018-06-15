/*
 * driver/watchdog/watchdog.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

static ssize_t watchdog_read_timeout(struct kobj_t * kobj, void * buf, size_t size)
{
	struct watchdog_t * wdg = (struct watchdog_t *)kobj->priv;
	int timeout;

	timeout = watchdog_get_timeout(wdg);
	return sprintf(buf, "%d", timeout);
}

static ssize_t watchdog_write_timeout(struct kobj_t * kobj, void * buf, size_t size)
{
	struct watchdog_t * wdg = (struct watchdog_t *)kobj->priv;
	int timeout = strtol(buf, NULL, 0);

	watchdog_set_timeout(wdg, timeout);
	return size;
}

struct watchdog_t * search_watchdog(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return NULL;
	return (struct watchdog_t *)dev->priv;
}

struct watchdog_t * search_first_watchdog(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return NULL;
	return (struct watchdog_t *)dev->priv;
}

bool_t register_watchdog(struct device_t ** device,struct watchdog_t * wdg)
{
	struct device_t * dev;

	if(!wdg || !wdg->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(wdg->name);
	dev->type = DEVICE_TYPE_WATCHDOG;
	dev->priv = wdg;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "timeout", watchdog_read_timeout, watchdog_write_timeout, wdg);

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

bool_t unregister_watchdog(struct watchdog_t * wdg)
{
	struct device_t * dev;

	if(!wdg || !wdg->name)
		return FALSE;

	dev = search_device(wdg->name, DEVICE_TYPE_WATCHDOG);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void watchdog_set_timeout(struct watchdog_t * wdg, int timeout)
{
	if(wdg && wdg->set)
	{
		if(timeout < 0)
			timeout = 0;
		wdg->set(wdg, timeout);
	}
}

int watchdog_get_timeout(struct watchdog_t * wdg)
{
	if(wdg && wdg->get)
		return wdg->get(wdg);
	return 0;
}
