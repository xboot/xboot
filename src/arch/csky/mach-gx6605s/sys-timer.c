/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <gx6605s-tim.h>

void __startup tim_mdelay(uint32_t ms)
{
    uint32_t oldt = read32(TIM_BASE + GX6605S_TIM_VALUE);
    while (read32(TIM_BASE + GX6605S_TIM_VALUE) < (oldt + ms));
}

void __startup tim_init(uint32_t freq)
{
    write32(TIM_BASE + GX6605S_TIM_CLKDIV, 0);
    write32(TIM_BASE + GX6605S_TIM_RELOAD, 0);
    write32(TIM_BASE + GX6605S_TIM_VALUE, 0);
    write32(TIM_BASE + GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_RST);
    write32(TIM_BASE + GX6605S_TIM_CONFIG, GX6605S_TIM_CONFIG_EN);
    write32(TIM_BASE + GX6605S_TIM_CLKDIV, (freq / 1000) - 1);
    write32(TIM_BASE + GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_START);
}
