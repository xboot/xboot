/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <gx6605s-gctl.h>

#define DDR_TYPE        2
#define DDR_DRIVER      4
#define DDR_PULL        8
#define DDR_VREF_EXTER  0
#define DDR_VREF_INTE   1

struct dramc_fixup {
    unsigned char offset;
    unsigned char shift;
    uint16_t off, set;
};

#if CONFIG_PRELOAD_DRAM_CLK == 400

static const __startup_data uint32_t dramc_ctl_config[155] = {
    0x00000400, 0x00000000, 0x00013880, 0x00000050, 0x000000c8,
    0x02040a02, 0x12170402, 0x02030503, 0x006d6005, 0x00000303,
    0x06050101, 0x0000c80b, 0x00a01203, 0x00000005, 0x002a0100,
    0x00000c30, 0x000a0002, 0x00080002, 0x002e00c8, 0x00010000,
    0x00030300, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x000a5300, 0x00000002, 0x00000000,
    0x00040a53, 0x00000000, 0x00000000, 0x00010100, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01000200, 0x02000040,
    0x00010040, 0xff0a0103, 0x010101ff, 0x01010101, 0x010c0100,
    0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01010000,
    0x00000202, 0x02020302, 0x02000101, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00002819, 0x00000000, 0x00000800,
    0x00000008, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00004444, 0x00000000, 0x00000000, 0x00030300,
    0x00000000, 0x00000000, 0x00000000, 0x00000303, 0x00000000,
    0x00000000, 0x00030300, 0x0f0f0000, 0x0f0f0f0f, 0x0f0f0f0f,
    0xffff0f0f, 0x00000808, 0x0808ffff, 0x08ffff00, 0xffff0008,
    0x00000808, 0x0808ffff, 0x08ffff00, 0xffff0008, 0x00000808,
    0x0808ffff, 0x08ffff00, 0xffff0008, 0x00000808, 0x0808ffff,
    0x08ffff00, 0xffff0008, 0x00000808, 0x0808ffff, 0x08ffff00,
    0xffff0008, 0x00000808, 0x00010c0f, 0x0c00010c, 0x010c0001,
    0x00010c00, 0x0c00010c, 0x010c0001, 0x00010c00, 0x0c00010c,
    0x010c0001, 0x00010c00, 0x0c00010c, 0x010c0001, 0x00000000,
    0x000503e8, 0x00000700, 0x000c2d00, 0x02000200, 0x02000200,
    0x00000c2d, 0x00003ce1, 0x01020405, 0x03800004, 0x00040703,
    0x0000000a, 0x00000000, 0x00000000, 0x0010ffff, 0x11070303,
    0x0000000f, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000204, 0x00000000, 0x00000000, 0x00000001,
};

static const __startup_data uint32_t dramc_phy_config[26] = {
    0x26272627, 0x263a263a, 0x012100a0, 0x00000048, 0x43034c03,
    0x00000000, 0x26272627, 0x263a263a, 0x012100a0, 0x00000048,
    0x6b036b03, 0x00000000, 0x26272627, 0x263a263a, 0x012100a0,
    0x0000006d, 0x6b036b03, 0x00000000, 0x26272627, 0x263a263a,
    0x012100a0, 0x0000006d, 0x6b036b03, 0x00000000, 0x00004005,
    0x00000000,
};

#elif CONFIG_PRELOAD_DRAM_CLK == 533

