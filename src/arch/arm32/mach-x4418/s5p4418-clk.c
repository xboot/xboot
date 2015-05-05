/*
 * s5p4418-clk.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <s5p4418/reg-sys.h>

/*
 * CORE CLK
 */
static struct clk_fixed_t core_fixed_clks[] = {
	{
		.name = "PLLXTI",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "RTCXTI",
		.rate = 32768,
	}
};

static void core_pll_clk_set_rate(struct clk_pll_t * pclk, u64_t prate, u64_t rate)
{
	if(strcmp(pclk->name, "PLL0") == 0)
	{
	}
	else if(strcmp(pclk->name, "PLL1") == 0)
	{
	}
	else if(strcmp(pclk->name, "PLL2") == 0)
	{
	}
	else if(strcmp(pclk->name, "PLL3") == 0)
	{
	}
}

static u64_t core_pll_clk_get_rate(struct clk_pll_t * pclk, u64_t prate)
{
	u32_t r, p, m, s, k;
	u64_t fout = 0;

	if(strcmp(pclk->name, "PLL0") == 0)
	{
		/*
		 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
		 */
		r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG0));
		p = (r >> 18) & 0x3f;
		m = (r >> 8) & 0x3ff;
		s = (r >> 0) & 0xff;
		fout = m * (prate / (p * (1 << s)));
	}
	else if(strcmp(pclk->name, "PLL1") == 0)
	{
		/*
		 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
		 */
		r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG1));
		p = (r >> 18) & 0x3f;
		m = (r >> 8) & 0x3ff;
		s = (r >> 0) & 0xff;
		fout = m * (prate / (p * (1 << s)));
	}
	else if(strcmp(pclk->name, "PLL2") == 0)
	{
		/*
		 * FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV)
		 */
		r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG2));
		p = (r >> 18) & 0x3f;
		m = (r >> 8) & 0x3ff;
		s = (r >> 0) & 0xff;
		r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG2_SSCG));
		k = (r >> 16) & 0xffff;
		fout = (m + k / 65536) * (prate / (p * (1 << s)));
	}
	else if(strcmp(pclk->name, "PLL3") == 0)
	{
		/*
		 * FOUT = (MDIV + K / 65535) * FIN / (PDIV * 2^SDIV)
		 */
		r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG3));
		p = (r >> 18) & 0x3f;
		m = (r >> 8) & 0x3ff;
		s = (r >> 0) & 0xff;
		r = read32(phys_to_virt(S5P4418_SYS_PLLSETREG3_SSCG));
		k = (r >> 16) & 0xffff;
		fout = (m + k / 65536) * (prate / (p * (1 << s)));
	}

	return fout;
}

static struct clk_pll_t core_pll_clks[] = {
	{
		.name = "PLL0",
		.parent = "PLLXTI",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	}, {
		.name = "PLL1",
		.parent = "PLLXTI",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	}, {
		.name = "PLL2",
		.parent = "PLLXTI",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	}, {
		.name = "PLL3",
		.parent = "PLLXTI",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	},
};

static struct clk_mux_t core_mux_clks[] = {

};

static struct clk_divider_t core_div_clks[] = {

};

static struct clk_gate_t core_gate_clks[] = {

};

static void clk_register_core(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(core_fixed_clks); i++)
		clk_fixed_register(&core_fixed_clks[i]);
	for(i = 0; i < ARRAY_SIZE(core_pll_clks); i++)
		clk_pll_register(&core_pll_clks[i]);
	for(i = 0; i < ARRAY_SIZE(core_mux_clks); i++)
		clk_mux_register(&core_mux_clks[i]);
	for(i = 0; i < ARRAY_SIZE(core_div_clks); i++)
		clk_divider_register(&core_div_clks[i]);
	for(i = 0; i < ARRAY_SIZE(core_gate_clks); i++)
		clk_gate_register(&core_gate_clks[i]);
}

static void clk_unregister_core(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(core_fixed_clks); i++)
		clk_fixed_unregister(&core_fixed_clks[i]);
	for(i = 0; i< ARRAY_SIZE(core_pll_clks); i++)
		clk_pll_unregister(&core_pll_clks[i]);
	for(i = 0; i < ARRAY_SIZE(core_mux_clks); i++)
		clk_mux_unregister(&core_mux_clks[i]);
	for(i = 0; i < ARRAY_SIZE(core_div_clks); i++)
		clk_divider_unregister(&core_div_clks[i]);
	for(i = 0; i < ARRAY_SIZE(core_gate_clks); i++)
		clk_gate_unregister(&core_gate_clks[i]);
}

/*
 * DEFAULT ON/OFF CLKS
 */
static const char * default_off_clks[] = {

};

static const char * default_on_clks[] = {

};

static void default_clks_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(default_off_clks); i++)
		clk_disable(default_off_clks[i]);
	for(i = 0; i < ARRAY_SIZE(default_on_clks); i++)
		clk_enable(default_on_clks[i]);
}

static __init void s5p4418_clk_init(void)
{
	clk_register_core();
	default_clks_init();
}

static __exit void s5p4418_clk_exit(void)
{
	clk_unregister_core();
}

core_initcall(s5p4418_clk_init);
core_exitcall(s5p4418_clk_exit);
