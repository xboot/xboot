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

static struct clk_fixed_t fixed_clks[] = {
	{
		.name = "FIN",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "XXTI",
		.rate = 24 * 1000 * 1000,
	}, {
		.name = "XUSBTI",
		.rate = 24 * 1000 * 1000,
	}
};

static void pll_clk_set_rate(struct clk_pll_t * pclk, u64_t prate, u64_t rate)
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

static u64_t pll_clk_get_rate(struct clk_pll_t * pclk, u64_t prate)
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

static struct clk_pll_t pll_clks[] = {
	{
		.name = "APLL",
		.parent = "FIN",
		.set_rate = pll_clk_set_rate,
		.get_rate = pll_clk_get_rate,
	}, {
		.name = "MPLL",
		.parent = "FIN",
		.set_rate = pll_clk_set_rate,
		.get_rate = pll_clk_get_rate,
	}, {
		.name = "EPLL",
		.parent = "FIN",
		.set_rate = pll_clk_set_rate,
		.get_rate = pll_clk_get_rate,
	}, {
		.name = "VPLL",
		.parent = "FIN",
		.set_rate = pll_clk_set_rate,
		.get_rate = pll_clk_get_rate,
	},
};

static struct clk_mux_table_t mux_apll_table[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "APLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mux_mpll_table[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mux_core_table[] = {
	{ .name = "MUX-APLL",	.val = 0 },
	{ .name = "MUX-MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mux_hpm_table[] = {
	{ .name = "MUX-APLL",	.val = 0 },
	{ .name = "MUX-MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mux_epll_table[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "EPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mux_vpll_table[] = {
	{ .name = "FIN",	.val = 0 },
	{ .name = "VPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mux_mpll_ctrl_user_t_table[] = {
	{ .name = "FIN",		.val = 0 },
	{ .name = "MUX-MPLL",	.val = 1 },
	{ 0, 0 },
};

static struct clk_mux_table_t mux_uart_table[] = {
	{ .name = "XXTI",					.val = 0 },
	{ .name = "XUSBTI",					.val = 1 },
	{ .name = "SCLK_HDMI24M",			.val = 2 },
	{ .name = "SCLK_USBPHY0",			.val = 3 },
	{ .name = "SCLK_HDMIPHY",			.val = 5 },
	{ .name = "MUX-MPLL-CTRL-USER-T",	.val = 6 },
	{ .name = "MUX-EPLL",				.val = 7 },
	{ .name = "MUX-VPLL",				.val = 8 },
	{ 0, 0 },
};

static struct clk_mux_t mux_clks[] = {
	{
		.name = "MUX-APLL",
		.parent = mux_apll_table,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 0,
		.width = 1,
	}, {
		.name = "MUX-MPLL",
		.parent = mux_mpll_table,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 24,
		.width = 1,
	}, {
		.name = "MUX-CORE",
		.parent = mux_core_table,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 16,
		.width = 1,
	}, {
		.name = "MUX-HPM",
		.parent = mux_hpm_table,
		.reg = EXYNOS4412_CLK_SRC_CPU,
		.shift = 20,
		.width = 1,
	}, {
		.name = "MUX-EPLL",
		.parent = mux_epll_table,
		.reg = EXYNOS4412_CLK_SRC_TOP0,
		.shift = 4,
		.width = 1,
	}, {
		.name = "MUX-VPLL",
		.parent = mux_vpll_table,
		.reg = EXYNOS4412_CLK_SRC_TOP0,
		.shift = 8,
		.width = 1,
	}, {
		.name = "MUX-MPLL-CTRL-USER-T",
		.parent = mux_mpll_ctrl_user_t_table,
		.reg = EXYNOS4412_CLK_SRC_TOP1,
		.shift = 12,
		.width = 1,
	}, {
		.name = "MUX-UART0",
		.parent = mux_uart_table,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 0,
		.width = 4,
	}, {
		.name = "MUX-UART1",
		.parent = mux_uart_table,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 4,
		.width = 4,
	}, {
		.name = "MUX-UART2",
		.parent = mux_uart_table,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 8,
		.width = 4,
	}, {
		.name = "MUX-UART3",
		.parent = mux_uart_table,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 12,
		.width = 4,
	}, {
		.name = "MUX-UART4",
		.parent = mux_uart_table,
		.reg = EXYNOS4412_CLK_SRC_PERIL0,
		.shift = 16,
		.width = 4,
	},
};

static struct clk_divider_t div_clks[] = {
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
	}, {
		.name = "DIV-UART0",
		.parent = "MUX-UART0",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 0,
		.width = 4,
	}, {
		.name = "DIV-UART1",
		.parent = "MUX-UART1",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 4,
		.width = 4,
	}, {
		.name = "DIV-UART2",
		.parent = "MUX-UART2",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 8,
		.width = 4,
	}, {
		.name = "DIV-UART3",
		.parent = "MUX-UART3",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 12,
		.width = 4,
	}, {
		.name = "DIV-UART4",
		.parent = "MUX-UART4",
		.reg = EXYNOS4412_CLK_DIV_PERIL0,
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 16,
		.width = 4,
	}
};

struct clk_gate_t gate_clks[] = {
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

static __init void exynos4412_clk_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(fixed_clks); i++)
		clk_fixed_register(&fixed_clks[i]);

	for(i = 0; i < ARRAY_SIZE(pll_clks); i++)
		clk_pll_register(&pll_clks[i]);

	for(i = 0; i < ARRAY_SIZE(mux_clks); i++)
		clk_mux_register(&mux_clks[i]);

	for(i = 0; i < ARRAY_SIZE(div_clks); i++)
		clk_divider_register(&div_clks[i]);

	for(i = 0; i < ARRAY_SIZE(gate_clks); i++)
		clk_gate_register(&gate_clks[i]);
}

static __exit void exynos4412_clk_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(fixed_clks); i++)
		clk_fixed_unregister(&fixed_clks[i]);

	for(i = 0; i< ARRAY_SIZE(pll_clks); i++)
		clk_pll_unregister(&pll_clks[i]);

	for(i = 0; i < ARRAY_SIZE(mux_clks); i++)
		clk_mux_unregister(&mux_clks[i]);

	for(i = 0; i < ARRAY_SIZE(div_clks); i++)
		clk_divider_unregister(&div_clks[i]);

	for(i = 0; i < ARRAY_SIZE(gate_clks); i++)
		clk_gate_unregister(&gate_clks[i]);
}

core_initcall(exynos4412_clk_init);
core_exitcall(exynos4412_clk_exit);