static const __startup_data uint32_t dramc_ctl_config[155] = {
    0x00000400, 0x00000000, 0x00019f8f, 0x0000006b, 0x0000010a,
    0x02060e02, 0x181f0602, 0x02040704, 0x0091d607, 0x00000303,
    0x08070101, 0x0000c80f, 0x00d51803, 0x00000007, 0x00380100,
    0x00001040, 0x000d0003, 0x000a0003, 0x003e00c8, 0x00010000,
    0x00030300, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x000e7300, 0x00000042, 0x00000000,
    0x00040e73, 0x00000000, 0x00000000, 0x00010100, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01000200, 0x02000040,
    0x00010040, 0xff0a0103, 0x010101ff, 0x01000101, 0x010c0300,
    0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x01010000,
    0x00000202, 0x02020302, 0x02000101, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00002819, 0x00000000, 0x00000800,
    0x00000008, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00004444, 0x00000000, 0x00000000, 0x00030300,
    0x00000000, 0x00000000, 0x00000000, 0x00000303, 0x00000000,
    0x00000000, 0x00030300, 0x0f0f0000, 0x0f0f0f0f, 0x0f0f0f0f,
    0xffff0f0f, 0x00000808, 0x0808ffff, 0x08ffff00, 0xffff0008,
    0x00000808, 0x0808ffff, 0x08ffff00, 0xffff0008, 0x00000808,
    0x0808ffff, 0x08ffff00, 0xffff0008, 0x00000808, 0x0808ffff,
    0x08ffff00, 0xffff0008, 0x00000808, 0x0808ffff, 0x08ffff00,
    0xffff0008, 0x00000808, 0x00010c0f, 0x0c00010c, 0x010c0001,
    0x00010c00, 0x0c00010c, 0x010c0001, 0x00010c00, 0x0c00010c,
    0x010c0001, 0x00010c00, 0x0c00010c, 0x010c0001, 0x00000000,
    0x000503e8, 0x00000700, 0x00103300, 0x02000200, 0x02000200,
    0x00001033, 0x000050ff, 0x01020607, 0x03800004, 0x00040703,
    0x0000000a, 0x00000000, 0x00000000, 0x0010ffff, 0x13070303,
    0x0000000f, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000204, 0x00000000, 0x00000000, 0x00000001,
};

static const __startup_data uint32_t dramc_phy_config[26] = {
    0x26272627, 0x263a263a, 0x012100a0, 0x00000048, 0x43034803,
    0x00000000, 0x26272627, 0x263a263a, 0x012100a0, 0x00000048,
    0x6b036b03, 0x00000000, 0x26272627, 0x263a263a, 0x012100a0,
    0x0000006d, 0x6b036b03, 0x00000000, 0x26272627, 0x263a263a,
    0x012100a0, 0x0000006d, 0x6b036b03, 0x00000000, 0x00004005,
    0x00010101,
};

#else
# error "unknow dram freq"
#endif

static const __startup_data struct dramc_fixup dramc_fixup_table[] = {
    {  74,   0,  0x0001,  0x0001},
    {  77,  24,  0x0001,  0x0001},
    { 149,   8,  0x0001,  0x0001},

    {  95,  16,  0xffff,  0xffff},
    {  97,   0,  0xffff,  0xffff},
    {  98,   8,  0xffff,  0xffff},
    {  99,  16,  0xffff,  0xffff},
    { 101,   0,  0xffff,  0xffff},
    { 102,   8,  0xffff,  0xffff},
    { 103,  16,  0xffff,  0xffff},
    { 105,   0,  0xffff,  0xffff},
    { 106,   8,  0xffff,  0xffff},
    { 107,  16,  0xffff,  0xffff},
    { 109,   0,  0xffff,  0xffff},
    { 110,   8,  0xffff,  0xffff},
    { 111,  16,  0xffff,  0xffff},
    { 113,   0,  0xffff,  0xffff},
    { 114,   8,  0xffff,  0xffff},
    { 115,  16,  0xffff,  0xffff},

    {  96,   0,  0x000f,  0x0005},
    {  97,  16,  0x000f,  0x0006},
    {  98,  24,  0x000f,  0x0009},
    { 100,   0,  0x000f,  0x0008},
    { 101,  16,  0x000f,  0x0008},
    { 102,  24,  0x000f,  0x0004},
    { 104,   0,  0x000f,  0x0003},
    { 105,  16,  0x000f,  0x0008},
    { 106,  24,  0x000f,  0x0003},
    { 108,   0,  0x000f,  0x0005},
    { 109,  16,  0x000f,  0x0003},
    { 110,  24,  0x000f,  0x0008},
    { 112,   0,  0x000f,  0x0008},
    { 113,  16,  0x000f,  0x0008},
    { 114,  24,  0x000f,  0x0008},
    { 116,   0,  0x000f,  0x0008},

    {  96,   8,  0x000f,  0x0005},
    {  97,  24,  0x000f,  0x0006},
    {  99,   0,  0x000f,  0x0009},
    { 100,   8,  0x000f,  0x0008},
    { 101,  24,  0x000f,  0x0008},
    { 103,   0,  0x000f,  0x0004},
    { 104,   8,  0x000f,  0x0003},
    { 105,  24,  0x000f,  0x0008},
    { 107,   0,  0x000f,  0x0003},
    { 108,   8,  0x000f,  0x0005},
    { 109,  24,  0x000f,  0x0003},
    { 111,   0,  0x000f,  0x0008},
    { 112,   8,  0x000f,  0x0008},
    { 113,  24,  0x000f,  0x0008},
    { 115,   0,  0x000f,  0x0008},
    { 116,   8,  0x000f,  0x0008},

    {  43,  16,  0x0001,  0x0000},
    {   0,   0,  0x0001,  0x0001},
};

