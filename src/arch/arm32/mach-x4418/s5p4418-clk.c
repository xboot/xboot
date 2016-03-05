/*
 * s5p4418-clk.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <s5p4418-rstcon.h>
#include <s5p4418/reg-sys.h>
#include <s5p4418/reg-clk.h>
#include <s5p4418/reg-timer.h>
#include <s5p4418/reg-pwm.h>

/*
 * [CORE CLK]
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-FCLKCPU0 --> DIV-FCLKCPU0 --> DIV-HCLKCPU0
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-BCLK --> DIV-BCLK --> DIV-PCLK
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-MDCLK --> DIV-MDCLK --> DIV-MCLK --> DIV-MBCLK --> DIV-MPCLK
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-GR3DBCLK --> DIV-GR3DBCLK --> DIV-GR3DPCLK
 * PLLXTI --> PLL0 | PLL1 | PLL2 | PLL3 --> MUX-MPEGBCLK --> DIV-MPEGBCLK --> DIV-MPEGPCLK
 *
 * [TIMER / PWM CLK]
 * DIV-PCLK --> DIV-TIMER-PRESCALER0
 * DIV-PCLK --> DIV-TIMER-PRESCALER1
 * DIV-PCLK --> DIV-PWM-PRESCALER0
 * DIV-PCLK --> DIV-PWM-PRESCALER1
 *
 * [UART CLK]
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX-UART0 --> DIV-UART0 --> GATE-UART0
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX-UART1 --> DIV-UART1 --> GATE-UART1
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX-UART2 --> DIV-UART2 --> GATE-UART2
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX-UART3 --> DIV-UART3 --> GATE-UART3
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX-UART4 --> DIV-UART4 --> GATE-UART4
 * PLLXTI --> PLL0 | PLL1 | PLL2 --> MUX-UART5 --> DIV-UART5 --> GATE-UART5
 *
 */

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

static struct clk_mux_table_t pll_mux_tables[] = {
	{ .name = "PLL0",	.val = 0 },
	{ .name = "PLL1",	.val = 1 },
	{ .name = "PLL2",	.val = 2 },
	{ .name = "PLL3",	.val = 3 },
	{ 0, 0 },
};

static struct clk_mux_t core_mux_clks[] = {
	{
		.name = "MUX-FCLKCPU0",
		.parent = pll_mux_tables,
		.shift = 0,
		.width = 3,
		.phys = S5P4418_SYS_CLKDIVREG0,
	}, {
		.name = "MUX-BCLK",
		.parent = pll_mux_tables,
		.shift = 0,
		.width = 3,
		.phys = S5P4418_SYS_CLKDIVREG1,
	}, {
		.name = "MUX-MDCLK",
		.parent = pll_mux_tables,
		.shift = 0,
		.width = 3,
		.phys = S5P4418_SYS_CLKDIVREG2,
	}, {
		.name = "MUX-GR3DBCLK",
		.parent = pll_mux_tables,
		.shift = 0,
		.width = 3,
		.phys = S5P4418_SYS_CLKDIVREG3,
	}, {
		.name = "MUX-MPEGBCLK",
		.parent = pll_mux_tables,
		.shift = 0,
		.width = 3,
		.phys = S5P4418_SYS_CLKDIVREG4,
	}
};

