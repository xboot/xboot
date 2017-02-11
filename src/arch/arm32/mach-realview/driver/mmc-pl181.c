/*
 * driver/mmc-pl181.c
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
#include <mmc/mmc.h>

struct mmc_pl181_pdata_t {
	virtual_addr_t virt;
};

static void mmc_pl181_reset(struct mmc_t * mmc)
{
}

static bool_t mmc_pl181_getcd(struct mmc_t * mmc)
{
	return TRUE;
}

static bool_t mmc_pl181_getwp(struct mmc_t * mmc)
{
	return FALSE;
}

static bool_t mmc_pl181_setios(struct mmc_t * mmc, struct mmc_ios_t * ios)
{
	return TRUE;
}

static bool_t mmc_pl181_request(struct mmc_t * mmc, struct mmc_cmd_t * cmd, struct mmc_data_t * dat)
{
	struct mmc_pl181_pdata_t * pdat = (struct mmc_pl181_pdata_t *)mmc->priv;
	return TRUE;
}

static struct device_t * mmc_pl181_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct mmc_pl181_pdata_t * pdat;
	struct mmc_t * mmc;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x181)
		return NULL;

	pdat = malloc(sizeof(struct mmc_pl181_pdata_t));
	if(!pdat)
		return FALSE;

	mmc = malloc(sizeof(struct mmc_t));
	if(!mmc)
	{
		free(pdat);
		return FALSE;
	}

	pdat->virt = virt;

	mmc->name = alloc_device_name(dt_read_name(n), -1);
	mmc->reset = mmc_pl181_reset;
	mmc->getcd = mmc_pl181_getcd;
	mmc->getwp = mmc_pl181_getwp;
	mmc->setios = mmc_pl181_setios;
	mmc->request = mmc_pl181_request;
	mmc->priv = pdat;

	if(!register_mmc(&dev, mmc))
	{
		free_device_name(mmc->name);
		free(mmc->priv);
		free(mmc);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void mmc_pl181_remove(struct device_t * dev)
{
	struct mmc_t * mmc = (struct mmc_t *)dev->priv;

	if(mmc && unregister_mmc(mmc))
	{
		free_device_name(mmc->name);
		free(mmc->priv);
		free(mmc);
	}
}

static void mmc_pl181_suspend(struct device_t * dev)
{
}

static void mmc_pl181_resume(struct device_t * dev)
{
}

static struct driver_t mmc_pl181 = {
	.name		= "mmc-pl181",
	.probe		= mmc_pl181_probe,
	.remove		= mmc_pl181_remove,
	.suspend	= mmc_pl181_suspend,
	.resume		= mmc_pl181_resume,
};

static __init void mmc_pl181_driver_init(void)
{
	register_driver(&mmc_pl181);
}

static __exit void mmc_pl181_driver_exit(void)
{
	unregister_driver(&mmc_pl181);
}

driver_initcall(mmc_pl181_driver_init);
driver_exitcall(mmc_pl181_driver_exit);
