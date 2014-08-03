/*
 * exynos4412-clk.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <exynos4412/reg-clk.h>

enum EXYNOS4412_PLL {
	EXYNOS4412_APLL,
	EXYNOS4412_MPLL,
	EXYNOS4412_EPLL,
	EXYNOS4412_VPLL
};

static u64_t exynos4412_get_pll_clk(u64_t fin, enum EXYNOS4412_PLL pll)
{
	u32_t r, k, m, p, s;
	u64_t fout;

	switch(pll)
	{
	/*
	 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
	 */
	case EXYNOS4412_APLL:
		r = readl(EXYNOS4412_APLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (fin / (p * (1 << s)));
		break;

	/*
	 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
	 */
	case EXYNOS4412_MPLL:
		r = readl(EXYNOS4412_MPLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (fin / (p * (1 << s)));
		break;

	/*
	 * FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV)
	 */
	case EXYNOS4412_EPLL:
		r = readl(EXYNOS4412_EPLL_CON0);
		k = readl(EXYNOS4412_EPLL_CON1);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		k = k & 0xffff;
		fout = (m + k / 65536) * (fin / (p * (1 << s)));
		break;

	/*
	 * FOUT = (MDIV + K / 65535) * FIN / (PDIV * 2^SDIV)
	 */
	case EXYNOS4412_VPLL:
		r = readl(EXYNOS4412_VPLL_CON0);
		k = readl(EXYNOS4412_VPLL_CON1);
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

static u64_t exynos4412_get_mout_apll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4412_CLK_SRC_CPU);

	if( ((reg >> 0) & 0x1) == 0x1 )
		ret = exynos4412_get_pll_clk(fin, EXYNOS4412_APLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4412_get_sclk_apll(u64_t fin)
{
	u32_t reg;
	u64_t ret, mout_apll;

	mout_apll = exynos4412_get_mout_apll(fin);
	reg = (readl(EXYNOS4412_CLK_DIV_CPU0) >> 24) & 0x7;

	ret = mout_apll / (reg + 1);
	return ret;
}

static u64_t exynos4412_get_mout_mpll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4412_CLK_SRC_CPU);

	if( ((reg >> 24) & 0x1) == 0x1 )
		ret = exynos4412_get_pll_clk(fin, EXYNOS4412_MPLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4412_get_sclk_mpll(u64_t fin)
{
	return exynos4412_get_mout_mpll(fin);
}

static u64_t exynos4412_get_sclk_mpll_user_c(u64_t fin)
{
	return exynos4412_get_sclk_mpll(fin);
}

static u64_t exynos4412_get_sclk_mpll_user_t(u64_t fin)
{
	return exynos4412_get_sclk_mpll(fin);
}

static u64_t exynos4412_get_mout_core(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4412_CLK_SRC_CPU);

	if( ((reg >> 16) & 0x1) == 0x1 )
		ret = exynos4412_get_sclk_mpll_user_c(fin);
	else
		ret = exynos4412_get_mout_apll(fin);

	return ret;
}

static u64_t exynos4412_get_sclk_epll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4412_CLK_SRC_TOP0);

	if( ((reg >> 4) & 0x1) == 0x1 )
		ret = exynos4412_get_pll_clk(fin, EXYNOS4412_EPLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4412_get_sclk_vpll(u64_t fin)
{
	u32_t reg;
	u64_t ret;

	reg = readl(EXYNOS4412_CLK_SRC_TOP0);

	if( ((reg >> 8) & 0x1) == 0x1 )
		ret = exynos4412_get_pll_clk(fin, EXYNOS4412_VPLL);
	else
		ret = fin;

	return ret;
}

static u64_t exynos4412_get_armclk(u64_t fin)
{
	u32_t reg, core, core2;
	u64_t ret, mout_core;

	mout_core = exynos4412_get_mout_core(fin);
	reg = readl(EXYNOS4412_CLK_DIV_CPU0);
	core = (reg >> 0) & 0x7;
	core2 = (reg >> 28) & 0x7;

	ret = mout_core / (core + 1) / (core2 + 1);
	return ret;
}

static u64_t exynos4412_get_sclk_uart(u64_t fin, u32_t ch)
{
	u32_t reg;
	u64_t ret, mout_uart;

	if(ch < 0 || ch > 4)
		return 0;

	reg = (readl(EXYNOS4412_CLK_SRC_PERIL0) >> (ch * 4)) & 0xf;
	switch(reg)
	{
	case 0x6:
		mout_uart = exynos4412_get_sclk_mpll_user_t(fin);
		break;

	case 0x7:
		mout_uart = exynos4412_get_sclk_epll(fin);
		break;

	case 0x8:
		mout_uart = exynos4412_get_sclk_vpll(fin);
		break;

	default:
		mout_uart = fin;
		break;
	}

	reg = (readl(EXYNOS4412_CLK_DIV_PERIL0) >> (ch * 4)) & 0xf;
	ret = mout_uart / (reg + 1);

	return ret;
}

/*
 * the array of clocks, which will to be setup.
 */
static struct clk_t exynos4412_clocks[11];

/*
 * setup the exynos4412's clock array.
 */
static void exynos4412_setup_clocks(u64_t xtal)
{
	exynos4412_clocks[0].name = "xtal";
	exynos4412_clocks[0].rate = xtal;

	exynos4412_clocks[1].name = "sclk_apll";
	exynos4412_clocks[1].rate = exynos4412_get_sclk_apll(xtal);

	exynos4412_clocks[2].name = "sclk_mpll";
	exynos4412_clocks[2].rate = exynos4412_get_sclk_mpll(xtal);

	exynos4412_clocks[3].name = "sclk_epll";
	exynos4412_clocks[3].rate = exynos4412_get_sclk_epll(xtal);

	exynos4412_clocks[4].name = "sclk_vpll";
	exynos4412_clocks[4].rate = exynos4412_get_sclk_vpll(xtal);

	exynos4412_clocks[5].name = "armclk";
	exynos4412_clocks[5].rate = exynos4412_get_armclk(xtal);

	exynos4412_clocks[6].name = "sclk_uart0";
	exynos4412_clocks[6].rate = exynos4412_get_sclk_uart(xtal, 0);

	exynos4412_clocks[7].name = "sclk_uart1";
	exynos4412_clocks[7].rate = exynos4412_get_sclk_uart(xtal, 1);

	exynos4412_clocks[8].name = "sclk_uart2";
	exynos4412_clocks[8].rate = exynos4412_get_sclk_uart(xtal, 2);

	exynos4412_clocks[9].name = "sclk_uart3";
	exynos4412_clocks[9].rate = exynos4412_get_sclk_uart(xtal, 3);

	exynos4412_clocks[10].name = "sclk_uart4";
	exynos4412_clocks[10].rate = exynos4412_get_sclk_uart(xtal, 4);
}

static __init void exynos4412_clk_init(void)
{
	u64_t xtal = 0;
	int i;

	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 24 * 1000 * 1000;

	exynos4412_setup_clocks(xtal);

	for(i=0; i< ARRAY_SIZE(exynos4412_clocks); i++)
	{
		if(clk_register(&exynos4412_clocks[i]))
			LOG("Register clock source '%s' [%LdHZ]", exynos4412_clocks[i].name, exynos4412_clocks[i].rate);
		else
			LOG("Failed to register clock source '%s' [%LdHZ]", exynos4412_clocks[i].name, exynos4412_clocks[i].rate);
	}
}

static __exit void exynos4412_clk_exit(void)
{
	int i;

	for(i=0; i< ARRAY_SIZE(exynos4412_clocks); i++)
	{
		if(clk_unregister(&exynos4412_clocks[i]))
			LOG("Unregister clock source '%s' [%LdHZ]", exynos4412_clocks[i].name, exynos4412_clocks[i].rate);
		else
			LOG("Failed to unregister clock '%s' [%LdHZ]", exynos4412_clocks[i].name, exynos4412_clocks[i].rate);
	}
}

core_initcall(exynos4412_clk_init);
core_exitcall(exynos4412_clk_exit);
