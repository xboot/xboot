/*
 * sys-clock.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <h3/reg-ccu.h>

static inline void sdelay(int loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0"(loops));
}

static void clock_set_pll_cpux(u32_t clk)
{
	int p = 0;
	int k = 1;
	int m = 1;
	u32_t val;

	if(clk > 1152000000)
	{
		k = 2;
	}
	else if(clk > 768000000)
	{
		k = 3;
		m = 2;
	}

	/* Switch to 24MHz clock while changing cpux pll */
	val = (2 << 0) | (1 << 8) | (1 << 16);
	write32(H3_CCU_BASE + CCU_CPUX_AXI_CFG, val);

	/* cpu pll rate = ((24000000 * n * k) >> p) / m */
	val = (0x1 << 31);
	val |= ((p & 0x3) << 16);
	val |= ((((clk / (24000000 * k / m)) - 1) & 0x1f) << 8);
	val |= (((k - 1) & 0x3) << 4);
	val |= (((m - 1) & 0x3) << 0);
	write32(H3_CCU_BASE + CCU_PLL_CPUX_CTRL, val);
	sdelay(200);

	/* Switch clock source */
	val = (2 << 0) | (1 << 8) | (2 << 16);
	write32(H3_CCU_BASE + CCU_CPUX_AXI_CFG, val);
}

void sys_clock_init(void)
{
	clock_set_pll_cpux(1008000000);

	/* pll periph0 - 600MHZ */
	write32(H3_CCU_BASE + CCU_PLL_PERIPH0_CTRL, 0x90041811);
	while(!(read32(H3_CCU_BASE + CCU_PLL_PERIPH0_CTRL) & (1 << 28)));

	/* ahb1 = pll periph0 / 3, apb1 = ahb1 / 2 */
	write32(H3_CCU_BASE + CCU_AHB1_APB1_CFG, 0x00003180);

	/* mbus  = pll periph0 / 2 */
	write32(H3_CCU_BASE + CCU_MBUS_CLK, 0x81000001);
}
