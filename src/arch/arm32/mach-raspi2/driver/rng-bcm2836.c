/*
 * driver/rng-bcm2836.c
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
#include <rng/rng.h>

/*
 * RNG BCM2835 - Hardware Random Number Generator (RNG)
 *
 * Example:
 *   "rng-bcm2836@0x3f104000": {
 *   }
 */

enum {
	RNG_CTRL 	= 0x00,
	RNG_STATUS 	= 0x04,
	RNG_DATA 	= 0x08,
};

struct rng_bcm2836_pdata_t {
	virtual_addr_t virt;
};

static int rng_bcm2836_read(struct rng_t * rng, void * buf, int max, int wait)
{
	struct rng_bcm2836_pdata_t * pdat = (struct rng_bcm2836_pdata_t *)rng->priv;
	int len = 0, n;
	u32_t val;

	while(len < max)
	{
		while((read32(pdat->virt + RNG_STATUS) >> 24) == 0)
		{
			if(!wait)
				return len;
		}
		val = read32(pdat->virt + RNG_DATA);

		if(max - len < 4)
			n = max - len;
		else
			n = 4;
		memcpy((char *)buf + len, &val, n);
		len += n;
	}

	return len;
}

static struct device_t * rng_bcm2836_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct rng_bcm2836_pdata_t * pdat;
	struct rng_t * rng;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));

	pdat = malloc(sizeof(struct rng_bcm2836_pdata_t));
	if(!pdat)
		return NULL;

	rng = malloc(sizeof(struct rng_t));
	if(!rng)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;

	rng->name = alloc_device_name(dt_read_name(n), -1);
	rng->read = rng_bcm2836_read;
	rng->priv = pdat;

	write32(pdat->virt + RNG_STATUS, 0x40000);
	write32(pdat->virt + RNG_CTRL, 0x1);

	if(!register_rng(&dev, rng))
	{
		free_device_name(rng->name);
		free(rng->priv);
		free(rng);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void rng_bcm2836_remove(struct device_t * dev)
{
	struct rng_t * rng = (struct rng_t *)dev->priv;
	struct rng_bcm2836_pdata_t * pdat = (struct rng_bcm2836_pdata_t *)rng->priv;

	if(rng && unregister_rng(rng))
	{
		write32(pdat->virt + RNG_CTRL, 0);

		free_device_name(rng->name);
		free(rng->priv);
		free(rng);
	}
}

static void rng_bcm2836_suspend(struct device_t * dev)
{
}

static void rng_bcm2836_resume(struct device_t * dev)
{
}

static struct driver_t rng_bcm2836 = {
	.name		= "rng-bcm2836",
	.probe		= rng_bcm2836_probe,
	.remove		= rng_bcm2836_remove,
	.suspend	= rng_bcm2836_suspend,
	.resume		= rng_bcm2836_resume,
};

static __init void rng_bcm2836_driver_init(void)
{
	register_driver(&rng_bcm2836);
}

static __exit void rng_bcm2836_driver_exit(void)
{
	unregister_driver(&rng_bcm2836);
}

driver_initcall(rng_bcm2836_driver_init);
driver_exitcall(rng_bcm2836_driver_exit);
