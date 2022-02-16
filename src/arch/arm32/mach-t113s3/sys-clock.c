/*
 * sys-clock.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
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
#include <t113/reg-ccu.h>

static inline void sdelay(int loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0"(loops));
}

static void set_pll_cpux_axi(void)
{
	uint32_t val;

	/* Select cpux clock src to osc24m, axi divide ratio is 3, system apb clk ratio is 4 */
	write32(T113_CCU_BASE + CCU_CPU_AXI_CFG_REG, (0 << 24) | (3 << 8) | (1 << 0));
	sdelay(1);

	/* Disable pll gating */
	val = read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
	val &= ~(1 << 27);
	write32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

	/* Enable pll ldo */
	val = read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
	val |= (1 << 30);
	write32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);
	sdelay(5);

	/* Set default clk to 1008mhz */
	val = read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
	val &= ~((0x3 << 16) | (0xff << 8) | (0x3 << 0));
	val |= (41 << 8);
	write32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

	/* Lock enable */
	val = read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
	val |= (1 << 29);
	write32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

	/* Enable pll */
	val = read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
	val |= (1 << 31);
	write32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

	/* Wait pll stable */
	while(!(read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG) & (0x1 << 28)));
	sdelay(20);

	/* Enable pll gating */
	val = read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
	val |= (1 << 27);
	write32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);

	/* Lock disable */
	val = read32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG);
	val &= ~(1 << 29);
	write32(T113_CCU_BASE + CCU_PLL_CPU_CTRL_REG, val);
	sdelay(1);

	/* Set and change cpu clk src */
	val = read32(T113_CCU_BASE + CCU_CPU_AXI_CFG_REG);
	val &= ~(0x07 << 24 | 0x3 << 16 | 0x3 << 8 | 0xf << 0);
	val |= (0x03 << 24 | 0x0 << 16 | 0x0 << 8 | 0x0 << 0);
	write32(T113_CCU_BASE + CCU_CPU_AXI_CFG_REG, val);
	sdelay(1);
}

static void set_pll_periph0(void)
{
	uint32_t val;

	/* Periph0 has been enabled */
	if(read32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG) & (1 << 31))
		return;

	/* Change psi src to osc24m */
	val = read32(T113_CCU_BASE + CCU_PSI_CLK_REG);
	val &= (~(0x3 << 24));
	write32(val, T113_CCU_BASE + CCU_PSI_CLK_REG);

	/* Set default val */
	write32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, 0x63 << 8);

	/* Lock enable */
	val = read32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
	val |= (1 << 29);
	write32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, val);

	/* Enabe pll 600m(1x) 1200m(2x) */
	val = read32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
	val |= (1 << 31);
	write32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, val);

	/* Wait pll stable */
	while(!(read32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG) & (0x1 << 28)));
	sdelay(20);

	/* Lock disable */
	val = read32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
	val &= ~(1 << 29);
	write32(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG, val);
}

static void set_ahb(void)
{
	write32(T113_CCU_BASE + CCU_PSI_CLK_REG, (2 << 0) | (0 << 8));
	write32(T113_CCU_BASE + CCU_PSI_CLK_REG, read32(T113_CCU_BASE + CCU_PSI_CLK_REG) | (0x03 << 24));
	sdelay(1);
}

static void set_apb(void)
{
	write32(T113_CCU_BASE + CCU_APB0_CLK_REG, (2 << 0) | (1 << 8));
	write32(T113_CCU_BASE + CCU_APB0_CLK_REG, (0x03 << 24) | read32(T113_CCU_BASE + CCU_APB0_CLK_REG));
	sdelay(1);
}

static void set_dma(void)
{
	/* Dma reset */
	write32(T113_CCU_BASE + CCU_DMA_BGR_REG, read32(T113_CCU_BASE + CCU_DMA_BGR_REG) | (1 << 16));
	sdelay(20);
	/* Enable gating clock for dma */
	write32(T113_CCU_BASE + CCU_DMA_BGR_REG, read32(T113_CCU_BASE + CCU_DMA_BGR_REG) | (1 << 0));
}

static void set_mbus(void)
{
	uint32_t val;

	/* Reset mbus domain */
	val = read32(T113_CCU_BASE + CCU_MBUS_CLK_REG);
	val |= (0x1 << 30);
	write32(T113_CCU_BASE + CCU_MBUS_CLK_REG, val);
	sdelay(1);
	/* Enable mbus master clock gating */
	write32(T113_CCU_BASE + CCU_MBUS_MAT_CLK_GATING_REG, 0x00000d87);
}

static void set_module(virtual_addr_t addr)
{
	uint32_t val;

	if(!(read32(addr) & (1 << 31)))
	{
		val = read32(addr);
		write32(addr, val | (1 << 31) | (1 << 30));

		/* Lock enable */
		val = read32(addr);
		val |= (1 << 29);
		write32(addr, val);

		/* Wait pll stable */
		while(!(read32(addr) & (0x1 << 28)));
		sdelay(20);

		/* Lock disable */
		val = read32(addr);
		val &= ~(1 << 29);
		write32(addr, val);
	}
}

void sys_clock_init(void)
{
	set_pll_cpux_axi();
	set_pll_periph0();
	set_ahb();
	set_apb();
	set_dma();
	set_mbus();
	set_module(T113_CCU_BASE + CCU_PLL_PERI0_CTRL_REG);
	set_module(T113_CCU_BASE + CCU_PLL_VIDEO0_CTRL_REG);
	set_module(T113_CCU_BASE + CCU_PLL_VIDEO1_CTRL_REG);
	set_module(T113_CCU_BASE + CCU_PLL_VE_CTRL);
	set_module(T113_CCU_BASE + CCU_PLL_AUDIO0_CTRL_REG);
	set_module(T113_CCU_BASE + CCU_PLL_AUDIO1_CTRL_REG);
}
