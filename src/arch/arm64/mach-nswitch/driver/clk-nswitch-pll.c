/*
 * driver/clk-nswitch-pll.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <clk/clk.h>
#include <nswitch/reg-clkrst.h>

enum {
	PLLC_OUT0		= 0,
	PLLC_OUT1		= 1,
	PLLA1_OUT		= 2,
	PLLC2_OUT0		= 3,
	PLLC3_OUT0		= 4,
	PLLA			= 5,
	PLLA_OUT0		= 6,
	PLLP_OUT0		= 7,
	PLLP_OUT1		= 8,
	PLLP_OUT2		= 9,
	PLLP_OUT3		= 10,
	PLLP_OUT4		= 11,
	PLLP_OUT5		= 12,
	PLLE_VCO		= 13,
	PLLE_DOUT		= 14,
	PLLREFE_OUT1	= 15,
	PLLC4_VCO		= 16,
	PLLC4_OUT0		= 17,
	PLLC4_OUT1		= 18,
	PLLC4_OUT2		= 19,
	PLLC4_OUT3		= 20,
};

struct clk_nswitch_pll_pdata_t {
	virtual_addr_t virt;
	char * parent;
	int index;
};

static void clk_nswitch_pll_set_parent(struct clk_t * clk, const char * pname)
{
}

static const char * clk_nswitch_pll_get_parent(struct clk_t * clk)
{
	struct clk_nswitch_pll_pdata_t * pdat = (struct clk_nswitch_pll_pdata_t *)clk->priv;
	return pdat->parent;
}

static void clk_nswitch_pll_set_enable(struct clk_t * clk, bool_t enable)
{
	struct clk_nswitch_pll_pdata_t * pdat = (struct clk_nswitch_pll_pdata_t *)clk->priv;
	struct clkrst_reg_t * cr = (struct clkrst_reg_t *)pdat->virt;
	u32_t r;

	switch(pdat->index)
	{
	case PLLC_OUT0:
		r = read32((virtual_addr_t)&cr->pllc_base);
		if(enable)
			r |= (1 << 30);
		else
			r &= ~(1 << 30);
		write32((virtual_addr_t)&cr->pllc_base, r);
		break;

	case PLLC_OUT1:
		r = read32((virtual_addr_t)&cr->pllc_out);
		if(enable)
			r |= (1 << 1);
		else
			r &= ~(1 << 1);
		write32((virtual_addr_t)&cr->pllc_out, r);
		break;

	case PLLA1_OUT:
		r = read32((virtual_addr_t)&cr->plla1_base0);
		if(enable)
			r |= (1 << 30);
		else
			r &= ~(1 << 30);
		write32((virtual_addr_t)&cr->plla1_base0, r);
		break;

	case PLLC2_OUT0:
		r = read32((virtual_addr_t)&cr->pllc2_base);
		if(enable)
			r |= (1 << 30);
		else
			r &= ~(1 << 30);
		write32((virtual_addr_t)&cr->pllc2_base, r);
		break;

	case PLLC3_OUT0:
		r = read32((virtual_addr_t)&cr->pllc3_base);
		if(enable)
			r |= (1 << 30);
		else
			r &= ~(1 << 30);
		write32((virtual_addr_t)&cr->pllc3_base, r);
		break;

	case PLLA:
		r = read32((virtual_addr_t)&cr->plla_base);
		if(enable)
			r |= (1 << 30);
		else
			r &= ~(1 << 30);
		write32((virtual_addr_t)&cr->plla_base, r);
		break;

	case PLLA_OUT0:
		r = read32((virtual_addr_t)&cr->plla_out);
		if(enable)
			r |= (1 << 1);
		else
			r &= ~(1 << 1);
		write32((virtual_addr_t)&cr->plla_out, r);
		break;

	case PLLP_OUT0:
		r = read32((virtual_addr_t)&cr->pllp_base);
		if(enable)
			r |= (1 << 30);
		else
			r &= ~(1 << 30);
		write32((virtual_addr_t)&cr->pllp_base, r);
		break;

	case PLLP_OUT1:
		r = read32((virtual_addr_t)&cr->pllp_outa);
		if(enable)
			r |= (1 << 1);
		else
			r &= ~(1 << 1);
		write32((virtual_addr_t)&cr->pllp_outa, r);
		break;

	case PLLP_OUT2:
		break;

	case PLLP_OUT3:
		r = read32((virtual_addr_t)&cr->pllp_outb);
		if(enable)
			r |= (1 << 1);
		else
			r &= ~(1 << 1);
		write32((virtual_addr_t)&cr->pllp_outb, r);
		break;

	case PLLP_OUT4:
		r = read32((virtual_addr_t)&cr->pllp_outb);
		if(enable)
			r |= (1 << 17);
		else
			r &= ~(1 << 17);
		write32((virtual_addr_t)&cr->pllp_outb, r);
		break;

	case PLLP_OUT5:
		r = read32((virtual_addr_t)&cr->pllp_outc0);
		if(enable)
			r |= (1 << 17);
		else
			r &= ~(1 << 17);
		write32((virtual_addr_t)&cr->pllp_outc0, r);
		break;

	case PLLE_VCO:
		r = read32((virtual_addr_t)&cr->plle_base);
		if(enable)
			r |= (1 << 31);
		else
			r &= ~(1 << 31);
		write32((virtual_addr_t)&cr->plle_base, r);
		break;

	case PLLE_DOUT:
		break;

	case PLLREFE_OUT1:
		r = read32((virtual_addr_t)&cr->pllrefe_out0);
		if(enable)
			r |= (1 << 1);
		else
			r &= ~(1 << 1);
		write32((virtual_addr_t)&cr->pllrefe_out0, r);
		break;

	case PLLC4_VCO:
		r = read32((virtual_addr_t)&cr->pllc4_base0);
		if(enable)
			r |= (1 << 30);
		else
			r &= ~(1 << 30);
		write32((virtual_addr_t)&cr->pllc4_base0, r);
		break;

	case PLLC4_OUT0:
		break;

	case PLLC4_OUT1:
		break;

	case PLLC4_OUT2:
		break;

	case PLLC4_OUT3:
		r = read32((virtual_addr_t)&cr->pllc4_out0);
		if(enable)
			r |= (1 << 1);
		else
			r &= ~(1 << 1);
		write32((virtual_addr_t)&cr->pllc4_out0, r);
		break;

	default:
		break;
	}
}

static bool_t clk_nswitch_pll_get_enable(struct clk_t * clk)
{
	struct clk_nswitch_pll_pdata_t * pdat = (struct clk_nswitch_pll_pdata_t *)clk->priv;
	struct clkrst_reg_t * cr = (struct clkrst_reg_t *)pdat->virt;
	u32_t r;

	switch(pdat->index)
	{
	case PLLC_OUT0:
		r = read32((virtual_addr_t)&cr->pllc_base);
		return (r & (1 << 30)) ? TRUE : FALSE;

	case PLLC_OUT1:
		r = read32((virtual_addr_t)&cr->pllc_out);
		return (r & (1 << 1)) ? TRUE : FALSE;

	case PLLA1_OUT:
		r = read32((virtual_addr_t)&cr->plla1_base0);
		return (r & (1 << 30)) ? TRUE : FALSE;

	case PLLC2_OUT0:
		r = read32((virtual_addr_t)&cr->pllc2_base);
		return (r & (1 << 30)) ? TRUE : FALSE;

	case PLLC3_OUT0:
		r = read32((virtual_addr_t)&cr->pllc3_base);
		return (r & (1 << 30)) ? TRUE : FALSE;

	case PLLA:
		r = read32((virtual_addr_t)&cr->plla_base);
		return (r & (1 << 30)) ? TRUE : FALSE;

	case PLLA_OUT0:
		r = read32((virtual_addr_t)&cr->plla_out);
		return (r & (1 << 1)) ? TRUE : FALSE;

	case PLLP_OUT0:
		r = read32((virtual_addr_t)&cr->pllp_base);
		return (r & (1 << 30)) ? TRUE : FALSE;

	case PLLP_OUT1:
		r = read32((virtual_addr_t)&cr->pllp_outa);
		return (r & (1 << 1)) ? TRUE : FALSE;

	case PLLP_OUT2:
		return TRUE;

	case PLLP_OUT3:
		r = read32((virtual_addr_t)&cr->pllp_outb);
		return (r & (1 << 1)) ? TRUE : FALSE;

	case PLLP_OUT4:
		r = read32((virtual_addr_t)&cr->pllp_outb);
		return (r & (1 << 17)) ? TRUE : FALSE;

	case PLLP_OUT5:
		r = read32((virtual_addr_t)&cr->pllp_outc0);
		return (r & (1 << 17)) ? TRUE : FALSE;

	case PLLE_VCO:
		r = read32((virtual_addr_t)&cr->plle_base);
		return (r & (1 << 31)) ? TRUE : FALSE;

	case PLLE_DOUT:
		return TRUE;

	case PLLREFE_OUT1:
		r = read32((virtual_addr_t)&cr->pllrefe_out0);
		return (r & (1 << 1)) ? TRUE : FALSE;

	case PLLC4_VCO:
		r = read32((virtual_addr_t)&cr->pllc4_base0);
		return (r & (1 << 30)) ? TRUE : FALSE;

	case PLLC4_OUT0:
		return TRUE;

	case PLLC4_OUT1:
		return TRUE;

	case PLLC4_OUT2:
		return TRUE;

	case PLLC4_OUT3:
		r = read32((virtual_addr_t)&cr->pllc4_out0);
		return (r & (1 << 1)) ? TRUE : FALSE;

	default:
		break;
	}
	return FALSE;
}

static void clk_nswitch_pll_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

static const u32_t pdivmap[] = { 1, 2, 3, 4, 5, 6, 8, 10, 12, 16, 12, 16, 20, 24, 32, 1,
								 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  1,  1,  1,  1,  1, 1, };

static u64_t clk_nswitch_pll_get_rate(struct clk_t * clk, u64_t prate)
{
	struct clk_nswitch_pll_pdata_t * pdat = (struct clk_nswitch_pll_pdata_t *)clk->priv;
	struct clkrst_reg_t * cr = (struct clkrst_reg_t *)pdat->virt;
	u32_t r, m, n, p, d;
	u64_t cf, vco, rate = 0;

	switch(pdat->index)
	{
	case PLLC_OUT0:
		r = read32((virtual_addr_t)&cr->pllc_base);
		if(!(r & (1 << 31)))
		{
			m = (r >> 0) & 0xff;
			n = (r >> 10) & 0xff;
			p = (r >> 20) & 0x1f;
			cf = prate / m;
			vco = cf * n;
			rate = vco / pdivmap[p];
		}
		else
			rate = prate;
		break;

	case PLLC_OUT1:
		r = read32((virtual_addr_t)&cr->pllc_out);
		if(!(r & (1 << 16)))
		{
			d = (r >> 8) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	case PLLA1_OUT:
		r = read32((virtual_addr_t)&cr->plla1_base0);
		if(!(r & (1 << 31)))
		{
			m = (r >> 0) & 0xff;
			n = (r >> 10) & 0xff;
			p = (r >> 20) & 0x1f;
			cf = prate / m;
			vco = cf * n;
			rate = vco / pdivmap[p];
		}
		else
			rate = prate;
		break;

	case PLLC2_OUT0:
		r = read32((virtual_addr_t)&cr->pllc2_base);
		if(!(r & (1 << 31)))
		{
			m = (r >> 0) & 0xff;
			n = (r >> 10) & 0xff;
			p = (r >> 20) & 0x1f;
			cf = prate / m;
			vco = cf * n;
			rate = vco / pdivmap[p];
		}
		else
			rate = prate;
		break;

	case PLLC3_OUT0:
		r = read32((virtual_addr_t)&cr->pllc3_base);
		if(!(r & (1 << 31)))
		{
			m = (r >> 0) & 0xff;
			n = (r >> 10) & 0xff;
			p = (r >> 20) & 0x1f;
			cf = prate / m;
			vco = cf * n;
			rate = vco / pdivmap[p];
		}
		else
			rate = prate;
		break;

	case PLLA:
		r = read32((virtual_addr_t)&cr->plla_base);
		if(!(r & (1 << 31)))
		{
			m = (r >> 0) & 0xff;
			n = (r >> 10) & 0xff;
			p = (r >> 20) & 0x1f;
			cf = prate / m;
			vco = cf * n;
			rate = vco / pdivmap[p];
		}
		else
			rate = prate;
		break;

	case PLLA_OUT0:
		r = read32((virtual_addr_t)&cr->plla_misc);
		if(!(r & (1 << 30)))
		{
			r = read32((virtual_addr_t)&cr->plla_out);
			d = (r >> 8) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	case PLLP_OUT0:
		r = read32((virtual_addr_t)&cr->pllp_base);
		if(!(r & (1 << 31)))
		{
			m = (r >> 0) & 0xff;
			n = (r >> 10) & 0xff;
			p = (r >> 20) & 0x1f;
			cf = prate / m;
			vco = cf * n;
			rate = vco;
		}
		else
			rate = prate;
		break;

	case PLLP_OUT1:
		r = read32((virtual_addr_t)&cr->pllp_misc);
		if(!(r & (1 << 24)))
		{
			r = read32((virtual_addr_t)&cr->pllp_outa);
			d = (r >> 8) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	case PLLP_OUT2:
		r = read32((virtual_addr_t)&cr->pllp_base);
		p = (r >> 20) & 0x1f;
		rate = prate / (1 << p);
		break;

	case PLLP_OUT3:
		r = read32((virtual_addr_t)&cr->pllp_misc);
		if(!(r & (1 << 26)))
		{
			r = read32((virtual_addr_t)&cr->pllp_outb);
			d = (r >> 8) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	case PLLP_OUT4:
		r = read32((virtual_addr_t)&cr->pllp_misc);
		if(!(r & (1 << 27)))
		{
			r = read32((virtual_addr_t)&cr->pllp_outb);
			d = (r >> 24) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	case PLLP_OUT5:
		r = read32((virtual_addr_t)&cr->pllp_misc10);
		if(!(r & (1 << 30)))
		{
			r = read32((virtual_addr_t)&cr->pllp_outc0);
			d = (r >> 24) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	case PLLE_VCO:
		r = read32((virtual_addr_t)&cr->plle_base);
		m = (r >> 0) & 0xff;
		n = (r >> 8) & 0xff;
		p = (r >> 24) & 0x1f;
		cf = prate / m;
		vco = cf * n;
		if(!(r & (1 << 29)))
			rate = vco / 4;
		else
			rate = vco / 2;
		break;

	case PLLE_DOUT:
		r = read32((virtual_addr_t)&cr->plle_base);
		p = (r >> 24) & 0x1f;
		rate = prate / pdivmap[p];
		break;

	case PLLREFE_OUT1:
		r = read32((virtual_addr_t)&cr->pllrefe_base);
		if(!(r & (1 << 16)))
		{
			d = (r >> 8) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	case PLLC4_VCO:
		r = read32((virtual_addr_t)&cr->pllc4_base0);
		if(!(r & (1 << 31)))
		{
			m = (r >> 0) & 0xff;
			n = (r >> 8) & 0xff;
			p = (r >> 19) & 0x1f;
			cf = prate / m;
			vco = cf * n;
			rate = vco;
		}
		else
			rate = prate;
		break;

	case PLLC4_OUT0:
		r = read32((virtual_addr_t)&cr->pllc4_base0);
		p = (r >> 19) & 0x1f;
		rate = prate / pdivmap[p];
		break;

	case PLLC4_OUT1:
		rate = prate / 3;
		break;

	case PLLC4_OUT2:
		rate = prate / 5;
		break;

	case PLLC4_OUT3:
		r = read32((virtual_addr_t)&cr->pllc4_out0);
		if(!(r & (1 << 16)))
		{
			d = (r >> 8) & 0xff;
			rate = prate / ((d >> 1) + 1);
		}
		else
			rate = prate;
		break;

	default:
		break;
	}

	return rate;
}

static struct device_t * clk_nswitch_pll_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_nswitch_pll_pdata_t * pdat;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	char * parent = dt_read_string(n, "parent", NULL);
	char * name = dt_read_string(n, "name", NULL);
	int index = dt_read_int(n, "index", -1);

	if(index < PLLC_OUT0 || index > PLLC4_OUT3)
		return NULL;

	if(!parent || !name)
		return NULL;

	if(!search_clk(parent) || search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_nswitch_pll_pdata_t));
	if(!pdat)
		return NULL;

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = phys_to_virt(NSWITCH_CLKRST_BASE);
	pdat->parent = strdup(parent);
	pdat->index = index;

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_nswitch_pll_set_parent;
	clk->get_parent = clk_nswitch_pll_get_parent;
	clk->set_enable = clk_nswitch_pll_set_enable;
	clk->get_enable = clk_nswitch_pll_get_enable;
	clk->set_rate = clk_nswitch_pll_set_rate;
	clk->get_rate = clk_nswitch_pll_get_rate;
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

static void clk_nswitch_pll_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_nswitch_pll_pdata_t * pdat = (struct clk_nswitch_pll_pdata_t *)clk->priv;

	if(clk && unregister_clk(clk))
	{
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

static void clk_nswitch_pll_suspend(struct device_t * dev)
{
}

static void clk_nswitch_pll_resume(struct device_t * dev)
{
}

static struct driver_t clk_nswitch_pll = {
	.name		= "clk-nswitch-pll",
	.probe		= clk_nswitch_pll_probe,
	.remove		= clk_nswitch_pll_remove,
	.suspend	= clk_nswitch_pll_suspend,
	.resume		= clk_nswitch_pll_resume,
};

static __init void clk_nswitch_pll_driver_init(void)
{
	register_driver(&clk_nswitch_pll);
}

static __exit void clk_nswitch_pll_driver_exit(void)
{
	unregister_driver(&clk_nswitch_pll);
}

driver_initcall(clk_nswitch_pll_driver_init);
driver_exitcall(clk_nswitch_pll_driver_exit);
