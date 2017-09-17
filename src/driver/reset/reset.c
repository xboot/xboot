/*
 * driver/reset/reset.c
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

bool_t register_resetchip(struct device_t ** device, struct resetchip_t * chip)
{
	struct device_t * dev;

	if(!chip || !chip->name)
		return FALSE;

	if(chip->base < 0 || chip->nreset <= 0)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(chip->name);
	dev->type = DEVICE_TYPE_RESETCHIP;
	dev->priv = chip;
	dev->kobj = kobj_alloc_directory(dev->name);
	kobj_add_regular(dev->kobj, "base", resetchip_read_base, NULL, chip);
	kobj_add_regular(dev->kobj, "nreset", resetchip_read_nreset, NULL, chip);

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

bool_t unregister_resetchip(struct resetchip_t * chip)
{
	struct device_t * dev;

	if(!chip || !chip->name)
		return FALSE;

	if(chip->base < 0 || chip->nreset <= 0)
		return FALSE;

	dev = search_device(chip->name, DEVICE_TYPE_RESETCHIP);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

int reset_is_valid(int reset)
{
	return search_resetchip(reset) ? 1 : 0;
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
