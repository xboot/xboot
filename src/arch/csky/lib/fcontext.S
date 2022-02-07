/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <fcontext.h>

ENTRY(make_fcontext)
    subi    a0, (FCONTEXT_MAX * 4)
    stw     a2, (a0, FCONTEXT_LR * 4)
    rts
END(make_fcontext)

ENTRY(jump_fcontext)
    /* save registers */
    subi    sp, (FCONTEXT_MAX * 4)
    stm     r8-r15, (sp)

    /* switch stack */
    mov     a2, sp
    mov     sp, a0

    /* restore registers */
    ldm     r8-r15, (sp)
    addi    sp, (FCONTEXT_MAX * 4)

    /*
     * pass transfer_t as first arg in context function
     * a0 == FCTX, a1 == PRIV (not changed)
     */
    mov     a0, a2

    rts
END(jump_fcontext)
