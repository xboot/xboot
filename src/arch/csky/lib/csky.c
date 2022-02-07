/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <csky.h>

void proc_idle(void)
{
    asm volatile ("stop");
}

void proc_halt(void)
{
    local_irq_disable();
    for (;;)
        proc_idle();
}