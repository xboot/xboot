/*
 * driver/battery/battery.c
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

#include <battery/battery.h>

static ssize_t battery_read_supply(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%s", power_supply_string(info.supply));
	return sprintf(buf, "unknown");
}

static ssize_t battery_read_status(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%s", battery_status_string(info.status));
	return sprintf(buf, "unknown");
}

static ssize_t battery_read_health(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%s", battery_health_string(info.health));
	return sprintf(buf, "unknown");
}

static ssize_t battery_read_count(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d", info.count);
	return sprintf(buf, "%d", 0);
}

static ssize_t battery_read_capacity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d", info.capacity);
	return sprintf(buf, "%d", 0);
}

static ssize_t battery_read_voltage(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d", info.voltage);
	return sprintf(buf, "%d", 0);
}

static ssize_t battery_read_current(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d", info.current);
	return sprintf(buf, "%d", 0);
}

static ssize_t battery_read_temperature(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d", info.temperature);
	return sprintf(buf, "%d", 0);
}

static ssize_t battery_read_level(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d", info.level);
	return sprintf(buf, "%d", 0);
}

struct battery_t * search_battery(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_BATTERY);
	if(!dev)
		return NULL;

	return (struct battery_t *)dev->priv;
}

struct battery_t * search_first_battery(void)
{
	struct device_t * dev;

	dev = search_first_device(DEVICE_TYPE_BATTERY);
	if(!dev)
		return NULL;

	return (struct battery_t *)dev->priv;
}

bool_t register_battery(struct device_t ** device, struct battery_t * bat)
{
	struct device_t * dev;

	if(!bat || !bat->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(bat->name);
	dev->type = DEVICE_TYPE_BATTERY;
	dev->priv = bat;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "supply", battery_read_supply, NULL, bat);
	kobj_add_regular(dev->kobj, "status", battery_read_status, NULL, bat);
	kobj_add_regular(dev->kobj, "health", battery_read_health, NULL, bat);
	kobj_add_regular(dev->kobj, "count", battery_read_count, NULL, bat);
	kobj_add_regular(dev->kobj, "capacity", battery_read_capacity, NULL, bat);
	kobj_add_regular(dev->kobj, "voltage", battery_read_voltage, NULL, bat);
	kobj_add_regular(dev->kobj, "current", battery_read_current, NULL, bat);
	kobj_add_regular(dev->kobj, "temperature", battery_read_temperature, NULL, bat);
	kobj_add_regular(dev->kobj, "level", battery_read_level, NULL, bat);

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

bool_t unregister_battery(struct battery_t * bat)
{
	struct device_t * dev;

	if(!bat || !bat->name)
		return FALSE;

	dev = search_device(bat->name, DEVICE_TYPE_BATTERY);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

char * power_supply_string(enum power_supply_t supply)
{
	switch(supply)
	{
	case POWER_SUPPLAY_BATTERY:
		return "battery";
	case POWER_SUPPLAY_AC:
		return "ac";
	case POWER_SUPPLAY_USB:
		return "usb";
	case POWER_SUPPLAY_WIRELESS:
		return "wireless";
	default:
		break;
	}
	return "unknown";
}

char * battery_status_string(enum battery_status_t status)
{
	switch(status)
	{
	case BATTERY_STATUS_DISCHARGING:
		return "discharging";
	case BATTERY_STATUS_CHARGING:
		return "charging";
	case BATTERY_STATUS_EMPTY:
		return "empty";
	case BATTERY_STATUS_FULL:
		return "full";
	default:
		break;
	}
	return "unknown";
}

char * battery_health_string(enum battery_health_t health)
{
	switch(health)
	{
	case BATTERY_HEALTH_GOOD:
		return "good";
	case BATTERY_HEALTH_DEAD:
		return "dead";
	case BATTERY_HEALTH_OVERHEAT:
		return "overheat";
	case BATTERY_HEALTH_OVERVOLTAGE:
		return "overvoltage";
	default:
		break;
	}
	return "unknown";
}

bool_t battery_update(struct battery_t * bat, struct battery_info_t * info)
{
	if(bat && bat->update)
		return bat->update(bat, info);
	return FALSE;
}
