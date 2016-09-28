/*
 * driver/dac/dac.c
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
#include <dac/dac.h>

static ssize_t dac_read_vreference(struct kobj_t * kobj, void * buf, size_t size)
{
	struct dac_t * dac = (struct dac_t *)kobj->priv;
	return sprintf(buf, "%Ld.%06LdV", dac->vreference / (u64_t)(1000 * 1000), dac->vreference % (u64_t)(1000 * 1000));
}

static ssize_t dac_read_resolution(struct kobj_t * kobj, void * buf, size_t size)
{
	struct dac_t * dac = (struct dac_t *)kobj->priv;
	return sprintf(buf, "%d", dac->resolution);
}

static ssize_t dac_read_nchannel(struct kobj_t * kobj, void * buf, size_t size)
{
	struct dac_t * dac = (struct dac_t *)kobj->priv;
	return sprintf(buf, "%d", dac->nchannel);
}

static ssize_t dac_write_raw_channel(struct kobj_t * kobj, void * buf, size_t size)
{
	struct dac_t * dac = (struct dac_t *)kobj->priv;
	int channel = strtoul(kobj->name + strlen("raw"), NULL, 0);
	u32_t value = strtol(buf, NULL, 0);
	dac_write_raw(dac, channel, value);
	return size;
}

static ssize_t dac_write_voltage_channel(struct kobj_t * kobj, void * buf, size_t size)
{
	struct dac_t * dac = (struct dac_t *)kobj->priv;
	int channel = strtoul(kobj->name + strlen("voltage"), NULL, 0);
	int voltage = strtol(buf, NULL, 0);
	dac_write_voltage(dac, channel, voltage);
	return size;
}

struct dac_t * search_dac(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_DAC);
	if(!dev)
		return NULL;

	return (struct dac_t *)dev->priv;
}

bool_t register_dac(struct device_t ** device, struct dac_t * dac)
{
	struct device_t * dev;
	char buf[64];
	int i;

	if(!dac || !dac->name || (dac->resolution <= 0) || (dac->nchannel <= 0) || !dac->write)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(dac->name);
	dev->type = DEVICE_TYPE_DAC;
	dev->priv = dac;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "vreference", dac_read_vreference, NULL, dac);
	kobj_add_regular(dev->kobj, "resolution", dac_read_resolution, NULL, dac);
	kobj_add_regular(dev->kobj, "nchannel", dac_read_nchannel, NULL, dac);
	for(i = 0; i< dac->nchannel; i++)
	{
		sprintf(buf, "raw%d", i);
		kobj_add_regular(dev->kobj, buf, NULL, dac_write_raw_channel, dac);
	}
	for(i = 0; i< dac->nchannel; i++)
	{
		sprintf(buf, "voltage%d", i);
		kobj_add_regular(dev->kobj, buf, NULL, dac_write_voltage_channel, dac);
	}

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

bool_t unregister_dac(struct dac_t * dac)
{
	struct device_t * dev;

	if(!dac || !dac->name)
		return FALSE;

	dev = search_device(dac->name, DEVICE_TYPE_DAC);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void dac_write_raw(struct dac_t * dac, int channel, u32_t value)
{
	if(dac && dac->write)
	{
		if(channel < 0)
			channel = 0;
		else if(channel > dac->nchannel - 1)
			channel = dac->nchannel - 1;
		dac->write(dac, channel, value);
	}
}

void dac_write_voltage(struct dac_t * dac, int channel, int voltage)
{
	if(dac && dac->write)
	{
		if(channel < 0)
			channel = 0;
		else if(channel > dac->nchannel - 1)
			channel = dac->nchannel - 1;
		if(voltage > dac->vreference)
			voltage = dac->vreference;
		dac->write(dac, channel, voltage * ((1 << dac->resolution) - 1) / dac->vreference);
	}
}
