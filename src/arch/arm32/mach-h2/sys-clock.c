/*
 * sys-clock.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <h2/reg-ccu.h>

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
