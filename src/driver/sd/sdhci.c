/*
 * driver/sd/sdhci.c
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
#include <sd/sdhci.h>

struct sdhci_t * search_sdhci(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_SDHCI);
	if(!dev)
		return NULL;

	return (struct sdhci_t *)dev->priv;
}

bool_t register_sdhci(struct device_t ** device, struct sdhci_t * sdhci)
{
	struct device_t * dev;

	if(!sdhci || !sdhci->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(sdhci->name);
	dev->type = DEVICE_TYPE_SDHCI;
	dev->priv = sdhci;
	dev->kobj = kobj_alloc_directory(dev->name);

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

bool_t unregister_sdhci(struct sdhci_t * sdhci)
{
	struct device_t * dev;

	if(!sdhci || !sdhci->name)
		return FALSE;

	dev = search_device(sdhci->name, DEVICE_TYPE_SDHCI);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

void sdhci_reset(struct sdhci_t * sdhci)
{
	if(sdhci && sdhci->reset)
		sdhci->reset(sdhci);
}

bool_t sdhci_detect(struct sdhci_t * sdhci)
{
	if(sdhci && sdhci->detect)
		return sdhci->detect(sdhci);
	return FALSE;
}

bool_t sdhci_set_width(struct sdhci_t * sdhci, int width)
{
	if(sdhci && sdhci->setwidth)
		return sdhci->setwidth(sdhci, width);
	return FALSE;
}

bool_t sdhci_set_clock(struct sdhci_t * sdhci, int clock)
{
	if(sdhci && sdhci->setclock)
		return sdhci->setclock(sdhci, clock);
	return FALSE;
}

bool_t sdhci_transfer(struct sdhci_t * sdhci, struct sdhci_cmd_t * cmd, struct sdhci_data_t * dat)
{
	if(sdhci && sdhci->transfer)
		return sdhci->transfer(sdhci, cmd, dat);
	return FALSE;
}
