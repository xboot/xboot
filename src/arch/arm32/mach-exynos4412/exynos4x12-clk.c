/*
 * exynos4x12-clk.c
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
#include <exynos4x12/reg-clk.h>

enum EXYNOS4X12_PLL {
	EXYNOS4X12_APLL,
	EXYNOS4X12_MPLL,
	EXYNOS4X12_EPLL,
	EXYNOS4X12_VPLL
};

static u64_t exynos4x12_get_pll_clk(u64_t fin, enum EXYNOS4X12_PLL pll)
{
	u32_t r, k, m, p, s;
	u64_t fout;

	switch(pll)
	{
	/*
	 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
	 */
	case EXYNOS4X12_APLL:
		r = readl(EXYNOS4X12_APLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (fin / (p * (1 << s)));
		break;

	/*
	 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
	 */
	case EXYNOS4X12_MPLL:
		r = readl(EXYNOS4X12_MPLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (fin / (p * (1 << s)));
		break;

	/*
	 * FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV)
	 */
	case EXYNOS4X12_EPLL:
		r = readl(EXYNOS4X12_EPLL_CON0);
		k = readl(EXYNOS4X12_EPLL_CON1);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		k = k & 0xffff;
		fout = (m + k / 65536) * (fin / (p * (1 << s)));
		break;

	/*
	 * FOUT = (MDIV + K / 65535) * FIN / (PDIV * 2^SDIV)
	 */
	case EXYNOS4X12_VPLL:
		r = readl(EXYNOS4X12_VPLL_CON0);
		k = readl(EXYNOS4X12_VPLL_CON1);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		k = k & 0xffff;
		fout = (m + k / 65535) * (fin / (p * (1 << s)));
		break;

	default:
		return 0;
	}

	return fout;
}

static u64_t exynos4x12_get_mout_apll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4X12_CLK_SRC_CPU);

	if( ((reg >> 0) & 0x1) == 0x1 )
		ret = exynos4x12_get_pll_clk(fin, EXYNOS4X12_APLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4x12_get_sclk_apll(u64_t fin)
{
	u32_t reg;
	u64_t ret, mout_apll;

	mout_apll = exynos4x12_get_mout_apll(fin);
	reg = (readl(EXYNOS4X12_CLK_DIV_CPU0) >> 24) & 0x7;

	ret = mout_apll / (reg + 1);
	return ret;
}

static u64_t exynos4x12_get_mout_mpll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4X12_CLK_SRC_CPU);

	if( ((reg >> 24) & 0x1) == 0x1 )
		ret = exynos4x12_get_pll_clk(fin, EXYNOS4X12_MPLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4x12_get_sclk_mpll(u64_t fin)
{
	return exynos4x12_get_mout_mpll(fin);
}

static u64_t exynos4x12_get_sclk_mpll_user_c(u64_t fin)
{
	return exynos4x12_get_sclk_mpll(fin);
}

static u64_t exynos4x12_get_sclk_mpll_user_t(u64_t fin)
{
	return exynos4x12_get_sclk_mpll(fin);
}

static u64_t exynos4x12_get_mout_core(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4X12_CLK_SRC_CPU);

	if( ((reg >> 16) & 0x1) == 0x1 )
		ret = exynos4x12_get_sclk_mpll_user_c(fin);
	else
		ret = exynos4x12_get_mout_apll(fin);

	return ret;
}

