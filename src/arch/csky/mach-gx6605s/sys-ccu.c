/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <gx6605s.h>
#include <gx6605s-gctl.h>

static uint32_t __startup pll_get_factors(uint32_t freq, uint32_t *n, uint32_t *m)
{
    uint32_t div, pll;

    if (freq > 1152 * MHZ)
        div = 2;
    else if (freq > 864 * MHZ)
        div = 3;
    else
        div = 4;

    pll = freq / (osc_freq / div);

    *n = div;
    *m = pll;

    return osc_freq / div * pll;
}

static void __startup dto_freq_set(unsigned int channel, uint32_t fin, uint32_t freq)
{
    uint32_t div = (freq / 1000) / (fin / 1000000) * 1073741;
    virtual_addr_t reg = GCTL_BASE + GX6605S_DTO0_CONFIG + (4 * (channel - 1));

    /**
     * div = freq * 2^30 / fin
     * freq = fin *  div / 2^30
     */

    write32(reg, (1 << 31));
    write32(reg, (0 << 31));
    write32(reg, (1 << 31));
    write32(reg, (1 << 31) | div);
    write32(reg, (1 << 31) | (1 << 30)| div);
    write32(reg, (1 << 31) | (1 << 30)| div);
    write32(reg, (1 << 31) | div);
}

/**
 * sys_ccu_cpu - Set the freq of cpu
 * @freq: freq want to set
 * @return: set success freq
 */
void __startup sys_ccu_cpu(uint32_t dto, uint32_t freq)
{
    return dto_freq_set(12, dto, freq);
}

/**
 * sys_ccu_axi - Set the freq of axi
 * @freq: freq want to set
 * @return: set success freq
 */
void __startup sys_ccu_axi(uint32_t cpu, uint32_t freq)
{
    uint32_t val, div = cpu / freq;

    val = read32(GCTL_BASE + GX6605S_CLOCK_DIV_CONFIG1);
    val &= ~(0x0f << 0);
    val |= (div & 0x0f) << 0;
    write32(GCTL_BASE + GX6605S_CLOCK_DIV_CONFIG1, val);
}

/**
 * sys_ccu_ahb - Set the freq of the ahb
 * @freq: freq want to set
 * @return: set success freq
 * TODO: computational problems
 */
void __startup sys_ccu_ahb(uint32_t cpu, uint32_t freq)
{
    uint32_t val;

    /* 0x07 is five div */
    val = read32(GCTL_BASE + GX6605S_SOURCE_SEL1);
    val &= ~(0x0f << 28);
    val |= (7 & 0x0f) << 28;
    write32(GCTL_BASE + GX6605S_SOURCE_SEL1, val);

    /* Selcect the CPU clock to from xtal to PLL */
    val = read32(GCTL_BASE + GX6605S_SOURCE_SEL0);
    val |= GX6605S_SOURCE_SEL0_CPU;
    write32(GCTL_BASE + GX6605S_SOURCE_SEL0, val);
}

/**
 * sys_ccu_apb - Set the freq of the apb
 * @freq: freq want to set
 * @return: set success freq
 */
void __startup sys_ccu_apb(uint32_t dto, uint32_t freq)
{
    uint32_t val;

    dto_freq_set(10, dto, freq); /* APB ir and other*/
    val = read32(GCTL_BASE + GX6605S_SOURCE_SEL0);
    val |= GX6605S_SOURCE_SEL0_APB;
    write32(GCTL_BASE + GX6605S_SOURCE_SEL0, val);

    dto_freq_set(11, dto, freq); /* APB uart */
    val = read32(GCTL_BASE + GX6605S_SOURCE_SEL0);
    val |= GX6605S_SOURCE_SEL0_UART;
    write32(GCTL_BASE + GX6605S_SOURCE_SEL0, val);
}

/**
 * sys_ccu_dram - Set the freq of the dram
 * @freq: freq want to set
 * @return: set success freq
 */
void __startup sys_ccu_dram(uint32_t freq)
{
    uint32_t val, n, m;

    pll_get_factors(freq, &n, &m);
    write32(GCTL_BASE + GX6605S_PLL_DDR_BASE, (1 << 14)|(n << 8)| m);
    val = read32(GCTL_BASE + GX6605S_PLL_DDR_BASE);
    val &= ~BIT(14);
    write32(GCTL_BASE + GX6605S_PLL_DDR_BASE, val);

    /* this is for dramc, phy and port clock */
    val = read32(GCTL_BASE + GX6605S_SOURCE_SEL0);
    val |= GX6605S_SOURCE_SEL0_DRAMC;
    write32(GCTL_BASE + GX6605S_SOURCE_SEL0, val);
}

void __startup sys_ccu_init(void)
{
    uint32_t val, n, m;

    write32(GCTL_BASE + GX6605S_SOURCE_SEL0, 0);

    pll_get_factors(dto_freq, &n, &m);
    write32(GCTL_BASE + GX6605S_PLL_DTO_BASE, BIT(14) | (n << 8) | m);
    val = read32(GCTL_BASE + GX6605S_PLL_DTO_BASE);
    val &= ~BIT(14);
    write32(GCTL_BASE + GX6605S_PLL_DTO_BASE, val);

    pll_get_factors(dvb_freq, &n, &m);
    write32(GCTL_BASE + GX6605S_PLL_DVB_BASE, BIT(14) | (n << 8) | m);
    val = read32(GCTL_BASE + GX6605S_PLL_DVB_BASE);
    val &= ~BIT(14);
    write32(GCTL_BASE + GX6605S_PLL_DVB_BASE, val);
}
