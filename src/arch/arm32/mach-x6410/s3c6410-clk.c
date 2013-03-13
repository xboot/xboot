/*
 * arch/arm/mach-x6410/s3c6410-clk.c
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
#include <s3c6410/reg-clk.h>


/*
 * the array of clocks, which will to be setup.
 */
static struct clk s3c6410_clocks[13];

/*
 * get pll.
 */
static inline u64_t s3c6410_get_pll(u64_t baseclk, u32_t pllcon)
{
	u32_t mdiv, pdiv, sdiv;
	u64_t fvco = baseclk;

	mdiv = (pllcon >> 16) & ((1 << (25-16+1)) - 1);
	pdiv = (pllcon >> 8) & ((1 << (13-8+1)) - 1);
	sdiv = (pllcon >> 0) & ((1 << (2-0+1)) - 1);

	fvco *= mdiv;
	div64_64(&fvco, (pdiv << sdiv));

	return (u64_t)fvco;
}

/*
 * get epll
 */
static inline u64_t s3c6410_get_epll(u64_t baseclk)
{
	u64_t result;
	u32_t epll0 = readl(S3C6410_EPLL_CON0);
	u32_t epll1 = readl(S3C6410_EPLL_CON1);
	u32_t mdiv, pdiv, sdiv, kdiv;
	u64_t tmp;

	mdiv = (epll0 >> 16) & ((1 << (25-16+1)) - 1);
	pdiv = (epll0 >> 8) & ((1 << (13-8+1)) - 1);
	sdiv = (epll0 >> 0) & ((1 << (2-0+1)) - 1);
	kdiv = epll1 & 0xffff;

	/*
	 * We need to multiple baseclk by mdiv (the integer part) and kdiv
	 * which is in 2^16ths, so shift mdiv up (does not overflow) and
	 * add kdiv before multiplying. The use of tmp is to avoid any
	 * overflows before shifting bac down into result when multipling
	 * by the mdiv and kdiv pair.
	 */
	tmp = baseclk;
	tmp *= (mdiv << 16) + kdiv;
	div64_64(&tmp, (pdiv << sdiv));
	result = tmp >> 16;

	return result;
}

/*
 * setup the s3c6410's clock array.
 */
