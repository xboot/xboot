/*
 * driver/reset/reset.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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

#include <reset/reset.h>

static ssize_t resetchip_read_base(struct kobj_t * kobj, void * buf, size_t size)
{
	struct resetchip_t * chip = (struct resetchip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->base);
}

static ssize_t resetchip_read_nreset(struct kobj_t * kobj, void * buf, size_t size)
{
	struct resetchip_t * chip = (struct resetchip_t *)kobj->priv;
	return sprintf(buf, "%d", chip->nreset);
}

struct resetchip_t * search_resetchip(int reset)
{
	struct device_t * pos, * n;
	struct resetchip_t * chip;

	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_RESETCHIP], head)
	{
		chip = (struct resetchip_t *)(pos->priv);
		if((reset >= chip->base) && (reset < (chip->base + chip->nreset)))
			return chip;
	}
	return NULL;
}

struct device_t * register_resetchip(struct resetchip_t * chip, struct driver_t * drv)
{
	struct device_t * dev;

	if(!chip || !chip->name)
		return NULL;

	if(chip->base < 0 || chip->nreset <= 0)
		return NULL;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return NULL;

	dev->name = strdup(chip->name);
	dev->type = DEVICE_TYPE_RESETCHIP;
	dev->driver = drv;
	dev->priv = chip;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "base", resetchip_read_base, NULL, chip);
	kobj_add_regular(dev->kobj, "nreset", resetchip_read_nreset, NULL, chip);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return NULL;
	}
	return dev;
}

void unregister_resetchip(struct resetchip_t * chip)
{
	struct device_t * dev;

	if(chip && chip->name && (chip->base >= 0) && (chip->nreset > 0))
	{
		dev = search_device(chip->name, DEVICE_TYPE_RESETCHIP);
		if(dev && unregister_device(dev))
		{
			kobj_remove_self(dev->kobj);
			free(dev->name);
			free(dev);
		}
	}
}

int reset_is_valid(int rst)
{
	return search_resetchip(rst) ? 1 : 0;
}

void reset_assert(int rst)
{
	struct resetchip_t * chip = search_resetchip(rst);

	#undef assert
	if(chip && chip->assert)
		chip->assert(chip, rst - chip->base);
}

void reset_deassert(int rst)
{
	struct resetchip_t * chip = search_resetchip(rst);

	if(chip && chip->deassert)
		chip->deassert(chip, rst - chip->base);
}

void reset_reset(int rst, int us)
{
	struct resetchip_t * chip = search_resetchip(rst);

	if(chip && chip->assert)
	{
		int offset = rst - chip->base;
		chip->assert(chip, offset);
		udelay(us);
		chip->deassert(chip, offset);
		udelay(us);
	}
}

struct resets_t * resets_alloc(struct dtnode_t * n, const char * name)
{
	int l;

	if(n)
	{
		if(!name)
			name = "resets";
		if((l = dt_read_array_length(n, name)) > 0)
		{
			struct resets_t * rsts = malloc(sizeof(struct resets_t) + l * sizeof(int));
			rsts->n = l;
			for(int i = 0; i < l; i++)
				rsts->rst[i] = dt_read_array_int(n, name, i, -1);
			return rsts;
		}
	}
	return NULL;
}

void resets_free(struct resets_t * rsts)
{
	if(rsts)
		free(rsts);
}

void resets_assert(struct resets_t * rsts)
{
	if(rsts)
	{
		for(int i = 0; i < rsts->n; i++)
			reset_assert(rsts->rst[i]);
	}
}

void resets_deassert(struct resets_t * rsts)
{
	if(rsts)
	{
		for(int i = rsts->n - 1; i >= 0; i--)
			reset_deassert(rsts->rst[i]);
	}
}

void resets_reset(struct resets_t * rsts, int us)
{
	if(rsts)
	{
		for(int i = rsts->n - 1; i >= 0; i--)
			reset_reset(rsts->rst[i], us);
	}
}
