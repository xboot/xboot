/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <csky.h>

struct transfer_t;

void *make_fcontext(void *stack, size_t size, void (*func)(struct transfer_t))
{
    struct regs *context = stack - sizeof(*context);

    memset(context, 0, sizeof(*context));

    context->psr = PSR_S | PSR_EE | PSR_IE | PSR_FE;
    context->lr = (uint32_t)func;

    return context;
}

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