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
#include <v831/reg-ccu.h>

static inline void sdelay(int loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0"(loops));
}

static void clock_set_pll_cpu(u32_t clk)
{
	u32_t val;

	/* Switch to 24MHz clock while changing cpu pll */
	val = (2 << 0) | (3 << 8) | (0 << 24);
	write32(V831_CCU_BASE + CCU_CPUX_AXI_CFG, val);

	/* Disable cpu pll output */
	val = read32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL);
	val &= ~(0x1 << 27);
	write32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL, val);

	/* rate = 24000000 * n / p */
	val = read32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL);
	val &= ~((0x3 << 16) | (0xff << 8) | (0x3 << 0));
	val |= (0 << 16) | ((clk / 24000000 - 1) << 8) | (0 << 0);
	write32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL, val);
	sdelay(200);

	/* Enable cpu pll lock */
	val = read32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL);
	val |= (0x1 << 29);
	write32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL, val);

	/* Wait cpu pll lock */
	do {
		val = read32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL);
	} while(!(val & (0x1 << 28)));

	/* Eanble cpu pll output */
	val = read32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL);
	val |= (0x1 << 27);
	write32(V831_CCU_BASE + CCU_PLL_CPUX_CTRL, val);

	/* Switch clock source */
	val = read32(V831_CCU_BASE + CCU_CPUX_AXI_CFG);
	val &= ~(0x3 << 24);
	val |= (0x3 << 24);
	write32(V831_CCU_BASE + CCU_CPUX_AXI_CFG, val);
}

void sys_clock_init(void)
{
	clock_set_pll_cpu(816000000);

	/* pll video - 297MHZ */
	write32(V831_CCU_BASE + CCU_PLL_VIDEO0_CTRL, 0x08006203);

	/* pll periph0 - 600MHZ */
	write32(V831_CCU_BASE + CCU_PLL_PERI0_CTRL, 0xa8003100);
	while(!(read32(V831_CCU_BASE + CCU_PLL_PERI0_CTRL) & (1 << 28)));

	/* ahb1 = ahb2 = pll periph0 / (2 + 1) / (0 + 1) */
	write32(V831_CCU_BASE + CCU_PSI_AHB1_AHB2_CFG, 0x03000002);

	/* psi gating enable */
	write32(V831_CCU_BASE + 0x79c, 0x00010001);

	/* ahb3 = pll periph0 / (2 + 1) / (0 + 1) */
	write32(V831_CCU_BASE + CCU_AHB3_CFG, 0x03000002);

	/* apb1 = pll periph0 / (2 + 1) / (1 + 1) */
	write32(V831_CCU_BASE + CCU_APB1_CFG, 0x03000102);

	/* apb2 = osc24m / (0 + 1) / (0 + 1) */
	write32(V831_CCU_BASE + CCU_APB2_CFG, 0x00000000);

	/* mclk gating enable for core component */
	write32(V831_CCU_BASE + 0x0804, 0x08800707);
}
