/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>

ENTRY(setjmp)
    stw     sp, (a0, 0x00)
    stw     l0, (a0, 0x04)
    stw     l1, (a0, 0x08)
    stw     l2, (a0, 0x0c)
    stw     l3, (a0, 0x10)
    stw     l4, (a0, 0x14)
    stw     l5, (a0, 0x18)
    stw     gb, (a0, 0x1c)
    stw     lr, (a0, 0x20)

    rts
END(setjmp)

ENTRY(longjmp)
    ldw     sp, (a0, 0x00)
    ldw     l0, (a0, 0x04)
    ldw     l1, (a0, 0x08)
    ldw     l2, (a0, 0x0c)
    ldw     l3, (a0, 0x10)
    ldw     l4, (a0, 0x14)
    ldw     l5, (a0, 0x18)
    ldw     gb, (a0, 0x1c)
    ldw     lr, (a0, 0x20)

    mov     a0, a1
    cmpnei  a1, 0
    bt      1f
    movi    a0, 1

1:
    rts
END(longjmp)
