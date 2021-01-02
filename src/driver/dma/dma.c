/*
 * driver/dma/dma.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <dma/dma.h>

static ssize_t dmachip_read_base(struct kobj_t * kobj, void * buf, size_t size)
{
	struct dmachip_t * chip = (struct dmachip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->base);
}

static ssize_t dmachip_read_ndma(struct kobj_t * kobj, void * buf, size_t size)
{
	struct dmachip_t * chip = (struct dmachip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->ndma);
}

struct dmachip_t * search_dmachip(int dma)
{
	struct device_t * pos, * n;
	struct dmachip_t * chip;

	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_DMACHIP], head)
	{
		chip = (struct dmachip_t *)(pos->priv);
		if((dma >= chip->base) && (dma < (chip->base + chip->ndma)))
			return chip;
	}
	return NULL;
}

struct device_t * register_dmachip(struct dmachip_t * chip, struct driver_t * drv)
{
	struct device_t * dev;
	irq_flags_t flags;
	int i;

	if(!chip || !chip->name)
		return NULL;

	if(chip->base < 0 || chip->ndma <= 0)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	for(i = 0; i < chip->ndma; i++)
	{
		spin_lock_init(&chip->channel[i].lock);
		spin_lock_irqsave(&chip->channel[i].lock, flags);
		if(chip->stop)
			chip->stop(chip, i);
		chip->channel[i].src = NULL;
		chip->channel[i].dst = NULL;
		chip->channel[i].size = 0;
		chip->channel[i].flag = 0;
		chip->channel[i].len = 0;
		chip->channel[i].data = NULL;
		chip->channel[i].complete = NULL;
		spin_unlock_irqrestore(&chip->channel[i].lock, flags);
	}
	dev->name = strdup(chip->name);
	dev->type = DEVICE_TYPE_DMACHIP;
	dev->driver = drv;
	dev->priv = chip;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "base", dmachip_read_base, NULL, chip);
	kobj_add_regular(dev->kobj, "ndma", dmachip_read_ndma, NULL, chip);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_dmachip(struct dmachip_t * chip)
{
	struct device_t * dev;
	irq_flags_t flags;
	int i;

	if(chip && chip->name && (chip->base >= 0) && (chip->ndma > 0))
	{
		dev = search_device(chip->name, DEVICE_TYPE_DMACHIP);
		if(dev && unregister_device(dev))
		{
			for(i = 0; i < chip->ndma; i++)
			{
				spin_lock_irqsave(&chip->channel[i].lock, flags);
				if(chip->stop)
					chip->stop(chip, i);
				chip->channel[i].src = NULL;
				chip->channel[i].dst = NULL;
				chip->channel[i].size = 0;
				chip->channel[i].flag = 0;
				chip->channel[i].len = 0;
				chip->channel[i].data = NULL;
				chip->channel[i].complete = NULL;
				spin_unlock_irqrestore(&chip->channel[i].lock, flags);
			}
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

bool_t dma_is_valid(int dma)
{
	return search_dmachip(dma) ? TRUE : FALSE;
}

void dma_start(int dma, void * src, void * dst, int size, int flag, void (*complete)(void *), void * data)
{
	struct dmachip_t * chip = search_dmachip(dma);
	irq_flags_t flags;
	int offset;

	if(chip && src && dst && (src != dst) && (size > 0))
	{
		offset = dma - chip->base;
		if(chip->busying)
			while(chip->busying(chip, offset));
		spin_lock_irqsave(&chip->channel[offset].lock, flags);
		chip->channel[offset].src = src;
		chip->channel[offset].dst = dst;
		chip->channel[offset].size = size;
		chip->channel[offset].flag = flag;
		chip->channel[offset].len = 0;
		chip->channel[offset].data = data;
		chip->channel[offset].complete = complete;
		if(chip->start)
			chip->start(chip, offset);
		spin_unlock_irqrestore(&chip->channel[offset].lock, flags);
	}
}

void dma_stop(int dma)
{
	struct dmachip_t * chip = search_dmachip(dma);
	irq_flags_t flags;
	int offset;

	if(chip)
	{
		offset = dma - chip->base;
		spin_lock_irqsave(&chip->channel[offset].lock, flags);
		if(chip->stop)
			chip->stop(chip, offset);
		chip->channel[offset].src = NULL;
		chip->channel[offset].dst = NULL;
		chip->channel[offset].size = 0;
		chip->channel[offset].flag = 0;
		chip->channel[offset].len = 0;
		chip->channel[offset].data = NULL;
		chip->channel[offset].complete = NULL;
		spin_unlock_irqrestore(&chip->channel[offset].lock, flags);
	}
}

void dma_wait(int dma)
{
	struct dmachip_t * chip = search_dmachip(dma);
	int offset;

	if(chip && chip->busying)
	{
		offset = dma - chip->base;
		while(chip->busying(chip, offset))
			task_yield();
	}
}
