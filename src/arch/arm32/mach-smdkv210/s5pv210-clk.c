/*
 * s5pv210-clk.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <sizes.h>
#include <types.h>
#include <div64.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <s5pv210/reg-clk.h>

enum S5PV210_PLL {
	S5PV210_APLL,
	S5PV210_MPLL,
	S5PV210_EPLL,
	S5PV210_VPLL
};

/*
 * the array of clocks, which will to be setup.
 */
static struct clk s5pv210_clocks[8];

/*
 * get pll frequency.
 */
static u64_t s5pv210_get_pll(u64_t baseclk, enum S5PV210_PLL pll)
{
	u32_t r, m, p, s;
	u64_t fvco;

	switch(pll)
	{
	case S5PV210_APLL:
		r = readl(S5PV210_APLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		s = s - 1;
		break;
	case S5PV210_MPLL:
		r = readl(S5PV210_MPLL_CON);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		break;
	case S5PV210_EPLL:
		r = readl(S5PV210_EPLL_CON0);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		break;
	case S5PV210_VPLL:
		r = readl(S5PV210_VPLL_CON);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		break;
	default:
		return 0;
	}

	fvco = m * div64(baseclk, p * (1 << s));
	return (u64_t)fvco;
}

/*
 * setup the s5pv210's clock array.
 */
static void s5pv210_setup_clocks(u64_t xtal)
{
	u64_t fin, apll, mpll, epll, vpll, a2m;
	u64_t hpm, msys, dsys, psys;
	u64_t armclk, dsys_hclk, psys_hclk;
	u64_t tmp;
	u32_t clkdiv0, clkdiv1;
	u32_t clksrc0, clksrc1;
	u32_t muxstat0, muxstat1;

	/* get value of special register */
	clkdiv0 = readl(S5PV210_CLK_DIV0);
	clkdiv1 = readl(S5PV210_CLK_DIV1);
	clksrc0 = readl(S5PV210_CLK_SRC0);
	clksrc1 = readl(S5PV210_CLK_SRC1);
	muxstat0 = readl(S5PV210_MUX_STAT0);
	muxstat1 = readl(S5PV210_MUX_STAT1);

	/* use xtal as pll input clock, om[0]=0 */
	fin = xtal;

	/* extern clock */
	s5pv210_clocks[0].name = "xtal";
	s5pv210_clocks[0].rate = xtal;

	/* get apll clock */
	if( ((muxstat0 >> 0) & 0x7) == 0x2 )
		apll = s5pv210_get_pll(fin, S5PV210_APLL);
	else
		apll = fin;

	/* get mpll clock */
	if( ((muxstat0 >> 4) & 0x7) == 0x2 )
		mpll = s5pv210_get_pll(fin, S5PV210_MPLL);
	else
		mpll = fin;

	/* get epll clock */
	if( ((muxstat0 >> 8) & 0x7) == 0x2 )
		epll = s5pv210_get_pll(fin, S5PV210_EPLL);
	else
		epll = fin;

	/* get vpll clock */
	if(clksrc1 & (1<<28))
		tmp = 27 * 1000 * 1000;
	else
		tmp = fin;
	if( ((muxstat0 >> 12) & 0x7) == 0x2 )
		vpll = s5pv210_get_pll(tmp, S5PV210_VPLL);
	else
		vpll = tmp;

	/* get a2m clock */
	a2m = div64(apll, ((((clkdiv0) & S5PV210_CLKDIV0_A2M_MASK) >> S5PV210_CLKDIV0_A2M_SHIFT) + 1));

	/* get hpm clock */
	if( ((muxstat1 >> 16) & 0x7) == 0x2 )
		hpm = mpll;
	else
		hpm = apll;

	/* get msys clock */
	if( ((muxstat0 >> 16) & 0x7) == 0x2 )
		msys = mpll;
	else
		msys = apll;

	/* get dsys clock */
	if( ((muxstat0 >> 20) & 0x7) == 0x2 )
		dsys = a2m;
	else
		dsys = mpll;

	/* get psys clock */
	if( ((muxstat0 >> 24) & 0x7) == 0x2 )
		psys = a2m;
	else
		psys = mpll;

	/* get arm core clock */
	armclk = div64(msys, ((((clkdiv0) & S5PV210_CLKDIV0_APLL_MASK) >> S5PV210_CLKDIV0_APLL_SHIFT) + 1));

	/* get dsys hclk */
	dsys_hclk = div64(dsys, ((((clkdiv0) & S5PV210_CLKDIV0_HCLK_DSYS_MASK) >> S5PV210_CLKDIV0_HCLK_DSYS_SHIFT) + 1));

	/* get psys hclk */
	psys_hclk = div64(psys, ((((clkdiv0) & S5PV210_CLKDIV0_HCLK_PSYS_MASK) >> S5PV210_CLKDIV0_HCLK_PSYS_SHIFT) + 1));

	/* armclk */
	s5pv210_clocks[1].name = "armclk";
	s5pv210_clocks[1].rate = armclk;

	/* msys hclk */
	s5pv210_clocks[2].name = "msys-hclk";
	s5pv210_clocks[2].rate = div64(armclk, ((((clkdiv0) & S5PV210_CLKDIV0_HCLK_MSYS_MASK) >> S5PV210_CLKDIV0_HCLK_MSYS_SHIFT) + 1));

	/* msys pclk */
	s5pv210_clocks[3].name = "msys-pclk";
	s5pv210_clocks[3].rate = div64(s5pv210_clocks[2].rate, ((((clkdiv0) & S5PV210_CLKDIV0_PCLK_MSYS_MASK) >> S5PV210_CLKDIV0_PCLK_MSYS_SHIFT) + 1));

	/* dsys hclk */
	s5pv210_clocks[4].name = "dsys-hclk";
	s5pv210_clocks[4].rate = dsys_hclk;

	/* dsys pclk */
	s5pv210_clocks[5].name = "dsys-pclk";
	s5pv210_clocks[5].rate = div64(dsys_hclk, ((((clkdiv0) & S5PV210_CLKDIV0_PCLK_DSYS_MASK) >> S5PV210_CLKDIV0_PCLK_DSYS_SHIFT) + 1));

	/* psys hclk */
	s5pv210_clocks[6].name = "psys-hclk";
	s5pv210_clocks[6].rate = psys_hclk;

	/* psys pclk */
	s5pv210_clocks[7].name = "psys-pclk";
	s5pv210_clocks[7].rate = div64(psys_hclk, ((((clkdiv0) & S5PV210_CLKDIV0_PCLK_PSYS_MASK) >> S5PV210_CLKDIV0_PCLK_PSYS_SHIFT) + 1));
}

static __init void s5pv210_clk_init(void)
{
	u32_t i;
	u64_t xtal = 0;

	/* get system xtal */
	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 24 * 1000 * 1000;

	/* setup clock arrays */
	s5pv210_setup_clocks(xtal);

	/* register clocks to system */
	for(i=0; i< ARRAY_SIZE(s5pv210_clocks); i++)
	{
		if(!clk_register(&s5pv210_clocks[i]))
		{
			LOG_E("failed to register clock '%s'", s5pv210_clocks[i].name);
		}
	}
}

static __exit void s5pv210_clk_exit(void)
{
	u32_t i;

	for(i=0; i< ARRAY_SIZE(s5pv210_clocks); i++)
	{
		if(!clk_unregister(&s5pv210_clocks[i]))
		{
			LOG_E("failed to unregister clock '%s'", s5pv210_clocks[i].name);
		}
	}
}

core_initcall(s5pv210_clk_init);
core_exitcall(s5pv210_clk_exit);
