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
#include <exynos4412/reg-pmu.h>
#include <exynos4412/reg-clk.h>

/*
 * CORE CLK
 */
static struct clk_fixed_t core_fixed_clks[] = {
	{
		.name = "XXTI",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "XUSBXTI",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "USBPHY0",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "USBPHY1",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "MIPIPHY",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "HDMIPHY",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "HDMI24M",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "XRTCXTI",
		.rate = 32768,
	}
};

static void core_pll_clk_set_rate(struct clk_pll_t * pclk, u64_t prate, u64_t rate)
{
	if(strcmp(pclk->name, "APLL") == 0)
	{
	}
	else if(strcmp(pclk->name, "MPLL") == 0)
	{
	}
	else if(strcmp(pclk->name, "EPLL") == 0)
	{
	}
	else if(strcmp(pclk->name, "VPLL") == 0)
	{
	}
}

static u64_t core_pll_clk_get_rate(struct clk_pll_t * pclk, u64_t prate)
{
	u32_t r, k, m, p, s;
	u64_t fout = 0;

	if(strcmp(pclk->name, "APLL") == 0)
	{
		/*
		 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
		 */
		r = readl(EXYNOS4412_APLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (prate / (p * (1 << s)));
	}
	else if(strcmp(pclk->name, "MPLL") == 0)
	{
		/*
		 * FOUT = MDIV * FIN / (PDIV * 2^(SDIV))
		 */
		r = readl(EXYNOS4412_MPLL_CON0);
		m = (r >> 16) & 0x3ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		fout = m * (prate / (p * (1 << s)));
	}
	else if(strcmp(pclk->name, "EPLL") == 0)
	{
		/*
		 * FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV)
		 */
		r = readl(EXYNOS4412_EPLL_CON0);
		k = readl(EXYNOS4412_EPLL_CON1);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		k = k & 0xffff;
		fout = (m + k / 65536) * (prate / (p * (1 << s)));
	}
	else if(strcmp(pclk->name, "VPLL") == 0)
	{
		/*
		 * FOUT = (MDIV + K / 65535) * FIN / (PDIV * 2^SDIV)
		 */
		r = readl(EXYNOS4412_VPLL_CON0);
		k = readl(EXYNOS4412_VPLL_CON1);
		m = (r >> 16) & 0x1ff;
		p = (r >> 8) & 0x3f;
		s = r & 0x7;
		k = k & 0xffff;
		fout = (m + k / 65535) * (prate / (p * (1 << s)));
	}

	return fout;
}

static struct clk_pll_t core_pll_clks[] = {
	{
		.name = "APLL",
		.parent = "FIN",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	}, {
		.name = "MPLL",
		.parent = "FIN",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	}, {
		.name = "EPLL",
		.parent = "FIN",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	}, {
		.name = "VPLL",
		.parent = "FIN",
		.set_rate = core_pll_clk_set_rate,
		.get_rate = core_pll_clk_get_rate,
	},
};

static struct clk_mux_table_t fin_mux_tables[] = {
	{ .name = "XXTI",		.val = 0 },
	{ .name = "XUSBXTI",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t apll_mux_tables[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "APLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mpll_mux_tables[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t core_mux_tables[] = {
	{ .name = "MUX-APLL",	.val = 0 },
	{ .name = "MUX-MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t hpm_mux_tables[] = {
	{ .name = "MUX-APLL",	.val = 0 },
	{ .name = "MUX-MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t epll_mux_tables[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "EPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t vpll_mux_tables[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "VPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mpll_user_t_mux_tables[] = {
	{ .name = "FIN",		.val = 0 },
	{ .name = "MUX-MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_t core_mux_clks[] = {
	{
		.name = "FIN",
		.parent = fin_mux_tables,
		.reg = EXYNOS4412_PMU_OM_STAT,
		.shift = 0,
		.width = 1,
	}, {
		.name = "MUX-APLL",
		.parent = apll_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 0,
		.width = 1,
	}, {
		.name = "MUX-MPLL",
		.parent = mpll_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 24,
		.width = 1,
	}, {
		.name = "MUX-CORE",
		.parent = core_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 16,
		.width = 1,
	}, {
		.name = "MUX-HPM",
		.parent = hpm_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 20,
		.width = 1,
	}, {
		.name = "MUX-EPLL",
		.parent = epll_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_TOP0,
		.shift = 4,
		.width = 1,
	}, {
		.name = "MUX-VPLL",
		.parent = vpll_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_TOP0,
		.shift = 8,
		.width = 1,
	}, {
		.name = "MUX-MPLL-USER-T",
		.parent = mpll_user_t_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_TOP1,
		.shift = 12,
		.width = 1,
	},
};

static struct clk_divider_t core_div_clks[] = {
	{
		.name = "DIV-APLL",
		.parent = "MUX-APLL",
		.reg = EXYNOS4412_CLK_DIV_CPU0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 24,
		.width = 3,
	}, {
		.name = "DIV-CORE",
		.parent = "MUX-CORE",
		.reg = EXYNOS4412_CLK_DIV_CPU0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 0,
		.width = 3,
	}, {
		.name = "DIV-CORE2",
		.parent = "DIV-CORE",
		.reg = EXYNOS4412_CLK_DIV_CPU0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 28,
		.width = 3,
	}, {
		.name = "DIV-CORES",
		.parent = "DIV-CORE2",
		.reg = EXYNOS4412_CLK_DIV_CPU1,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 8,
		.width = 3,
	}, {
		.name = "DIV-COPY",
		.parent = "MUX-HPM",
		.reg = EXYNOS4412_CLK_DIV_CPU1,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 0,
		.width = 3,
	}, {
		.name = "DIV-HPM",
		.parent = "DIV-COPY",
		.reg = EXYNOS4412_CLK_DIV_CPU1,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 4,
		.width = 3,
	},
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
 * UART CLK
 *
 * XXTI-------------\
 * XUSBXTI----------|
 * HDMI24M----------|
 * USBPHY0----------|
 * HDMIPHY----------|--> MUX-UART --> GATE-MUX-UART --> DIV-UART --> GATE-UART
 * MUX-MPLL-USER-T--|
 * MUX-EPLL---------|
 * MUX-VPLL---------/
 */
static struct clk_mux_table_t uart_mux_tables[] = {
	{ .name = "XXTI",					.val = 0 },
	{ .name = "XUSBXTI",				.val = 1 },
	{ .name = "HDMI24M",				.val = 2 },
	{ .name = "USBPHY0",				.val = 3 },
	{ .name = "HDMIPHY",				.val = 5 },
	{ .name = "MUX-MPLL-USER-T",		.val = 6 },
	{ .name = "MUX-EPLL",				.val = 7 },
	{ .name = "MUX-VPLL",				.val = 8 },
	{ 0, 0 },
};

static struct clk_mux_t uart_mux_clks[] = {
	{
		.name = "MUX-UART0",
		.parent = uart_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 0,
		.width = 4,
	}, {
		.name = "MUX-UART1",
		.parent = uart_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 4,
		.width = 4,
	}, {
		.name = "MUX-UART2",
		.parent = uart_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 8,
		.width = 4,
	}, {
		.name = "MUX-UART3",
		.parent = uart_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 12,
		.width = 4,
	}, {
		.name = "MUX-UART4",
		.parent = uart_mux_tables,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 16,
		.width = 4,
	},
};

static struct clk_divider_t uart_div_clks[] = {
	{
		.name = "DIV-UART0",
		.parent = "GATE-MUX-UART0",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 0,
		.width = 4,
	}, {
		.name = "DIV-UART1",
		.parent = "GATE-MUX-UART1",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 4,
		.width = 4,
	}, {
		.name = "DIV-UART2",
		.parent = "GATE-MUX-UART2",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 8,
		.width = 4,
	}, {
		.name = "DIV-UART3",
		.parent = "GATE-MUX-UART3",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 12,
		.width = 4,
	}, {
		.name = "DIV-UART4",
		.parent = "GATE-MUX-UART4",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 16,
		.width = 4,
	},
};

static struct clk_gate_t uart_gate_clks[] = {
	/* GATE-MUX-UART */
	{
		.name = "GATE-MUX-UART0",
		.parent = "MUX-UART0",
		.reg = EXYNOS4412_CLK_SRC_MASK_PERIL0,
		.shift = 0,
		.invert = 0,
	}, {
		.name = "GATE-MUX-UART1",
		.parent = "MUX-UART1",
		.reg = EXYNOS4412_CLK_SRC_MASK_PERIL0,
		.shift = 4,
		.invert = 0,
	}, {
		.name = "GATE-MUX-UART2",
		.parent = "MUX-UART2",
		.reg = EXYNOS4412_CLK_SRC_MASK_PERIL0,
		.shift = 8,
		.invert = 0,
	}, {
		.name = "GATE-MUX-UART3",
		.parent = "MUX-UART3",
		.reg = EXYNOS4412_CLK_SRC_MASK_PERIL0,
		.shift = 12,
		.invert = 0,
	}, {
		.name = "GATE-MUX-UART4",
		.parent = "MUX-UART4",
		.reg = EXYNOS4412_CLK_SRC_MASK_PERIL0,
		.shift = 16,
		.invert = 0,
	},
	/* GATE-UART */
	{
		.name = "GATE-UART0",
		.parent = "DIV-UART0",
		.reg = EXYNOS4412_CLK_GATE_IP_PERIL,
		.shift = 0,
		.invert = 0,
	}, {
		.name = "GATE-UART1",
		.parent = "DIV-UART1",
		.reg = EXYNOS4412_CLK_GATE_IP_PERIL,
		.shift = 1,
		.invert = 0,
	}, {
		.name = "GATE-UART2",
		.parent = "DIV-UART2",
		.reg = EXYNOS4412_CLK_GATE_IP_PERIL,
		.shift = 2,
		.invert = 0,
	}, {
		.name = "GATE-UART3",
		.parent = "DIV-UART3",
		.reg = EXYNOS4412_CLK_GATE_IP_PERIL,
		.shift = 3,
		.invert = 0,
	}, {
		.name = "GATE-UART4",
		.parent = "DIV-UART4",
		.reg = EXYNOS4412_CLK_GATE_IP_PERIL,
		.shift = 4,
		.invert = 0,
	},
};

static void clk_register_uart(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(uart_mux_clks); i++)
		clk_mux_register(&uart_mux_clks[i]);
	for(i = 0; i < ARRAY_SIZE(uart_div_clks); i++)
		clk_divider_register(&uart_div_clks[i]);
	for(i = 0; i < ARRAY_SIZE(uart_gate_clks); i++)
		clk_gate_register(&uart_gate_clks[i]);
}

static void clk_unregister_uart(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(uart_mux_clks); i++)
		clk_mux_unregister(&uart_mux_clks[i]);
	for(i = 0; i < ARRAY_SIZE(uart_div_clks); i++)
		clk_divider_unregister(&uart_div_clks[i]);
	for(i = 0; i < ARRAY_SIZE(uart_gate_clks); i++)
		clk_gate_unregister(&uart_gate_clks[i]);
}

/*
 * DEFAULT ON/OFF CLKS
 */
static const char * default_off_clks[] = {
	"GATE-UART0",
	"GATE-UART1",
	"GATE-UART2",
	"GATE-UART3",
	"GATE-UART4",
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

static __init void exynos4412_clk_init(void)
{
	clk_register_core();
	clk_register_uart();

	default_clks_init();
}

static __exit void exynos4412_clk_exit(void)
{
	clk_unregister_core();
	clk_unregister_uart();
}

core_initcall(exynos4412_clk_init);
core_exitcall(exynos4412_clk_exit);
