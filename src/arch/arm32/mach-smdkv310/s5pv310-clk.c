/*
 * s5pv310-clk.c
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
#include <s5pv310/reg-clk.h>

enum S5PV310_PLL {
	S5PV310_APLL,
	S5PV310_MPLL,
	S5PV310_EPLL,
	S5PV310_VPLL
};

static u64_t s5pv310_get_pll_clk(u64_t fin, enum S5PV310_PLL pll)
{
	u32_t r, k, m, p, s;
	u64_t fout;

	switch(pll)
	{
	/*
	 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV - 1))
	 */
	case S5PV310_APLL:
		r = readl(S5PV310_APLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (fin / (p * (1 << (s - 1))));
		break;

	/*
	 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV - 1))
	 */
	case S5PV310_MPLL:
		r = readl(S5PV310_MPLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (fin / (p * (1 << (s - 1))));
		break;

	/*
	 * FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV)
	 */
	case S5PV310_EPLL:
		r = readl(S5PV310_EPLL_CON0);
		k = readl(S5PV310_EPLL_CON1);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		k = k & 0xffff;
		fout = (m + k / 65536) * (fin / (p * (1 << s)));
		break;

	/*
	 * FOUT = (MDIV + K / 1024) * FIN / (PDIV * 2^SDIV)
	 */
	case S5PV310_VPLL:
		r = readl(S5PV310_VPLL_CON0);
		k = readl(S5PV310_VPLL_CON1);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		k = k & 0xfff;
		fout = (m + k / 1024) * (fin / (p * (1 << s)));
		break;

	default:
		return 0;
	}

	return fout;
}

static u64_t s5pv310_get_mout_apll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(S5PV310_CLK_SRC_CPU);

	if( ((reg >> 0) & 0x1) == 0x1 )
		ret = s5pv310_get_pll_clk(fin, S5PV310_APLL);
	else
		ret = fin;

	return ret;
}

static u64_t s5pv310_get_sclk_apll(u64_t fin)
{
	u32_t reg;
	u64_t ret, mout_apll;

	mout_apll = s5pv310_get_mout_apll(fin);
	reg = (readl(S5PV310_CLK_DIV_CPU0) >> 24) & 0x7;

	ret = mout_apll / (reg + 1);
	return ret;
}

static u64_t s5pv310_get_mout_mpll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(S5PV310_CLK_SRC_CPU);

	if( ((reg >> 8) & 0x1) == 0x1 )
		ret = s5pv310_get_pll_clk(fin, S5PV310_MPLL);
	else
		ret = fin;

	return ret;
}

static u64_t s5pv310_get_sclk_mpll(u64_t fin)
{
	return s5pv310_get_mout_mpll(fin);
}

static u64_t s5pv310_get_mout_core(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(S5PV310_CLK_SRC_CPU);

	if( ((reg >> 16) & 0x1) == 0x1 )
		ret = s5pv310_get_sclk_mpll(fin);
	else
		ret = s5pv310_get_mout_apll(fin);

	return ret;
}

static u64_t s5pv310_get_sclk_epll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(S5PV310_CLK_SRC_TOP0);

	if( ((reg >> 4) & 0x1) == 0x1 )
		ret = s5pv310_get_pll_clk(fin, S5PV310_EPLL);
	else
		ret = fin;

	return ret;
}

static u64_t s5pv310_get_sclk_vpll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(S5PV310_CLK_SRC_TOP0);

	if( ((reg >> 8) & 0x1) == 0x1 )
		ret = s5pv310_get_pll_clk(fin, S5PV310_VPLL);
	else
		ret = fin;

	return ret;
}

static u64_t s5pv310_get_armclk(u64_t fin)
{
	u32_t reg;
	u64_t ret, mout_core;

	mout_core = s5pv310_get_mout_core(fin);
	reg = (readl(S5PV310_CLK_DIV_CPU0) >> 0) & 0x7;

	ret = mout_core / (reg + 1);
	return ret;
}

static u64_t s5pv310_get_sclk_uart(u64_t fin, u32_t ch)
{
	u32_t reg;
	u64_t ret, mout_uart;

	if(ch < 0 || ch > 4)
		return 0;

	reg = (readl(S5PV310_CLK_SRC_PERIL0) >> (ch * 4)) & 0xf;
	switch(reg)
	{
	case 0x6:
		mout_uart = s5pv310_get_sclk_mpll(fin);
		break;

	case 0x7:
		mout_uart = s5pv310_get_sclk_epll(fin);
		break;

	case 0x8:
		mout_uart = s5pv310_get_sclk_vpll(fin);
		break;

	default:
		mout_uart = fin;
		break;
	}

	reg = (readl(S5PV310_CLK_DIV_PERIL0) >> (ch * 4)) & 0xf;
	ret = mout_uart / (reg + 1);

	return ret;
}

/*
 * the array of clocks, which will to be setup.
 */
static struct clk s5pv310_clocks[11];

/*
 * setup the s5pv310's clock array.
 */
static void s5pv310_setup_clocks(u64_t xtal)
{
	s5pv310_clocks[0].name = "xtal";
	s5pv310_clocks[0].rate = xtal;

	s5pv310_clocks[1].name = "sclk_apll";
	s5pv310_clocks[1].rate = s5pv310_get_sclk_apll(xtal);

	s5pv310_clocks[2].name = "sclk_mpll";
	s5pv310_clocks[2].rate = s5pv310_get_sclk_mpll(xtal);

	s5pv310_clocks[3].name = "sclk_epll";
	s5pv310_clocks[3].rate = s5pv310_get_sclk_epll(xtal);

	s5pv310_clocks[4].name = "sclk_vpll";
	s5pv310_clocks[4].rate = s5pv310_get_sclk_vpll(xtal);

	s5pv310_clocks[5].name = "armclk";
	s5pv310_clocks[5].rate = s5pv310_get_armclk(xtal);

	s5pv310_clocks[6].name = "sclk_uart0";
	s5pv310_clocks[6].rate = s5pv310_get_sclk_uart(xtal, 0);

	s5pv310_clocks[7].name = "sclk_uart1";
	s5pv310_clocks[7].rate = s5pv310_get_sclk_uart(xtal, 1);

	s5pv310_clocks[8].name = "sclk_uart2";
	s5pv310_clocks[8].rate = s5pv310_get_sclk_uart(xtal, 2);

	s5pv310_clocks[9].name = "sclk_uart3";
	s5pv310_clocks[9].rate = s5pv310_get_sclk_uart(xtal, 3);

	s5pv310_clocks[10].name = "sclk_uart4";
	s5pv310_clocks[10].rate = s5pv310_get_sclk_uart(xtal, 4);
}

static __init void s5pv310_clk_init(void)
{
	u32_t i;
	u64_t xtal = 0;

	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 24 * 1000 * 1000;

	s5pv310_setup_clocks(xtal);

	for(i=0; i< ARRAY_SIZE(s5pv310_clocks); i++)
	{
		if(!clk_register(&s5pv310_clocks[i]))
		{
			LOG_E("failed to register clock '%s'", s5pv310_clocks[i].name);
		}
	}
}

static __exit void s5pv310_clk_exit(void)
{
	u32_t i;

	for(i=0; i< ARRAY_SIZE(s5pv310_clocks); i++)
	{
		if(!clk_unregister(&s5pv310_clocks[i]))
		{
			LOG_E("failed to unregister clock '%s'", s5pv310_clocks[i].name);
		}
	}
}

core_initcall(s5pv310_clk_init);
core_exitcall(s5pv310_clk_exit);