static u64_t exynos4x12_get_sclk_epll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4X12_CLK_SRC_TOP0);

	if( ((reg >> 4) & 0x1) == 0x1 )
		ret = exynos4x12_get_pll_clk(fin, EXYNOS4X12_EPLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4x12_get_sclk_vpll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4X12_CLK_SRC_TOP0);

	if( ((reg >> 8) & 0x1) == 0x1 )
		ret = exynos4x12_get_pll_clk(fin, EXYNOS4X12_VPLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4x12_get_armclk(u64_t fin)
{
	u32_t reg, core, core2;
	u64_t ret, mout_core;

	mout_core = exynos4x12_get_mout_core(fin);
	reg = readl(EXYNOS4X12_CLK_DIV_CPU0);
	core = (reg >> 0) & 0x7;
	core2 = (reg >> 28) & 0x7;

	ret = mout_core / (core + 1) / (core2 + 1);
	return ret;
}

static u64_t exynos4x12_get_sclk_uart(u64_t fin, u32_t ch)
{
	u32_t reg;
	u64_t ret, mout_uart;

	if(ch < 0 || ch > 4)
		return 0;

	reg = (readl(EXYNOS4X12_CLK_SRC_PERIL0) >> (ch * 4)) & 0xf;
	switch(reg)
	{
	case 0x6:
		mout_uart = exynos4x12_get_sclk_mpll_user_t(fin);
		break;

	case 0x7:
		mout_uart = exynos4x12_get_sclk_epll(fin);
		break;

	case 0x8:
		mout_uart = exynos4x12_get_sclk_vpll(fin);
		break;

	default:
		mout_uart = fin;
		break;
	}

	reg = (readl(EXYNOS4X12_CLK_DIV_PERIL0) >> (ch * 4)) & 0xf;
	ret = mout_uart / (reg + 1);

	return ret;
}

/*
 * the array of clocks, which will to be setup.
 */
static struct clk exynos4x12_clocks[11];

/*
 * setup the exynos4x12's clock array.
 */
static void exynos4x12_setup_clocks(u64_t xtal)
{
	exynos4x12_clocks[0].name = "xtal";
	exynos4x12_clocks[0].rate = xtal;

	exynos4x12_clocks[1].name = "sclk_apll";
	exynos4x12_clocks[1].rate = exynos4x12_get_sclk_apll(xtal);

	exynos4x12_clocks[2].name = "sclk_mpll";
	exynos4x12_clocks[2].rate = exynos4x12_get_sclk_mpll(xtal);

	exynos4x12_clocks[3].name = "sclk_epll";
	exynos4x12_clocks[3].rate = exynos4x12_get_sclk_epll(xtal);

	exynos4x12_clocks[4].name = "sclk_vpll";
	exynos4x12_clocks[4].rate = exynos4x12_get_sclk_vpll(xtal);

	exynos4x12_clocks[5].name = "armclk";
	exynos4x12_clocks[5].rate = exynos4x12_get_armclk(xtal);

	exynos4x12_clocks[6].name = "sclk_uart0";
	exynos4x12_clocks[6].rate = exynos4x12_get_sclk_uart(xtal, 0);

	exynos4x12_clocks[7].name = "sclk_uart1";
	exynos4x12_clocks[7].rate = exynos4x12_get_sclk_uart(xtal, 1);

	exynos4x12_clocks[8].name = "sclk_uart2";
	exynos4x12_clocks[8].rate = exynos4x12_get_sclk_uart(xtal, 2);

	exynos4x12_clocks[9].name = "sclk_uart3";
	exynos4x12_clocks[9].rate = exynos4x12_get_sclk_uart(xtal, 3);

	exynos4x12_clocks[10].name = "sclk_uart4";
	exynos4x12_clocks[10].rate = exynos4x12_get_sclk_uart(xtal, 4);
}

static __init void exynos4x12_clk_init(void)
{
	u32_t i;
	u64_t xtal = 0;

	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 24 * 1000 * 1000;

	exynos4x12_setup_clocks(xtal);

	for(i=0; i< ARRAY_SIZE(exynos4x12_clocks); i++)
	{
		if(!clk_register(&exynos4x12_clocks[i]))
		{
			LOG_E("failed to register clock '%s'", exynos4x12_clocks[i].name);
		}
	}
}

static __exit void exynos4x12_clk_exit(void)
{
	u32_t i;

	for(i=0; i< ARRAY_SIZE(exynos4x12_clocks); i++)
	{
		if(!clk_unregister(&exynos4x12_clocks[i]))
		{
			LOG_E("failed to unregister clock '%s'", exynos4x12_clocks[i].name);
		}
	}
}

core_initcall(exynos4x12_clk_init);
core_exitcall(exynos4x12_clk_exit);
