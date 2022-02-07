/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <setjmp.h>

ENTRY(setjmp)
    stw     sp, (a0, JMP_BUF_SP * 4)
    stw     l0, (a0, JMP_BUF_L0 * 4)
    stw     l1, (a0, JMP_BUF_L1 * 4)
    stw     l2, (a0, JMP_BUF_L2 * 4)
    stw     l3, (a0, JMP_BUF_L3 * 4)
    stw     l4, (a0, JMP_BUF_L4 * 4)
    stw     l5, (a0, JMP_BUF_L5 * 4)
    stw     gb, (a0, JMP_BUF_GB * 4)
    stw     lr, (a0, JMP_BUF_LR * 4)

    xor     a0, a0
    rts
END(setjmp)

ENTRY(longjmp)
    ldw     sp, (a0, JMP_BUF_SP * 4)
    ldw     l0, (a0, JMP_BUF_L0 * 4)
    ldw     l1, (a0, JMP_BUF_L1 * 4)
    ldw     l2, (a0, JMP_BUF_L2 * 4)
    ldw     l3, (a0, JMP_BUF_L3 * 4)
    ldw     l4, (a0, JMP_BUF_L4 * 4)
    ldw     l5, (a0, JMP_BUF_L5 * 4)
    ldw     gb, (a0, JMP_BUF_GB * 4)
    ldw     lr, (a0, JMP_BUF_LR * 4)

    mov     a0, a1
    cmpnei  a1, 0
    bt      1f
    movi    a0, 1

1:
    rts
END(longjmp)
