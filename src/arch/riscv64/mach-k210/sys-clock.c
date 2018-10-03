/*
 * sys-clock.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <k210/reg-sysctl.h>

#undef SYSCTL_PLL0
#undef SYSCTL_PLL1
#undef SYSCTL_PLL2
#include <sysctl.h>

#define PLL0_OUTPUT_FREQ 320000000UL
#define PLL1_OUTPUT_FREQ 160000000UL
#define PLL2_OUTPUT_FREQ 45158400UL

void sys_clock_init(void)
{
    sysctl_clock_set_clock_select(SYSCTL_CLOCK_SELECT_ACLK, SYSCTL_SOURCE_IN0);

    sysctl_pll_enable(SYSCTL_PLL0);
    sysctl_pll_set_freq(SYSCTL_PLL0, SYSCTL_SOURCE_IN0, PLL0_OUTPUT_FREQ);
    while (sysctl_pll_is_lock(SYSCTL_PLL0) == 0)
        sysctl_pll_clear_slip(SYSCTL_PLL0);
    sysctl_clock_enable(SYSCTL_CLOCK_PLL0);
    sysctl->clk_sel0.aclk_divider_sel = 0;
    sysctl_clock_set_clock_select(SYSCTL_CLOCK_SELECT_ACLK, SYSCTL_SOURCE_PLL0);

    sysctl_pll_enable(SYSCTL_PLL1);
    sysctl_pll_set_freq(SYSCTL_PLL1, SYSCTL_SOURCE_IN0, PLL1_OUTPUT_FREQ);
    while (sysctl_pll_is_lock(SYSCTL_PLL1) == 0)
        sysctl_pll_clear_slip(SYSCTL_PLL1);
    sysctl_clock_enable(SYSCTL_CLOCK_PLL1);

    sysctl_pll_enable(SYSCTL_PLL2);
    sysctl_pll_set_freq(SYSCTL_PLL2, SYSCTL_SOURCE_IN0, PLL2_OUTPUT_FREQ);
    while (sysctl_pll_is_lock(SYSCTL_PLL2) == 0)
        sysctl_pll_clear_slip(SYSCTL_PLL2);
    sysctl_clock_enable(SYSCTL_CLOCK_PLL2);
}