static void __startup dramc_fill_config(void)
{
    unsigned int count;
    uint32_t val;

    for (count = 0; count < ARRAY_SIZE(dramc_ctl_config); count++) {
        val = dramc_ctl_config[count];
        write32(DRAMC_BASE + (count << 2), val);
    }

    for (count = 0; count < ARRAY_SIZE(dramc_phy_config); count++) {
        val = dramc_phy_config[count];
        write32(DRAMC_BASE + 0x400 + (count << 2), val);
    }
}

static void __startup dramc_fixup_config(void)
{
    const struct dramc_fixup *table;
    unsigned int count;
    uint32_t val;

    for (count = 0; count < ARRAY_SIZE(dramc_fixup_table); count++) {
        table = &dramc_fixup_table[count];
        val = read32(DRAMC_BASE + (table->offset << 2));
        val &= ~(table->off << table->shift);
        val |= table->set << table->shift;
        write32(DRAMC_BASE + (table->offset << 2), val);
    }
}

static int __startup dramc_wait_ready(void)
{
    unsigned int timeout = 1000;
    uint32_t val;

    while (--timeout) {
        val = read32(DRAMC_BASE + 0xb8);
        if (val & 0x20)
            break;
        tim_mdelay(10);
    }

    return !!timeout;
}

void __startup sys_dramc_init(void)
{
    *(volatile uint32_t *)(GCTL_BASE + GX6605S_MPEG_CLK_INHIBIT_DAT) &= ~((1 << 27)|(1 << 28));
    *(volatile uint32_t *)(GCTL_BASE + GX6605S_DENALI_CONFIG0) = 0xffffffff;
    *(volatile uint32_t *)(GCTL_BASE + GX6605S_MPEG_CLD_RST1_DAT) |= (0x1);
    *(volatile uint32_t *)(GCTL_BASE + GX6605S_MPEG_CLD_RST1_DAT) &= ~(0x1);

    write32(GCTL_BASE + GX6605S_DRAM_CTRL0, 0x00);
    write32(GCTL_BASE + GX6605S_DRAM_CTRL1, 0x00);
    write32(GCTL_BASE + GX6605S_DRAM_CTRL2, 0x00);

#if DDR_TYPE == 3
    *(volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL1) &= ~(0x03 << 29);
    *(volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL1) |= 0x02 << 29;
#elif DDR_TYPE == 2
    *(volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL1) &= ~(0x03 << 29);
#else
# error "unknow dram type"
#endif

    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL0)) |= 0x01 << 0;          // fo 110/2
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL0)) &= ~(0x0f << 11);      // POWER DOWN
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL0)) &= ~(0x01 << 15);      // for pad_oe_e.low power

    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL0)) |= (DDR_PULL << 16)|(DDR_PULL << 20);
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL1)) |= (DDR_PULL <<  3)|(DDR_PULL <<  7)|(DDR_PULL << 14)|(DDR_PULL << 18);
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL2)) |= (DDR_PULL <<  3)|(DDR_PULL <<  7);

    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL0)) |= (DDR_DRIVER << 3)|(DDR_DRIVER << 6)|(DDR_DRIVER << 24);
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL1)) |= (DDR_DRIVER << 0)|(DDR_DRIVER << 11);
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL2)) |= (DDR_DRIVER << 0)|(DDR_DRIVER << 11);

    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL0)) |= (0x1 << 31);

#if DDR_VREF_EXTER
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL2)) &= ~(0xf << 16);
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL2)) |=  (0xf << 16);
#endif

#if DDR_VREF_INTE
    *((volatile uint32_t *)(GCTL_BASE + GX6605S_DRAM_CTRL2)) &= ~(0xf << 16);
#endif

    dramc_fill_config();
    dramc_fixup_config();
    dramc_wait_ready();
}
