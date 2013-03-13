/*
 * arch/arm/mach-sbc2410x/s3c2410-clk.c
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
#include <s3c2410/reg-clk.h>


/*
 * the array of clocks, which will to be setup.
 */
static struct clk s3c2410_clocks[4];

/*
 * setup the s3c2410's clock array.
 */
static void s3c2410_setup_clocks(u64_t xtal)
{
	u32_t pllval, mdiv, pdiv, sdiv;
	u64_t fclk, hclk, pclk;
	u32_t tmp;

	/*
	 * now we've got our machine bits initialised, work out what
	 * clocks we've got
	 */
	pllval = readl(S3C2410_MPLLCON);

	mdiv = pllval >> S3C2410_PLLCON_MDIVSHIFT;
	pdiv = pllval >> S3C2410_PLLCON_PDIVSHIFT;
	sdiv = pllval >> S3C2410_PLLCON_SDIVSHIFT;

	mdiv &= S3C2410_PLLCON_MDIVMASK;
	pdiv &= S3C2410_PLLCON_PDIVMASK;
	sdiv &= S3C2410_PLLCON_SDIVMASK;

	fclk = (u64_t)xtal * (u64_t)(mdiv + 8);
	div64_64(&fclk, (pdiv + 2) << sdiv);

	tmp = readl(S3C2410_CLKDIVN);

	/* work out clock scalings */
	hclk = div64(fclk, ((tmp & S3C2410_CLKDIVN_HDIVN) ? 2 : 1));
	pclk = div64(hclk, ((tmp & S3C2410_CLKDIVN_PDIVN) ? 2 : 1));

	/* intialize system clocks */
	s3c2410_clocks[0].name = "xtal";
	s3c2410_clocks[0].rate = xtal;

	s3c2410_clocks[1].name = "fclk";
	s3c2410_clocks[1].rate = fclk;

	s3c2410_clocks[2].name = "hclk";
	s3c2410_clocks[2].rate = hclk;

	s3c2410_clocks[3].name = "pclk";
	s3c2410_clocks[3].rate = pclk;
}

static __init void s3c2410_clk_init(void)
{
	u32_t i;
	u64_t xtal = 0;

	/* get system xtal. */
	if(get_machine() != 0)
		xtal = (get_machine())->res.xtal;
	if(xtal == 0)
		xtal = 12*1000*1000;

	/* setup clock arrays */
	s3c2410_setup_clocks(xtal);

	/* register clocks to system */
	for(i=0; i< ARRAY_SIZE(s3c2410_clocks); i++)
	{
		if(!clk_register(&s3c2410_clocks[i]))
		{
			LOG_E("failed to register clock '%s'", s3c2410_clocks[i].name);
		}
	}
}

static __exit void s3c2410_clk_exit(void)
{
	u32_t i;

	for(i=0; i< ARRAY_SIZE(s3c2410_clocks); i++)
	{
		if(!clk_unregister(&s3c2410_clocks[i]))
		{
			LOG_E("failed to unregister clock '%s'", s3c2410_clocks[i].name);
		}
	}
}

core_initcall(s3c2410_clk_init);
core_exitcall(s3c2410_clk_exit);
