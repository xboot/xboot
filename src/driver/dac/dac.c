/*
 * driver/dac/dac.c
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
