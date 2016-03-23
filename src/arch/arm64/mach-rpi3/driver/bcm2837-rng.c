/*
 * driver/bcm2837-rng.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <bcm2837-rng.h>

struct bcm2837_rng_pdata_t {
	virtual_addr_t virt;
};

static void rng_init(struct rng_t * rng)
{
	struct bcm2837_rng_pdata_t * pdat = (struct bcm2837_rng_pdata_t *)rng->priv;
	write32(pdat->virt + RNG_STATUS, 0x40000);
	write32(pdat->virt + RNG_CTRL, 0x1);
}

static void rng_exit(struct rng_t * rng)
{
	struct bcm2837_rng_pdata_t * pdat = (struct bcm2837_rng_pdata_t *)rng->priv;
	write32(pdat->virt + RNG_CTRL, 0);
}

static int rng_read(struct rng_t * rng, void * buf, int max, int wait)
{
	struct bcm2837_rng_pdata_t * pdat = (struct bcm2837_rng_pdata_t *)rng->priv;
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

static void rng_suspend(struct rng_t * rng)
{
}

static void rng_resume(struct rng_t * rng)
{
}

static bool_t bcm2837_register_rng(struct resource_t * res)
{
	struct bcm2837_rng_data_t * rdat = (struct bcm2837_rng_data_t *)res->data;
	struct bcm2837_rng_pdata_t * pdat;
	struct rng_t * rng;
	char name[64];

	pdat = malloc(sizeof(struct bcm2837_rng_pdata_t));
	if(!pdat)
		return FALSE;

	rng = malloc(sizeof(struct rng_t));
	if(!rng)
	{
		free(pdat);
		return FALSE;
	}

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	pdat->virt = phys_to_virt(rdat->phys);
	rng->name = strdup(name);
	rng->init = rng_init;
	rng->exit = rng_exit;
	rng->read = rng_read,
	rng->suspend = rng_suspend,
	rng->resume = rng_resume,
	rng->priv = pdat;

	if(register_rng(rng))
		return TRUE;

	free(rng->priv);
	free(rng->name);
	free(rng);
	return FALSE;
}

static bool_t bcm2837_unregister_rng(struct resource_t * res)
{
	struct rng_t * rng;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	rng = search_rng(name);
	if(!rng)
		return FALSE;

	if(!unregister_rng(rng))
		return FALSE;

	free(rng->priv);
	free(rng->name);
	free(rng);
	return TRUE;
}

static __init void bcm2837_rng_init(void)
{
	resource_for_each("bcm2837-rng", bcm2837_register_rng);
}

static __exit void bcm2837_rng_exit(void)
{
	resource_for_each("bcm2837-rng", bcm2837_unregister_rng);
}

device_initcall(bcm2837_rng_init);
device_exitcall(bcm2837_rng_exit);
