/*
 * driver/battery/battery.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
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

static ssize_t battery_read_design_capacity(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%dmAh", info.design_capacity);
	return sprintf(buf, "%dmAh", 0);
}

static ssize_t battery_read_design_voltage(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%dmV", info.design_voltage);
	return sprintf(buf, "%dmV", 0);
}

static ssize_t battery_read_voltage(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%dmV", info.voltage);
	return sprintf(buf, "%dmV", 0);
}

static ssize_t battery_read_current(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%dmA", info.current);
	return sprintf(buf, "%dmA", 0);
}

static ssize_t battery_read_temperature(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d.%03dC", info.temperature / 1000, abs(info.temperature % 1000));
	return sprintf(buf, "%d.%03dC", 0, 0);
}

static ssize_t battery_read_cycle(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d", info.cycle);
	return sprintf(buf, "%d", 0);
}

static ssize_t battery_read_level(struct kobj_t * kobj, void * buf, size_t size)
{
	struct battery_t * bat = (struct battery_t *)kobj->priv;
	struct battery_info_t info;

	if(battery_update(bat, &info))
		return sprintf(buf, "%d%%", info.level);
	return sprintf(buf, "%d%%", 0);
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
	kobj_add_regular(dev->kobj, "design-capacity", battery_read_design_capacity, NULL, bat);
	kobj_add_regular(dev->kobj, "design-voltage", battery_read_design_voltage, NULL, bat);
	kobj_add_regular(dev->kobj, "voltage", battery_read_voltage, NULL, bat);
	kobj_add_regular(dev->kobj, "current", battery_read_current, NULL, bat);
	kobj_add_regular(dev->kobj, "temperature", battery_read_temperature, NULL, bat);
	kobj_add_regular(dev->kobj, "cycle", battery_read_cycle, NULL, bat);
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
