/*
 * driver/adc/adc.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <adc/adc.h>

static ssize_t adc_read_vreference(struct kobj_t * kobj, void * buf, size_t size)
{
	struct adc_t * adc = (struct adc_t *)kobj->priv;
	return sprintf(buf, "%Ld.%06LdV", adc->vreference / (u64_t)(1000 * 1000), adc->vreference % (u64_t)(1000 * 1000));
}

static ssize_t adc_read_resolution(struct kobj_t * kobj, void * buf, size_t size)
{
	struct adc_t * adc = (struct adc_t *)kobj->priv;
	return sprintf(buf, "%d", adc->resolution);
}

static ssize_t adc_read_nchannel(struct kobj_t * kobj, void * buf, size_t size)
{
	struct adc_t * adc = (struct adc_t *)kobj->priv;
	return sprintf(buf, "%d", adc->nchannel);
}

static ssize_t adc_read_raw_channel(struct kobj_t * kobj, void * buf, size_t size)
{
	struct adc_t * adc = (struct adc_t *)kobj->priv;
	int channel = strtoul(kobj->name + strlen("raw"), NULL, 0);
	return sprintf(buf, "%d", adc_read_raw(adc, channel));
}

static ssize_t adc_read_voltage_channel(struct kobj_t * kobj, void * buf, size_t size)
{
	struct adc_t * adc = (struct adc_t *)kobj->priv;
	int channel = strtoul(kobj->name + strlen("voltage"), NULL, 0);
	int voltage = adc_read_voltage(adc, channel);
	return sprintf(buf, "%Ld.%06LdV", voltage / (u64_t)(1000 * 1000), voltage % (u64_t)(1000 * 1000));
}

struct adc_t * search_adc(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_ADC);
	if(!dev)
		return NULL;

	return (struct adc_t *)dev->priv;
}

bool_t register_adc(struct device_t ** device, struct adc_t * adc)
{
	struct device_t * dev;
	char buf[64];
	int i;

	if(!adc || !adc->name || (adc->resolution <= 0) || (adc->nchannel <= 0) || !adc->read)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(adc->name);
	dev->type = DEVICE_TYPE_ADC;
	dev->priv = adc;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "vreference", adc_read_vreference, NULL, adc);
	kobj_add_regular(dev->kobj, "resolution", adc_read_resolution, NULL, adc);
	kobj_add_regular(dev->kobj, "nchannel", adc_read_nchannel, NULL, adc);
	for(i = 0; i< adc->nchannel; i++)
	{
		sprintf(buf, "raw%d", i);
		kobj_add_regular(dev->kobj, buf, adc_read_raw_channel, NULL, adc);
	}
	for(i = 0; i< adc->nchannel; i++)
	{
		sprintf(buf, "voltage%d", i);
		kobj_add_regular(dev->kobj, buf, adc_read_voltage_channel, NULL, adc);
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

bool_t unregister_adc(struct adc_t * adc)
{
	struct device_t * dev;

	if(!adc || !adc->name)
		return FALSE;

	dev = search_device(adc->name, DEVICE_TYPE_ADC);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

u32_t adc_read_raw(struct adc_t * adc, int channel)
{
	if(adc && adc->read)
	{
		if(channel < 0)
			channel = 0;
		else if(channel > adc->nchannel - 1)
			channel = adc->nchannel - 1;
		return adc->read(adc, channel);
	}
	return 0;
}

int adc_read_voltage(struct adc_t * adc, int channel)
{
	if(adc && adc->read)
	{
		if(channel < 0)
			channel = 0;
		else if(channel > adc->nchannel - 1)
			channel = adc->nchannel - 1;
		return ((s64_t)adc->read(adc, channel) * adc->vreference) / ((1 << adc->resolution) - 1);
	}
	return 0;
}
