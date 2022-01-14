/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_INTC_H__
#define __GX6605S_INTC_H__

enum gx6605s_intc_registers {
    GX6605S_INTC_STAT0              = 0x00,
    GX6605S_INTC_STAT1              = 0x04,
    GX6605S_INTC_PENDSTAT0          = 0x10,
    GX6605S_INTC_PENDSTAT1          = 0x14,
    GX6605S_INTC_GATESET0           = 0x20,
    GX6605S_INTC_GATESET1           = 0x24,
    GX6605S_INTC_GATECLR0           = 0x30,
    GX6605S_INTC_GATECLR1           = 0x34,
    GX6605S_INTC_GATE0              = 0x40,
    GX6605S_INTC_GATE1              = 0x44,
    GX6605S_INTC_MASK0              = 0x50,
    GX6605S_INTC_MASK1              = 0x54,

    GX6605S_INTC_SOURCE0            = 0x60,
    GX6605S_INTC_SOURCE1            = 0x64,
    GX6605S_INTC_SOURCE2            = 0x68,
    GX6605S_INTC_SOURCE3            = 0x6c,
    GX6605S_INTC_SOURCE4            = 0x70,
    GX6605S_INTC_SOURCE5            = 0x74,
    GX6605S_INTC_SOURCE6            = 0x78,
    GX6605S_INTC_SOURCE7            = 0x7c,
    GX6605S_INTC_SOURCE8            = 0x80,
    GX6605S_INTC_SOURCE9            = 0x84,
    GX6605S_INTC_SOURCE10           = 0x88,
    GX6605S_INTC_SOURCE11           = 0x8c,
    GX6605S_INTC_SOURCE12           = 0x90,
    GX6605S_INTC_SOURCE13           = 0x94,
    GX6605S_INTC_SOURCE14           = 0x98,
    GX6605S_INTC_SOURCE15           = 0x9c,

    GX6605S_FIQC_STAT0              = 0x100,
    GX6605S_FIQC_STAT1              = 0x104,
    GX6605S_FIQC_PENDSTAT0          = 0x110,
    GX6605S_FIQC_PENDSTAT1          = 0x114,
    GX6605S_FIQC_GATESET0           = 0x120,
    GX6605S_FIQC_GATESET1           = 0x124,
    GX6605S_FIQC_GATECLR0           = 0x130,
    GX6605S_FIQC_GATECLR1           = 0x134,
    GX6605S_FIQC_GATE0              = 0x140,
    GX6605S_FIQC_GATE1              = 0x144,
    GX6605S_FIQC_MASK0              = 0x150,
    GX6605S_FIQC_MASK1              = 0x154,

    GX6605S_FIQC_SOURCE0            = 0x160,
    GX6605S_FIQC_SOURCE1            = 0x164,
    GX6605S_FIQC_SOURCE2            = 0x168,
    GX6605S_FIQC_SOURCE3            = 0x16c,
    GX6605S_FIQC_SOURCE4            = 0x170,
    GX6605S_FIQC_SOURCE5            = 0x174,
    GX6605S_FIQC_SOURCE6            = 0x178,
    GX6605S_FIQC_SOURCE7            = 0x17c,
    GX6605S_FIQC_SOURCE8            = 0x180,
    GX6605S_FIQC_SOURCE9            = 0x184,
    GX6605S_FIQC_SOURCE10           = 0x188,
    GX6605S_FIQC_SOURCE11           = 0x18c,
    GX6605S_FIQC_SOURCE12           = 0x190,
    GX6605S_FIQC_SOURCE13           = 0x194,
    GX6605S_FIQC_SOURCE14           = 0x198,
    GX6605S_FIQC_SOURCE15           = 0x19c,
};

#define GX6605S_INTC_SOURCE_NR      64
#define GX6605S_FIQC_SOURCE_NR      64

#endif  /* __GX6605S_INTC_H__ */
