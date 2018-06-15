/*
 * driver/regulator/regulator.c
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
#include <regulator/regulator.h>

static ssize_t regulator_read_summary(struct kobj_t * kobj, void * buf, size_t size)
{
	struct regulator_t * supply = (struct regulator_t *)kobj->priv;
	const char * name = supply->name;
	char * p = buf;
	int len = 0;
	int voltage;

	len += sprintf((char *)(p + len), "%-16s %16s %8s\r\n", "name", "voltage", "enable");
	while(name)
	{
		voltage = regulator_get_voltage(name);
		len += sprintf((char *)(p + len), "%-16s %6Ld.%06LdV %8d\r\n", name, voltage / (u64_t)(1000 * 1000), voltage % (u64_t)(1000 * 1000), regulator_status(name) ? 1 : 0);
		name = regulator_get_parent(name);
	}
	return len;
}

static ssize_t regulator_read_parent(struct kobj_t * kobj, void * buf, size_t size)
{
	struct regulator_t * supply = (struct regulator_t *)kobj->priv;
	const char * parent = regulator_get_parent(supply->name);
	return sprintf(buf, "%s", parent ? parent : "NONE");
}

static ssize_t regulator_write_parent(struct kobj_t * kobj, void * buf, size_t size)
{
	struct regulator_t * supply = (struct regulator_t *)kobj->priv;
	regulator_set_parent(supply->name, buf);
	return size;
}

static ssize_t regulator_read_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct regulator_t * supply = (struct regulator_t *)kobj->priv;
	return sprintf(buf, "%d", regulator_status(supply->name) ? 1 : 0);
}

static ssize_t regulator_write_enable(struct kobj_t * kobj, void * buf, size_t size)
{
	struct regulator_t * supply = (struct regulator_t *)kobj->priv;
	int enable = strtol(buf, NULL, 0);
	if(enable != 0)
		regulator_enable(supply->name);
	else
		regulator_disable(supply->name);
	return size;
}

static ssize_t regulator_read_voltage(struct kobj_t * kobj, void * buf, size_t size)
{
	struct regulator_t * supply = (struct regulator_t *)kobj->priv;
	int voltage = regulator_get_voltage(supply->name);
	return sprintf(buf, "%Ld.%06LdV", voltage / (u64_t)(1000 * 1000), voltage % (u64_t)(1000 * 1000));
}

static ssize_t regulator_write_voltage(struct kobj_t * kobj, void * buf, size_t size)
{
	struct regulator_t * supply = (struct regulator_t *)kobj->priv;
	int voltage = strtoull(buf, NULL, 0);
	regulator_set_voltage(supply->name, voltage);
	return size;
}

struct regulator_t * search_regulator(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_REGULATOR);
	if(!dev)
		return NULL;
	return (struct regulator_t *)dev->priv;
}

bool_t register_regulator(struct device_t ** device, struct regulator_t * supply)
{
	struct device_t * dev;

	if(!supply || !supply->name)
		return FALSE;

	if(search_regulator(supply->name))
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(supply->name);
	dev->type = DEVICE_TYPE_REGULATOR;
	dev->priv = supply;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "summary", regulator_read_summary, NULL, supply);
	kobj_add_regular(dev->kobj, "parent", regulator_read_parent, regulator_write_parent, supply);
	kobj_add_regular(dev->kobj, "enable", regulator_read_enable, regulator_write_enable, supply);
	kobj_add_regular(dev->kobj, "voltage", regulator_read_voltage, regulator_write_voltage, supply);

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

bool_t unregister_regulator(struct regulator_t * supply)
{
	struct device_t * dev;

	if(!supply || !supply->name)
		return FALSE;

	dev = search_device(supply->name, DEVICE_TYPE_REGULATOR);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void regulator_set_parent(const char * name, const char * pname)
{
	struct regulator_t * supply = search_regulator(name);
	struct regulator_t * psupply = search_regulator(pname);

	if(psupply && supply && supply->set_parent)
		supply->set_parent(supply, pname);
}

const char * regulator_get_parent(const char * name)
{
	struct regulator_t * supply = search_regulator(name);

	if(supply && supply->get_parent)
		return supply->get_parent(supply);
	return NULL;
}

void regulator_enable(const char * name)
{
	struct regulator_t * supply = search_regulator(name);

	if(!supply)
		return;

	if(supply->get_parent)
		regulator_enable(supply->get_parent(supply));

	if(supply->set_enable)
		supply->set_enable(supply, TRUE);

	supply->count++;
}

void regulator_disable(const char * name)
{
	struct regulator_t * supply = search_regulator(name);

	if(!supply)
		return;

	if(supply->count > 0)
		supply->count--;

	if(supply->count == 0)
	{
		if(supply->get_parent)
			regulator_disable(supply->get_parent(supply));

		if(supply->set_enable)
			supply->set_enable(supply, FALSE);
	}
}

bool_t regulator_status(const char * name)
{
	struct regulator_t * supply = search_regulator(name);

	if(!supply)
		return FALSE;

	if(!supply->get_parent(supply))
		return supply->get_enable(supply);

	if(supply->get_enable(supply))
		return regulator_status(supply->get_parent(supply));

	return FALSE;
}

void regulator_set_voltage(const char * name, int voltage)
{
	struct regulator_t * supply = search_regulator(name);

	if(supply && supply->set_voltage)
		supply->set_voltage(supply, voltage);
}

int regulator_get_voltage(const char * name)
{
	struct regulator_t * supply = search_regulator(name);

	if(supply && supply->get_parent)
		return supply->get_voltage(supply);
	return 0;
}
