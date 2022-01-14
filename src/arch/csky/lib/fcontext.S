/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <entry.h>

ENTRY(jump_fcontext)
    subi    sp, 0x20
    stm     r8-r15, (sp)

    subi    sp, 0x08
    subi    sp, 0x20
    mfcr    r1, psr
    stw     r1, (sp, 0x00)

    /* switch stack */
    mov     a2, sp
    mov     sp, a0

    /* restore registers */
    ldw     r1, (sp, 0x00)
    mtcr    r1, epsr
    addi    sp, 0x08
    addi    sp, 0x20

    ldm     r8-r15, (sp)
    addi    sp, 0x20

    mtcr    lr, epc

    /* return transfer_t from jump */
    stw     a2, (gb, 0x00)
    stw     a1, (gb, 0x04)

    /*
     * pass transfer_t as first arg in context function
     * a0 == FCTX, a1 == DATA
     */
    mov     a0, a2

    rte
END(jump_fcontext)