static void s3c6410_setup_clocks(u64_t xtal)
{
	u64_t apll, mpll, mpll_dout, epll;
	u64_t fclk, hclk, hclk2, pclk;
	u32_t clkdiv0, clkdiv1, clkdiv2;
	u32_t clksrc, tmp;

	/* get value of clkdiv0 and clksrc register */
	clkdiv0 = readl(S3C6410_CLK_DIV0);
	clkdiv1 = readl(S3C6410_CLK_DIV1);
	clkdiv2 = readl(S3C6410_CLK_DIV2);
	clksrc = readl(S3C6410_CLK_SRC);

	/* get apll clock */
	if(clksrc & 0x0000001)
		apll = s3c6410_get_pll(xtal, readl(S3C6410_APLL_CON));
	else
		apll = xtal;

	/* get mpll clock */
	if(clksrc & 0x0000002)
		mpll = s3c6410_get_pll(xtal, readl(S3C6410_MPLL_CON));
	else
		mpll = xtal;

	/* get epll clock */
	if(clksrc & 0x0000004)
		epll = s3c6410_get_epll(xtal);
	else
		epll = xtal;

	/* get mpll dout clock */
	mpll_dout = div64(mpll, ((((clkdiv0) & S3C6410_CLKDIV0_MPLL_MASK) >> S3C6410_CLKDIV0_MPLL_SHIFT) + 1));

	/* get fclk clock */
	fclk = div64(apll, ((((clkdiv0) & S3C6410_CLKDIV0_ARM_MASK) >> S3C6410_CLKDIV0_ARM_SHIFT) + 1));

	/* get hclk2 clock */
	if(readl(S3C6410_OTHERS) & 0x40)
		hclk2 = div64(apll, ((((clkdiv0) & S3C6410_CLKDIV0_HCLK2_MASK) >> S3C6410_CLKDIV0_HCLK2_SHIFT) + 1));
	else
		hclk2 = div64(mpll, ((((clkdiv0) & S3C6410_CLKDIV0_HCLK2_MASK) >> S3C6410_CLKDIV0_HCLK2_SHIFT) + 1));

	/* get hclk clock */
	hclk = div64(hclk2, ((((clkdiv0) & S3C6410_CLKDIV0_HCLK_MASK) >> S3C6410_CLKDIV0_HCLK_SHIFT) + 1));

	/* get pclk clock */
	pclk = div64(hclk2, ((((clkdiv0) & S3C6410_CLKDIV0_PCLK_MASK) >> S3C6410_CLKDIV0_PCLK_SHIFT) + 1));

	/* extern clock */
	s3c6410_clocks[0].name = "xtal";
	s3c6410_clocks[0].rate = xtal;

	s3c6410_clocks[1].name = "fclk";
	s3c6410_clocks[1].rate = fclk;

	s3c6410_clocks[2].name = "hclk2";
	s3c6410_clocks[2].rate = hclk2;

	s3c6410_clocks[3].name = "hclk";
	s3c6410_clocks[3].rate = hclk;

	s3c6410_clocks[4].name = "pclk";
	s3c6410_clocks[4].rate = pclk;

	/* get jpeg clock */
	s3c6410_clocks[5].name = "jpeg";
	s3c6410_clocks[5].rate = div64(hclk2, ((((clkdiv0) & S3C6410_CLKDIV0_JPEG_MASK) >> S3C6410_CLKDIV0_JPEG_SHIFT) + 1));

	/* get security clock */
	s3c6410_clocks[6].name = "secur";
	s3c6410_clocks[6].rate = div64(hclk2, ((((clkdiv0) & S3C6410_CLKDIV0_SECURITY_MASK) >> S3C6410_CLKDIV0_SECURITY_SHIFT) + 1));

	/* get camera clock */
	s3c6410_clocks[7].name = "cam";
	s3c6410_clocks[7].rate = div64(hclk2, ((((clkdiv0) & S3C6410_CLKDIV0_CAM_MASK) >> S3C6410_CLKDIV0_CAM_SHIFT) + 1));

	/* get mfc clock */
	s3c6410_clocks[8].name = "mfc";
	if(clksrc & 0x0000008)
		s3c6410_clocks[8].rate = div64(epll, ((((clkdiv0) & S3C6410_CLKDIV0_MFC_MASK) >> S3C6410_CLKDIV0_MFC_SHIFT) + 1));
	else
		s3c6410_clocks[8].rate = div64(hclk2, ((((clkdiv0) & S3C6410_CLKDIV0_MFC_MASK) >> S3C6410_CLKDIV0_MFC_SHIFT) + 1));

	/* get uclk1 clock */
	s3c6410_clocks[9].name = "uclk1";
	if(clksrc & (0x1<<13))
		s3c6410_clocks[9].rate = div64(mpll_dout, ((((clkdiv2) & S3C6410_CLKDIV2_UART_MASK) >> S3C6410_CLKDIV2_UART_SHIFT) + 1));
	else
		s3c6410_clocks[9].rate = div64(epll, ((((clkdiv2) & S3C6410_CLKDIV2_UART_MASK) >> S3C6410_CLKDIV2_UART_SHIFT) + 1));

	/* get lcd clock */
	s3c6410_clocks[10].name = "lcd";
	tmp = (clksrc >> 26) & 0x3;
	switch (tmp)
	{
	case 0:
		s3c6410_clocks[10].rate = div64(epll, ((((clkdiv1) & S3C6410_CLKDIV1_LCD_MASK) >> S3C6410_CLKDIV1_LCD_SHIFT) + 1));
		break;
	case 1:
		s3c6410_clocks[10].rate = div64(mpll_dout, ((((clkdiv1) & S3C6410_CLKDIV1_LCD_MASK) >> S3C6410_CLKDIV1_LCD_SHIFT) + 1));
		break;
	case 2:
		s3c6410_clocks[10].rate = div64(xtal, ((((clkdiv1) & S3C6410_CLKDIV1_LCD_MASK) >> S3C6410_CLKDIV1_LCD_SHIFT) + 1));
		break;
	default:
		s3c6410_clocks[10].rate = 0;
		break;
	}

	/* get lcd post clock */
	s3c6410_clocks[11].name = "post";
	s3c6410_clocks[11].rate = s3c6410_clocks[10].rate;

	/* get sclaer clock */
	s3c6410_clocks[12].name = "sclaer";
	tmp = (clksrc >> 28) & 0x3;
	switch (tmp)
	{
	case 0:
		s3c6410_clocks[12].rate = div64(epll, ((((clkdiv1) & S3C6410_CLKDIV1_SCALER_MASK) >> S3C6410_CLKDIV1_SCALER_SHIFT) + 1));
		break;
	case 1:
		s3c6410_clocks[12].rate = div64(mpll_dout, ((((clkdiv1) & S3C6410_CLKDIV1_SCALER_MASK) >> S3C6410_CLKDIV1_SCALER_SHIFT) + 1));
		break;
	case 2:
		s3c6410_clocks[12].rate = div64(xtal, ((((clkdiv1) & S3C6410_CLKDIV1_SCALER_MASK) >> S3C6410_CLKDIV1_SCALER_SHIFT) + 1));
		break;
	default:
		s3c6410_clocks[12].rate = 0;
		break;
	}
}

static __init void s3c6410_clk_init(void)
{
	u32_t i;
	u64_t xtal = 0;

	/* get system xtal. */
	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 12*1000*1000;

	/* setup clock arrays */
	s3c6410_setup_clocks(xtal);

	/* register clocks to system */
	for(i=0; i< ARRAY_SIZE(s3c6410_clocks); i++)
	{
		if(!clk_register(&s3c6410_clocks[i]))
		{
			LOG_E("failed to register clock '%s'", s3c6410_clocks[i].name);
		}
	}
}

static __exit void s3c6410_clk_exit(void)
{
	u32_t i;

	for(i=0; i< ARRAY_SIZE(s3c6410_clocks); i++)
	{
		if(!clk_unregister(&s3c6410_clocks[i]))
		{
			LOG_E("failed to unregister clock '%s'", s3c6410_clocks[i].name);
		}
	}
}

core_initcall(s3c6410_clk_init);
core_exitcall(s3c6410_clk_exit);
