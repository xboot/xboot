/*
 * arch/arm/mach-smdkc100/s5pc100-clk.c
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
#include <s5pc100/reg-clk.h>


enum S5PC100_PLL {
	S5PC100_APLL,
	S5PC100_MPLL,
	S5PC100_EPLL,
	S5PC100_HPLL
};

/*
 * the array of clocks, which will to be setup.
 */
static struct clk s5pc100_clocks[5];

/*
 * get pll frequency.
 */
static u64_t s5pc100_get_pll(u64_t baseclk, enum S5PC100_PLL pll)
{
	u32_t r, m, p, s, mask;
	u64_t fvco;

	switch(pll)
	{
	case S5PC100_APLL:
		r = readl(S5PC100_APLL_CON);
		break;
	case S5PC100_MPLL:
		r = readl(S5PC100_MPLL_CON);
		break;
	case S5PC100_EPLL:
		r = readl(S5PC100_EPLL_CON);
		break;
	case S5PC100_HPLL:
		r = readl(S5PC100_HPLL_CON);
		break;
	default:
		return 0;
	}

	if(pll == S5PC100_APLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;
	p = (r >> 8) & 0x3f;
	s = r & 0x7;

	fvco = m * div64(baseclk, p * (1 << s));
	return (u64_t)fvco;
}

/*
 * setup the s5pc100's clock array.
 */
static void s5pc100_setup_clocks(u64_t xtal)
{
	u64_t fin, apll, mpll, epll, hpll;
	u64_t dout_d1bus, dout_mpll2, dout_mpll;
	u64_t tmp;
	u32_t clkdiv0, clkdiv1;
	u32_t clksrc0;

	/* get value of clkdiv and clksrc register */
	clkdiv0 = readl(S5PC100_CLK_DIV0);
	clkdiv1 = readl(S5PC100_CLK_DIV1);
	clksrc0 = readl(S5PC100_CLK_SRC0);

	/* use xtal as pll input clock, om[0]=0 */
	fin = xtal;

	/* extern clock */
	s5pc100_clocks[0].name = "xtal";
	s5pc100_clocks[0].rate = xtal;

	/* get apll clock */
	if(clksrc0 & (1<<0))
		apll = s5pc100_get_pll(fin, S5PC100_APLL);
	else
		apll = fin;

	/* get mpll clock */
	if(clksrc0 & (1<<4))
		mpll = s5pc100_get_pll(fin, S5PC100_MPLL);
	else
		mpll = fin;

	/* get epll clock */
	if(clksrc0 & (1<<8))
		epll = s5pc100_get_pll(fin, S5PC100_EPLL);
	else
		epll = fin;

	/* get hpll clock */
	if(clksrc0 & (1<<20))
	{
		if(clksrc0 & (1<<12))
			hpll = s5pc100_get_pll(fin, S5PC100_HPLL);
		else
			hpll = fin;
	}
	else
	{
		if(clksrc0 & (1<<12))
			hpll = s5pc100_get_pll(27*1000*1000, S5PC100_HPLL);
		else
			hpll = 27*1000*1000;
	}

	/* get armclk clock */
	tmp = div64(apll, ((((clkdiv0) & S5PC100_CLKDIV0_APLL_MASK) >> S5PC100_CLKDIV0_APLL_SHIFT) + 1));
	tmp = div64(tmp, ((((clkdiv0) & S5PC100_CLKDIV0_ARM_MASK) >> S5PC100_CLKDIV0_ARM_SHIFT) + 1));

	/* armclk */
	s5pc100_clocks[1].name = "armclk";
	s5pc100_clocks[1].rate = tmp;

	/* get dout_d1bus, dout_mpll2 and dout_mpll's clock */
	if(clksrc0 & (1<<16))
		tmp = div64(apll, ((((clkdiv1) & S5PC100_CLKDIV1_APLL2_MASK) >> S5PC100_CLKDIV1_APLL2_SHIFT) + 1));
	else
		tmp = mpll;
	dout_d1bus = div64(tmp, ((((clkdiv1) & S5PC100_CLKDIV1_D1BUS_MASK) >> S5PC100_CLKDIV1_D1BUS_SHIFT) + 1));
	dout_mpll2 = div64(tmp, ((((clkdiv1) & S5PC100_CLKDIV1_MPLL2_MASK) >> S5PC100_CLKDIV1_MPLL2_SHIFT) + 1));
	dout_mpll = div64(tmp, ((((clkdiv1) & S5PC100_CLKDIV1_MPLL_MASK) >> S5PC100_CLKDIV1_MPLL_SHIFT) + 1));

	/* fclk */
	s5pc100_clocks[2].name = "fclk";
	s5pc100_clocks[2].rate = apll;

	/* hclk */
	s5pc100_clocks[3].name = "hclk";
	s5pc100_clocks[3].rate = dout_d1bus;

	/* pclk */
	s5pc100_clocks[4].name = "pclk";
	s5pc100_clocks[4].rate = div64(dout_d1bus, ((((clkdiv1) & S5PC100_CLKDIV1_PCLKD1_MASK) >> S5PC100_CLKDIV1_PCLKD1_SHIFT) + 1));
}

static __init void s5pc100_clk_init(void)
{
	u32_t i;
	u64_t xtal = 0;

	/* get system xtal. */
	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 12*1000*1000;

	/* setup clock arrays */
	s5pc100_setup_clocks(xtal);

	/* register clocks to system */
	for(i=0; i< ARRAY_SIZE(s5pc100_clocks); i++)
	{
		if(!clk_register(&s5pc100_clocks[i]))
		{
			LOG_E("failed to register clock '%s'", s5pc100_clocks[i].name);
		}
	}
}

static __exit void s5pc100_clk_exit(void)
{
	u32_t i;

	for(i=0; i< ARRAY_SIZE(s5pc100_clocks); i++)
	{
		if(!clk_unregister(&s5pc100_clocks[i]))
		{
			LOG_E("failed to unregister clock '%s'", s5pc100_clocks[i].name);
		}
	}
}

core_initcall(s5pc100_clk_init);
core_exitcall(s5pc100_clk_exit);
