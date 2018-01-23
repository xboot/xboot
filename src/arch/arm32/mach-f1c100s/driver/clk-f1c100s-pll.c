/*
 * driver/clk-f1c100s-pll.c
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
#include <f1c100s/reg-ccu.h>

struct clk_f1c100s_pll_pdata_t {
	virtual_addr_t virt;
	char * parent;
	int channel;
};

static void clk_f1c100s_pll_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_f1c100s_pll_get_parent(struct clk_t * clk)
{
	struct clk_f1c100s_pll_pdata_t * pdat = (struct clk_f1c100s_pll_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_f1c100s_pll_set_enable(struct clk_t * clk, bool_t enable)
{
	struct clk_f1c100s_pll_pdata_t * pdat = (struct clk_f1c100s_pll_pdata_t *)clk->priv;
	u32_t r = 0;

	switch(pdat->channel)
	{
	case 0:
		r = read32(pdat->virt + CCU_PLL_CPU_CTRL);
		if(enable)
			r |= (1 << 31);
		else
			r &= ~(1 << 31);
		write32(pdat->virt + CCU_PLL_CPU_CTRL, r);
		break;

	case 1:
		r = read32(pdat->virt + CCU_PLL_AUDIO_CTRL);
		if(enable)
			r |= (1 << 31);
		else
			r &= ~(1 << 31);
		write32(pdat->virt + CCU_PLL_AUDIO_CTRL, r);
		break;

	case 2:
		r = read32(pdat->virt + CCU_PLL_VIDEO_CTRL);
		if(enable)
			r |= (1 << 31);
		else
			r &= ~(1 << 31);
		write32(pdat->virt + CCU_PLL_VIDEO_CTRL, r);
		break;

	case 3:
		r = read32(pdat->virt + CCU_PLL_VE_CTRL);
		if(enable)
			r |= (1 << 31);
		else
			r &= ~(1 << 31);
		write32(pdat->virt + CCU_PLL_VE_CTRL, r);
		break;

	case 4:
		r = read32(pdat->virt + CCU_PLL_DDR_CTRL);
		if(enable)
			r |= (1 << 31);
		else
			r &= ~(1 << 31);
		write32(pdat->virt + CCU_PLL_DDR_CTRL, r);
		break;

	case 5:
		r = read32(pdat->virt + CCU_PLL_PERIPH_CTRL);
		if(enable)
			r |= (1 << 31);
		else
			r &= ~(1 << 31);
		write32(pdat->virt + CCU_PLL_PERIPH_CTRL, r);
		break;

	default:
		break;
	}
}

static bool_t clk_f1c100s_pll_get_enable(struct clk_t * clk)
{
	struct clk_f1c100s_pll_pdata_t * pdat = (struct clk_f1c100s_pll_pdata_t *)clk->priv;
	u32_t r = 0;

	switch(pdat->channel)
	{
	case 0:
		r = read32(pdat->virt + CCU_PLL_CPU_CTRL);
		break;

	case 1:
		r = read32(pdat->virt + CCU_PLL_AUDIO_CTRL);
		break;

	case 2:
		r = read32(pdat->virt + CCU_PLL_VIDEO_CTRL);
		break;

	case 3:
		r = read32(pdat->virt + CCU_PLL_VE_CTRL);
		break;

	case 4:
		r = read32(pdat->virt + CCU_PLL_DDR_CTRL);
		break;

	case 5:
		r = read32(pdat->virt + CCU_PLL_PERIPH_CTRL);
		break;

	default:
		break;
	}
	return (r & (1 << 31)) ? TRUE : FALSE;
}

static void clk_f1c100s_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static u64_t clk_f1c100s_pll_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_f1c100s_pll_pdata_t * pdat = (struct clk_f1c100s_pll_pdata_t *)clk->priv;
	u32_t r, n, k, m, p;
	u64_t rate = 0;

	switch(pdat->channel)
	{
	case 0:
		r = read32(pdat->virt + CCU_PLL_CPU_CTRL);
		n = ((r >> 8) & 0x1f) + 1;
		k = ((r >> 4) & 0x3) + 1;
		m = ((r >> 0) & 0x3) + 1;
		p = ((r >> 16) & 0x3) + 1;
		rate = (u64_t)((prate * n * k) / (m * p));
		break;

	case 1:
		r = read32(pdat->virt + CCU_PLL_AUDIO_CTRL);
		if(r & (1 << 24))
			n = ((r >> 8) & 0xf) + 1;
		else
			n = ((r >> 8) & 0x7f) + 1;
		m = ((r >> 0) & 0x1f) + 1;
		rate = (u64_t)((prate * n * 2) / m);
		break;

	case 2:
		r = read32(pdat->virt + CCU_PLL_VIDEO_CTRL);
		if(r & (1 << 24))
		{
			n = ((r >> 8) & 0x7f) + 1;
			m = ((r >> 0) & 0xf) + 1;
			rate = (u64_t)((prate * n) / m);
		}
		else
		{
			if(r & (1 << 25))
				rate = 297 * 1000 * 1000;
			else
				rate = 270 * 1000 * 1000;
		}
		break;

	case 3:
		r = read32(pdat->virt + CCU_PLL_VE_CTRL);
		if(r & (1 << 24))
		{
			n = ((r >> 8) & 0x7f) + 1;
			m = ((r >> 0) & 0xf) + 1;
			rate = (u64_t)((prate * n) / m);
		}
		else
		{
			if(r & (1 << 25))
				rate = 297 * 1000 * 1000;
			else
				rate = 270 * 1000 * 1000;
		}
		break;

	case 4:
		r = read32(pdat->virt + CCU_PLL_DDR_CTRL);
		n = ((r >> 8) & 0x1f) + 1;
		k = ((r >> 4) & 0x3) + 1;
		m = ((r >> 0) & 0x3) + 1;
		rate = (u64_t)((prate * n * k) / m);
		break;

	case 5:
		r = read32(pdat->virt + CCU_PLL_PERIPH_CTRL);
		n = ((r >> 8) & 0x1f) + 1;
		k = ((r >> 4) & 0x3) + 1;
		m = ((r >> 0) & 0x3) + 1;
		p = ((r >> 16) & 0x3) + 1;
		rate = (u64_t)((prate * n * k) / p);
		break;

	default:
		break;
	}

	return rate;
}

static struct device_t * clk_f1c100s_pll_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_f1c100s_pll_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);
	int channel = dt_read_int(n, "channel", -1);

	if(channel < 0 || channel > 5)
		return NULL;

	if(!parent || !name)
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_f1c100s_pll_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = phys_to_virt(F1C100S_CCU_BASE);
	pdat->parent = strdup(parent);
	pdat->channel = channel;

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_f1c100s_pll_set_parent;
	clk->get_parent = clk_f1c100s_pll_get_parent;
	clk->set_enable = clk_f1c100s_pll_set_enable;
	clk->get_enable = clk_f1c100s_pll_get_enable;
	clk->set_rate = clk_f1c100s_pll_set_rate;
	clk->get_rate = clk_f1c100s_pll_get_rate;
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

	if(dt_read_object(n, "default", &o))
	{
		char * c = clk->name;
		char * p;
		u64_t r;
		int e;

		if((p = dt_read_string(&o, "parent", NULL)) && search_clk(p))
			clk_set_parent(c, p);
		if((r = (u64_t)dt_read_long(&o, "rate", 0)) > 0)
			clk_set_rate(c, r);
		if((e = dt_read_bool(&o, "enable", -1)) != -1)
		{
			if(e > 0)
				clk_enable(c);
			else
				clk_disable(c);
		}
	}
	return dev;
}

static void clk_f1c100s_pll_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_f1c100s_pll_pdata_t * pdat = (struct clk_f1c100s_pll_pdata_t *)clk->priv;

	if(clk && unregister_clk(clk))
	{
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_f1c100s_pll_suspend(struct device_t * dev)
{
}

static void clk_f1c100s_pll_resume(struct device_t * dev)
{
}

static struct driver_t clk_f1c100s_pll = {
	.name		= "clk-f1c100s-pll",
	.probe		= clk_f1c100s_pll_probe,
	.remove		= clk_f1c100s_pll_remove,
	.suspend	= clk_f1c100s_pll_suspend,
	.resume		= clk_f1c100s_pll_resume,
};

static __init void clk_f1c100s_pll_driver_init(void)
{
	register_driver(&clk_f1c100s_pll);
}

static __exit void clk_f1c100s_pll_driver_exit(void)
{
	unregister_driver(&clk_f1c100s_pll);
}

driver_initcall(clk_f1c100s_pll_driver_init);
driver_exitcall(clk_f1c100s_pll_driver_exit);
