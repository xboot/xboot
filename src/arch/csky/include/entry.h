/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __CSKY_ENTRY_H__
#define __CSKY_ENTRY_H__

#define usr_sp  ss0
#define exc_sp  ss1
#define tmp0    ss2
#define tmp1    ss3

.macro SAVE_STACK
    subi    sp, 32
    stm     r8-r15, (sp)
.endm

.macro REC_STACK
    ldm     r8-r15, (sp)
    addi    sp, 32
.endm

.macro SWITCH_TO_INT
    mtcr    sp, usr_sp
    mfcr    sp, exc_sp
.endm

.macro SWITCH_TO_USR
    mtcr    sp, exc_sp
    mfcr    sp, usr_sp
.endm

.macro SAVE_REGS
    mtcr    r1, tmp1

    mfcr    r1, psr
    lsli    r1, 9
    lsri    r1, 25

    cmpnei  r1, 10
    bf      1f
    cmpnei  r1, 11
    bf      1f

    SWITCH_TO_INT
1:
    mtcr    sp, tmp0
    subi    sp, 0x20
    subi    sp, 0x20
    subi    sp, 0x08

    mfcr    r1, epsr
    stw     r1, (sp, 0x00)

    mfcr    r1, epc
    stw     r1, (sp, 0x04)

    mfcr    r1, tmp0
    bf      2f
    mfcr	r1, usr_sp
2:
    stw     r1, (sp, 0x08)

    addi    sp, 0x0c

    mfcr    r1, tmp1
    stm     r1-r15, (sp)

    subi    sp, 0x0c
.endm

.macro REC_REGS
    ldw     r1, (sp, 0x00)
    mtcr    r1, epsr

    mfcr    r1, psr
    lsli    r1, 9
    lsri    r1, 25

    cmpnei  r1, 10
    bf      1f
    cmpnei  r1, 11
1:

    ldw     r1, (sp, 0x04)
    mtcr    r1, epc

    bf      2f
    ldw     r1, (sp, 0x08)
    mtcr    r1, usr_sp
2:

    addi    sp, 0x0c
    ldm     r1-r15, (sp)

    addi    sp, 0x1c
    addi    sp, 0x20

    bf      3f
    SWITCH_TO_USR
3:
.endm

#endif  /* __CSKY_ENTRY_H__ */
