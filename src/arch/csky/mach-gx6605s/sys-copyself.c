/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <gx6605s.h>

static const char __startup_data preload_entry[] = "\nxboot\n";
static const char __startup_data preload_done[] = "booting...\n";

void __startup sys_copyself(void)
{
    sys_ccu_init();
    sys_ccu_cpu(dto_freq, cpu_freq);
    sys_ccu_axi(cpu_freq, axi_freq);
    sys_ccu_ahb(cpu_freq, ahb_freq);
    sys_ccu_apb(dto_freq, apb_freq);
    sys_ccu_dram(dram_freq);

    tim_init(apb_freq);
    sys_uart_init(apb_freq, uart_freq);

    sys_dramc_init();
    sys_spinor_init();
    sys_uart_print(preload_entry);

    sys_spinor_read((void *)DRAM_BASE, IMAGE_POS, IMAGE_SIZE);
    sys_uart_print(preload_done);
}