static struct clk_divider_t core_div_clks[] = {
	{
		.name = "DIV-FCLKCPU0",
		.parent = "MUX-FCLKCPU0",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 3,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG0,
	}, {
		.name = "DIV-HCLKCPU0",
		.parent = "DIV-FCLKCPU0",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 9,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG0,
	}, {
		.name = "DIV-BCLK",
		.parent = "MUX-BCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 3,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG1,
	}, {
		.name = "DIV-PCLK",
		.parent = "DIV-BCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 9,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG1,
	}, {
		.name = "DIV-MDCLK",
		.parent = "MUX-MDCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 3,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG2,
	}, {
		.name = "DIV-MCLK",
		.parent = "DIV-MDCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 9,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG2,
	}, {
		.name = "DIV-MBCLK",
		.parent = "DIV-MCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 15,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG2,
	}, {
		.name = "DIV-MPCLK",
		.parent = "DIV-MBCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 21,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG2,
	}, {
		.name = "DIV-GR3DBCLK",
		.parent = "MUX-GR3DBCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 3,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG3,
	}, {
		.name = "DIV-GR3DPCLK",
		.parent = "DIV-GR3DBCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 9,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG3,
	}, {
		.name = "DIV-MPEGBCLK",
		.parent = "MUX-MPEGBCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 3,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG4,
	}, {
		.name = "DIV-MPEGPCLK",
		.parent = "DIV-MPEGBCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 9,
		.width = 6,
		.phys = S5P4418_SYS_CLKDIVREG4,
	}, {
		.name = "DIV-TIMER-PRESCALER0",
		.parent = "DIV-PCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 0,
		.width = 8,
		.phys = S5P4418_TIMER_BASE + TIMER_TCFG0,
	}, {
		.name = "DIV-TIMER-PRESCALER1",
		.parent = "DIV-PCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 8,
		.width = 8,
		.phys = S5P4418_TIMER_BASE + TIMER_TCFG0,
	}, {
		.name = "DIV-PWM-PRESCALER0",
		.parent = "DIV-PCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 0,
		.width = 8,
		.phys = S5P4418_PWM_BASE + PWM_TCFG0,
	}, {
		.name = "DIV-PWM-PRESCALER1",
		.parent = "DIV-PCLK",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 8,
		.width = 8,
		.phys = S5P4418_PWM_BASE + PWM_TCFG0,
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

/*
 * UART CLK
 */
static struct clk_mux_table_t uart_mux_tables[] = {
	{ .name = "PLL0",	.val = 0 },
	{ .name = "PLL1",	.val = 1 },
	{ .name = "PLL2",	.val = 2 },
	{ 0, 0 },
};

static struct clk_mux_t uart_mux_clks[] = {
	{
		.name = "MUX-UART0",
		.parent = uart_mux_tables,
		.shift = 2,
		.width = 3,
		.phys = S5P4418_CLK_UART0CLKGEN0L,
	}, {
		.name = "MUX-UART1",
		.parent = uart_mux_tables,
		.shift = 2,
		.width = 3,
		.phys = S5P4418_CLK_UART1CLKGEN0L,
	}, {
		.name = "MUX-UART2",
		.parent = uart_mux_tables,
		.shift = 2,
		.width = 3,
		.phys = S5P4418_CLK_UART2CLKGEN0L,
	}, {
		.name = "MUX-UART3",
		.parent = uart_mux_tables,
		.shift = 2,
		.width = 3,
		.phys = S5P4418_CLK_UART3CLKGEN0L,
	}, {
		.name = "MUX-UART4",
		.parent = uart_mux_tables,
		.shift = 2,
		.width = 3,
		.phys = S5P4418_CLK_UART4CLKGEN0L,
	}, {
		.name = "MUX-UART5",
		.parent = uart_mux_tables,
		.shift = 2,
		.width = 3,
		.phys = S5P4418_CLK_UART5CLKGEN0L,
	}
};

static struct clk_divider_t uart_div_clks[] = {
	{
		.name = "DIV-UART0",
		.parent = "MUX-UART0",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 5,
		.width = 8,
		.phys = S5P4418_CLK_UART0CLKGEN0L,
	}, {
		.name = "DIV-UART1",
		.parent = "MUX-UART1",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 5,
		.width = 8,
		.phys = S5P4418_CLK_UART1CLKGEN0L,
	}, {
		.name = "DIV-UART2",
		.parent = "MUX-UART2",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 5,
		.width = 8,
		.phys = S5P4418_CLK_UART2CLKGEN0L,
	}, {
		.name = "DIV-UART3",
		.parent = "MUX-UART3",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 5,
		.width = 8,
		.phys = S5P4418_CLK_UART3CLKGEN0L,
	}, {
		.name = "DIV-UART4",
		.parent = "MUX-UART4",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 5,
		.width = 8,
		.phys = S5P4418_CLK_UART4CLKGEN0L,
	}, {
		.name = "DIV-UART5",
		.parent = "MUX-UART5",
		.type = CLK_DIVIDER_ONE_BASED,
		.shift = 5,
		.width = 8,
		.phys = S5P4418_CLK_UART5CLKGEN0L,
	}
};

static struct clk_gate_t uart_gate_clks[] = {
	{
		.name = "GATE-UART0",
		.parent = "DIV-UART0",
		.shift = 1,
		.invert = 0,
		.phys = S5P4418_CLK_UART0CLKENB,
	}, {
		.name = "GATE-UART1",
		.parent = "DIV-UART1",
		.shift = 1,
		.invert = 0,
		.phys = S5P4418_CLK_UART1CLKENB,
	}, {
		.name = "GATE-UART2",
		.parent = "DIV-UART2",
		.shift = 1,
		.invert = 0,
		.phys = S5P4418_CLK_UART2CLKENB,
	}, {
		.name = "GATE-UART3",
		.parent = "DIV-UART3",
		.shift = 1,
		.invert = 0,
		.phys = S5P4418_CLK_UART3CLKENB,
	}, {
		.name = "GATE-UART4",
		.parent = "DIV-UART4",
		.shift = 1,
		.invert = 0,
		.phys = S5P4418_CLK_UART4CLKENB,
	}, {
		.name = "GATE-UART5",
		.parent = "DIV-UART5",
		.shift = 1,
		.invert = 0,
		.phys = S5P4418_CLK_UART5CLKENB,
	}
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

static __init void s5p4418_clk_init(void)
{
	/*
	 * Reset some IP modules.
	 */
	s5p4418_ip_reset(RESET_ID_TIMER, 0);
	s5p4418_ip_reset(RESET_ID_PWM, 0);
	s5p4418_ip_reset(RESET_ID_UART0, 0);
	s5p4418_ip_reset(RESET_ID_UART1, 0);
	s5p4418_ip_reset(RESET_ID_UART2, 0);
	s5p4418_ip_reset(RESET_ID_UART3, 0);

	clk_register_core();
	clk_register_uart();

	clk_set_rate("DIV-TIMER-PRESCALER0", 150 * 1000 * 1000);
	clk_set_rate("DIV-TIMER-PRESCALER1", 150 * 1000 * 1000);
	clk_set_rate("DIV-PWM-PRESCALER0", 150 * 1000 * 1000);
	clk_set_rate("DIV-PWM-PRESCALER1", 150 * 1000 * 1000);
	clk_set_rate("DIV-UART0", 12 * 1000 * 1000);
	clk_set_rate("DIV-UART1", 12 * 1000 * 1000);
	clk_set_rate("DIV-UART2", 12 * 1000 * 1000);
	clk_set_rate("DIV-UART3", 12 * 1000 * 1000);
}
core_initcall(s5p4418_clk_init);
