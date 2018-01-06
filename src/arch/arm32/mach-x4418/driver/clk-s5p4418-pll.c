/*
 * driver/clk-s5p4418-pll.c
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

#include <clk/clk.h>

enum {
	PLLSETREG		= 0x00,
	PLLSETREG_SSCG	= 0x40,
};

struct clk_s5p4418_pll_pdata_t {
	virtual_addr_t virt;
	char * parent;
};

static void clk_s5p4418_pll_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_s5p4418_pll_get_parent(struct clk_t * clk)
{
	struct clk_s5p4418_pll_pdata_t * pdat = (struct clk_s5p4418_pll_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_s5p4418_pll_set_enable(struct clk_t * clk, bool_t enable)
{
}

static bool_t clk_s5p4418_pll_get_enable(struct clk_t * clk)
{
	return TRUE;
}

static void clk_s5p4418_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_s5p4418_pll_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_s5p4418_pll_pdata_t * pdat = (struct clk_s5p4418_pll_pdata_t *)clk->priv;
	u32_t r, p, m, s, k;

	/*
	 * FOUT = (MDIV + K / 65535) * FIN / (PDIV * 2^SDIV)
	 */
	r = read32(pdat->virt + PLLSETREG);
	p = (r >> 18) & 0x3f;
	m = (r >> 8) & 0x3ff;
	s = (r >> 0) & 0xff;
	r = read32(pdat->virt + PLLSETREG_SSCG);
	k = (r >> 16) & 0xffff;

	return (u64_t)((m + k / 65536) * (prate / (p * (1 << s))));
}

static struct device_t * clk_s5p4418_pll_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_s5p4418_pll_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);

	if(!parent || !name)
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_s5p4418_pll_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->parent = strdup(parent);

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_s5p4418_pll_set_parent;
	clk->get_parent = clk_s5p4418_pll_get_parent;
	clk->set_enable = clk_s5p4418_pll_set_enable;
	clk->get_enable = clk_s5p4418_pll_get_enable;
	clk->set_rate = clk_s5p4418_pll_set_rate;
	clk->get_rate = clk_s5p4418_pll_get_rate;
	clk->priv = pdat;

	if(!register_clk(&dev, clk))
	{
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void clk_s5p4418_pll_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_s5p4418_pll_pdata_t * pdat = (struct clk_s5p4418_pll_pdata_t *)clk->priv;

	if(clk && unregister_clk(clk))
	{
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_s5p4418_pll_suspend(struct device_t * dev)
{
}

static void clk_s5p4418_pll_resume(struct device_t * dev)
{
}

static struct driver_t clk_s5p4418_pll = {
	.name		= "clk-s5p4418-pll",
	.probe		= clk_s5p4418_pll_probe,
	.remove		= clk_s5p4418_pll_remove,
	.suspend	= clk_s5p4418_pll_suspend,
	.resume		= clk_s5p4418_pll_resume,
};

static __init void clk_s5p4418_pll_driver_init(void)
{
	register_driver(&clk_s5p4418_pll);
}

static __exit void clk_s5p4418_pll_driver_exit(void)
{
	unregister_driver(&clk_s5p4418_pll);
}

driver_initcall(clk_s5p4418_pll_driver_init);
driver_exitcall(clk_s5p4418_pll_driver_exit);
