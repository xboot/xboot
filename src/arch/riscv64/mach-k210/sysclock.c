#include "sysclock.h"
#include "stdio.h"
#include "sysctl.h"
#include "uarths.h"

void sys_clock_init()
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

uint32_t system_set_cpu_frequency(uint32_t frequency)
{
    sysctl_clock_set_clock_select(SYSCTL_CLOCK_SELECT_ACLK, SYSCTL_SOURCE_IN0);
    sysctl->pll0.pll_reset0 = 1;

    uint32_t result = sysctl_pll_set_freq(SYSCTL_PLL0, SYSCTL_SOURCE_IN0, frequency * 2);
    sysctl->pll0.pll_reset0 = 0;
    while (1)
    {
        uint32_t lock = sysctl->pll_lock.pll_lock0 & 0x3;
        if (lock == 0x3)
        {
            break;
        }
        else
        {
            sysctl->pll_lock.pll_slip_clear0 = 1;
        }
    }

    sysctl->pll0.pll_out_en0 = 1;
    sysctl_clock_set_clock_select(SYSCTL_CLOCK_SELECT_ACLK, SYSCTL_SOURCE_PLL0);
    uart_init();
    return result;
}
