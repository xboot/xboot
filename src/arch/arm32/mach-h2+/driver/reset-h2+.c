/*
 * driver/reset-h3.c
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
#include <reset/reset.h>

struct reset_h3_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nreset;
};

static void reset_h3_assert(struct resetchip_t * chip, int offset)
{
	struct reset_h3_pdata_t * pdat = (struct reset_h3_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->nreset)
		return;

	val = read32(pdat->virt);
	val &= ~(1 << offset);
	write32(pdat->virt, val);
}

static void reset_h3_deassert(struct resetchip_t * chip, int offset)
{
	struct reset_h3_pdata_t * pdat = (struct reset_h3_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->nreset)
		return;

	val = read32(pdat->virt);
	val |= (1 << offset);
	write32(pdat->virt, val);
}

static struct device_t * reset_h3_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct reset_h3_pdata_t * pdat;
	struct resetchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "reset-base", -1);
	int nreset = dt_read_int(n, "reset-count", -1);

	if((base < 0) || (nreset <= 0))
		return NULL;

	pdat = malloc(sizeof(struct reset_h3_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct resetchip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->base = base;
	pdat->nreset = nreset;

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nreset = pdat->nreset;
	chip->assert = reset_h3_assert;
	chip->deassert = reset_h3_deassert;
	chip->priv = pdat;

	if(!register_resetchip(&dev, chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void reset_h3_remove(struct device_t * dev)
{
	struct resetchip_t * chip = (struct resetchip_t *)dev->priv;

	if(chip && unregister_resetchip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void reset_h3_suspend(struct device_t * dev)
{
}

static void reset_h3_resume(struct device_t * dev)
{
}

static struct driver_t reset_h3 = {
	.name		= "reset-h3",
	.probe		= reset_h3_probe,
	.remove		= reset_h3_remove,
	.suspend	= reset_h3_suspend,
	.resume		= reset_h3_resume,
};

static __init void reset_h3_driver_init(void)
{
	register_driver(&reset_h3);
}

static __exit void reset_h3_driver_exit(void)
{
	unregister_driver(&reset_h3);
}

driver_initcall(reset_h3_driver_init);
driver_exitcall(reset_h3_driver_exit);
