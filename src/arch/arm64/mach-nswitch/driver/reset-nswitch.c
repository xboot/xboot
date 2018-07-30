/*
 * driver/reset-nswitch.c
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

enum {
	RESET_SET	= 0x0,
	RESET_CLR	= 0x4,
};

struct reset_nswitch_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nreset;
};

static u32_t nswitch_read_chipid(void)
{
	virtual_addr_t virt = phys_to_virt(0x70000800);
	return read32(virt + 0x4);
}

static void reset_nswitch_assert(struct resetchip_t * chip, int offset)
{
	struct reset_nswitch_pdata_t * pdat = (struct reset_nswitch_pdata_t *)chip->priv;

	if(offset >= chip->nreset)
		return;
	nswitch_read_chipid();
	write32(pdat->virt + RESET_SET, 1 << offset);
}

static void reset_nswitch_deassert(struct resetchip_t * chip, int offset)
{
	struct reset_nswitch_pdata_t * pdat = (struct reset_nswitch_pdata_t *)chip->priv;

	if(offset >= chip->nreset)
		return;
	write32(pdat->virt + RESET_CLR, 1 << offset);
}

static struct device_t * reset_nswitch_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct reset_nswitch_pdata_t * pdat;
	struct resetchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "reset-base", -1);
	int nreset = dt_read_int(n, "reset-count", -1);

	if((base < 0) || (nreset <= 0))
		return NULL;

	pdat = malloc(sizeof(struct reset_nswitch_pdata_t));
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
	chip->assert = reset_nswitch_assert;
	chip->deassert = reset_nswitch_deassert;
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

static void reset_nswitch_remove(struct device_t * dev)
{
	struct resetchip_t * chip = (struct resetchip_t *)dev->priv;

	if(chip && unregister_resetchip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void reset_nswitch_suspend(struct device_t * dev)
{
}

static void reset_nswitch_resume(struct device_t * dev)
{
}

static struct driver_t reset_nswitch = {
	.name		= "reset-nswitch",
	.probe		= reset_nswitch_probe,
	.remove		= reset_nswitch_remove,
	.suspend	= reset_nswitch_suspend,
	.resume		= reset_nswitch_resume,
};

static __init void reset_nswitch_driver_init(void)
{
	register_driver(&reset_nswitch);
}

static __exit void reset_nswitch_driver_exit(void)
{
	unregister_driver(&reset_nswitch);
}

driver_initcall(reset_nswitch_driver_init);
driver_exitcall(reset_nswitch_driver_exit);
