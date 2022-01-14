/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <gx6605s-vpu.h>

struct gx6605s_device {
    void *mmio, *region;
    int byteseq;
};

static uint32_t palette_buffer[2] = {
    0, 0xff00ff00
};

static uint32_t sPhaseNomal[] = {
    0x10909010, 0x00ff0100, 0x01ff0200, 0x02ff0300,
    0x03ff0400, 0x04ff0500, 0x05fe0700, 0x06fe0800,
    0x07fd0a00, 0x07fc0b00, 0x08fc0c00, 0x09fb0e00,
    0x09fa1001, 0x0af91201, 0x0bf81401, 0x0bf71501,
    0x0cf61701, 0x0cf51801, 0x0df41b02, 0x0df31c02,
    0x0ef11f02, 0x0ef02002, 0x0fef2303, 0x0fed2503,
    0x10ec2703, 0x10ea2903, 0x10e92b04, 0x10e72d04,
    0x11e53004, 0x11e33305, 0x11e13505, 0x11df3705,
    0x12de3a06, 0x12dc3c06, 0x12da3e06, 0x12d74106,
    0x12d54407, 0x12d34607, 0x12d14807, 0x12cf4b08,
    0x12cd4d08, 0x12ca5008, 0x12c85309, 0x12c65509,
    0x12c35809, 0x12c15b0a, 0x12bf5d0a, 0x12bc600a,
    0x12ba630b, 0x12b7660b, 0x12b5680b, 0x12b26b0b,
    0x12b06e0c, 0x12ad710c, 0x12aa750d, 0x11a8760d,
    0x11a5790d, 0x11a27d0e, 0x11a07f0e, 0x119d820e,
    0x109a840e, 0x1098870f, 0x10958a0f, 0x10928d0f,
};

static uint32_t gx3201_osd_filter_table[] = {
    0x00808000, 0x007e8200, 0x007c8400, 0x007a8600,
    0x00788800, 0x00768a00, 0x00748c00, 0x00728e00,
    0x00709000, 0x006e9200, 0x006c9400, 0x006a9600,
    0x00689800, 0x00669a00, 0x00649c00, 0x00629e00,
    0x0060a000, 0x005ea200, 0x005ca400, 0x005aa600,
    0x0058a800, 0x0056aa00, 0x0054ac00, 0x0052ae00,
    0x0050b000, 0x004eb200, 0x004cb400, 0x004ab600,
    0x0048b800, 0x0046ba00, 0x0044bc00, 0x0042be00,
    0x0040c000, 0x003ec200, 0x003cc400, 0x003ac600,
    0x0038c800, 0x0036ca00, 0x0034cc00, 0x0032ce00,
    0x0030d000, 0x002ed200, 0x002cd400, 0x002ad600,
    0x0028d800, 0x0026da00, 0x0024dc00, 0x0022de00,
    0x0020e000, 0x001ee200, 0x001ce400, 0x001ae600,
    0x0018e800, 0x0016ea00, 0x0014ec00, 0x0012ee00,
    0x0010f000, 0x000ef200, 0x000cf400, 0x000af600,
    0x0008f800, 0x0006fa00, 0x0004fc00, 0x0002fe00,
};

/********  CVBS PAL/NTSC  ********/
#ifdef ENABLE_PAL
static uint32_t gx3201_dve_pal_bdghi[] = {
    0x00010000, 0x00000000, 0x0097D47E, 0x001AE677,
    0x00781E13, 0x00F09618, 0x25D03940, 0x21C39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x69973F3F, 0x00013F37, 0x00000000, 0x00000098,
    0x00000000, 0x00000000, 0x000000D7, 0x00000000,
    0x00002C22, 0x008004B0, 0x0003F2E0, 0x06400000,
    0x00000018, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xAAAAAAAA, 0x000000A9,
    0x00000000, 0x0001FFFF, 0x10002000, 0x40826083,
    0xA082B082, 0xD082F000, 0x0026135F, 0x00002093,
    0x028506BF, 0x00001096, 0x0000002C, 0x00025625,
    0x00004AC4, 0x0000A935, 0x000A726E, 0x0009BA70,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08A8F20D,
    0x08A8F20D, 0x08A8F20D, 0x1705C170, 0x00000180
};
#endif

#ifdef ENABLE_PAL_M
static uint32_t gx3201_dve_pal_m[] = {
    0x00010001, 0x00000000, 0x009de17e, 0x001A2671,
    0x00781e13, 0x00b0a610, 0x25d05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6b953c47, 0x00004737, 0x00000000, 0x00000089,
    0x00000000, 0x00000000, 0x000000c2, 0x00000000,
    0x00002C22, 0x008004b0, 0x0003F2E0, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0026135F, 0x00002093,
    0xa000b083, 0x00001096, 0x0000002c, 0x00025625,
    0x00004AC4, 0x00007901, 0x0008c208, 0x0008320b,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000180
};
#endif

#ifdef ENABLE_PAL_N
static uint32_t gx3201_dve_pal_n[] = {
    0x00010002, 0x00000000, 0x0097d47e, 0x001AE677,
    0x00781e13, 0x00b0a610, 0x25d05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x69973C47, 0x00003F37, 0x00000000, 0x00000089,
    0x00000000, 0x00000000, 0x000000c2, 0x00000000,
    0x00002C22, 0x008004b0, 0x0003F2E0, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0026135F, 0x00002093,
    0x028506BF, 0x00001096, 0x0000002c, 0x00025625,
    0x00004AC4, 0x0000A935, 0x000A726E, 0x0009BA70,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

#ifdef ENABLE_PAL_NC
static uint32_t gx3201_dve_pal_nc[] = {
    0x00010003, 0x00000000, 0x0097da7e, 0x001AE677,
    0x00781e13, 0x00b0a610, 0x25d05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x69973F3F, 0x00003F37, 0x00000000, 0x00000089,
    0x00000000, 0x00000000, 0x000000c2, 0x00000000,
    0x00002C22, 0x008004b0, 0x0003F2E0, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0026135F, 0x00002093,
    0x028506BF, 0x00001096, 0x0000002c, 0x00025625,
    0x00004AC4, 0x0000A935, 0x000A726E, 0x0009BA70,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

#ifdef ENABLE_NTSC_M
static uint32_t gx3201_dve_ntsc_m[] = {
    0x00010004, 0x00000000, 0x0090d47e, 0x0017a65d,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x0004b800, 0x00000000,
    0x64803c47, 0x00003f37, 0x00000000, 0x0000008e,
    0x00000000, 0x00000000, 0x000000c6, 0x00000000,
    0x00002C22, 0x008004B0, 0x0003D2D8, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x001FA359, 0x00002093,
    0x028506B3, 0x00001096, 0x0000002c, 0x00025625,
    0x00004AC4, 0x00007901, 0x0008C208, 0x0008320B,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

#ifdef ENABLE_NTSC_443
static uint32_t gx3201_dve_ntsc_443[] = {
    0x00010005, 0x00000000, 0x0090c67e, 0x001A2671,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x0004b800, 0x00000000,
    0x64803c47, 0x00003f37, 0x00000000, 0x0000008e,
    0x00000000, 0x00000000, 0x000000c6, 0x00000000,
    0x00002C22, 0x008004B0, 0x0003D2D8, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x001FA359, 0x00002093,
    0x028506B3, 0x00001096, 0x0000002c, 0x00025625,
    0x00004AC4, 0x00007901, 0x0008C208, 0x0008320B,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x00000187
};
#endif

/********  YPbPr/HDMI  ********/
#ifdef ENABLE_YPBPR_HDMI_480I
static uint32_t gx3201_dve_ycbcr_480i[] = {
    0x14010004, 0x00000000, 0x0097D47E, 0x0015c64f,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00002F22, 0x008004B0, 0x0003D2D8, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x001FA359, 0x00002093,
    0x028506B3, 0x00001096, 0x0000002c, 0x00025625,
    0x00004AC4, 0x00007901, 0x0008C208, 0x0008320B,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_576I
static uint32_t gx3201_dve_ycbcr_576i[] = {
    0x14010000, 0x00000000, 0x0097D47E, 0x00182661,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00002F22, 0x008004B0, 0x0003F2E0, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0026135f, 0x00002093,
    0x028506BF, 0x00001096, 0x0000002c, 0x00025625,
    0x00004AC4, 0x0000a935, 0x000a726e, 0x0009ba70,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0xC00001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_480P
static uint32_t gx3201_dve_ypbpr_480p[] = {
    0x14010008, 0x00000000, 0x0097D43F, 0x00162651,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00003810, 0x008004B0, 0x0207e5b0, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x001fa359, 0x00002093,
    0x028506b3, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x00011A02, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_576P
static uint32_t gx3201_dve_ypbpr_576p[] = {
    0x14010009, 0x00000000, 0x0097D43F, 0x00188664,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x00003f37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x0000341b, 0x008004B0, 0x0207e5C0, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0026135f, 0x00002093,
    0x028506bf, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x00014267, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_50HZ
static uint32_t gx3201_dve_ypbpr_720p_50hz[] = {
    0x1405000D, 0x00000000, 0x0097D40A, 0x0042cc16,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00001d12, 0x00a584B0, 0x0207e671, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x002E87BB, 0x00002093,
    0x02850F77, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x0000C2E7, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_720P_60HZ
static uint32_t gx3201_dve_ypbpr_720p_60hz[] = {
    0x1405000D, 0x00000000, 0x0097D40A, 0x0042cc16,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00001d12, 0x00a584B0, 0x0207e62C, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x002E8671, 0x00002093,
    0x02850CE3, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x0000C2E7, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_50HZ
static uint32_t gx3201_dve_ypbpr_1080i_50hz[] = {
    0x14050006, 0x00000000, 0x0097D40A, 0x002e1070,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00003129, 0x00aC04B0, 0x0207e83E, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0045Aa4f, 0x00002093,
    0x058B149F, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x00009A2E, 0x00123461, 0x00119464,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080I_60HZ
static uint32_t gx3201_dve_ypbpr_1080i_60hz[] = {
    0x14050006, 0x00000000, 0x0097D40A, 0x002e1070,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00003129, 0x00aC04B0, 0x0207e83E, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0045A897, 0x00002093,
    0x058B112D, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x00009A2E, 0x00123461, 0x00119464,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_50HZ
static uint32_t gx3201_dve_ypbpr_1080p_50hz[] = {
    0x1405000E, 0x00000000, 0x0097D40A, 0x00190848,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00001612, 0x008B04B0, 0x0081F436, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0045F527, 0x00002093,
    0x02C58a4f, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x0001445F, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

#ifdef ENABLE_YPBPR_HDMI_1080P_60HZ
static uint32_t gx3201_dve_ypbpr_1080p_60hz[] = {
    0x1405000E, 0x00000000, 0x0097D40A, 0x00190848,
    0x00781E13, 0x00B0A610, 0x25D05940, 0x1EC39102,
    0x20885218, 0x2010F094, 0x00050000, 0x00000000,
    0x6B953F3F, 0x01803F37, 0x00000000, 0x0000009d,
    0x00000000, 0x00000000, 0x0000009d, 0x00000000,
    0x00001612, 0x008B04B0, 0x0081F436, 0x06400000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0xaaaaaaaa, 0x000000A9,
    0x00000000, 0x0001FFFE, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x0045F44B, 0x00002093,
    0x02C58897, 0x00001096, 0x04000024, 0x00025625,
    0x00004AC4, 0x0001445F, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x10002083, 0x40866082,
    0xA000B083, 0xD081F086, 0x10040100, 0x08a8f20d,
    0x08a8f20d, 0x08a8f20d, 0x1705c170, 0x000001c7
};
#endif

enum gx6605s_byte_order {
    GX6605S_DCBA_HGFE   = 0,
    GX6605S_EFGH_ABCD   = 1,
    GX6605S_HGFE_DCBA   = 2,
    GX6605S_ABCD_EFGH   = 3,
    GX6605S_CDAB_GHEF   = 4,
    GX6605S_FEHG_BADC   = 5,
    GX6605S_GHEF_CDAB   = 6,
    GX6605S_BADC_FEHG   = 7,
};

enum gx6605s_color_fmt {
    GX6605S_COLOR_CLUT1             = 0,
    GX6605S_COLOR_CLUT2             = 1,
    GX6605S_COLOR_CLUT4             = 2,
    GX6605S_COLOR_CLUT8             = 3,
    GX6605S_COLOR_RGBA4444          = 4,
    GX6605S_COLOR_RGBA5551          = 5,
    GX6605S_COLOR_RGB565            = 6,
    GX6605S_COLOR_RGBA8888          = 7,
    GX6605S_COLOR_RGB888            = 8,
    GX6605S_COLOR_BGR888            = 9,

    GX6605S_COLOR_ARGB4444          = 10,
    GX6605S_COLOR_ARGB1555          = 11,
    GX6605S_COLOR_ARGB8888          = 12,

    GX6605S_COLOR_YCBCR422          = 13,
    GX6605S_COLOR_YCBCRA6442        = 14,
    GX6605S_COLOR_YCBCR420          = 15,

    GX6605S_COLOR_YCBCR420_Y_UV     = 16,
    GX6605S_COLOR_YCBCR420_Y_U_V    = 17,
    GX6605S_COLOR_YCBCR420_Y        = 18,
    GX6605S_COLOR_YCBCR420_U        = 19,
    GX6605S_COLOR_YCBCR420_V        = 20,
    GX6605S_COLOR_YCBCR420_UV       = 21,

    GX6605S_COLOR_YCBCR422_Y_UV     = 22,
    GX6605S_COLOR_YCBCR422_Y_U_V    = 23,
    GX6605S_COLOR_YCBCR422_Y        = 24,
    GX6605S_COLOR_YCBCR422_U        = 25,
    GX6605S_COLOR_YCBCR422_V        = 26,
    GX6605S_COLOR_YCBCR422_UV       = 27,

    GX6605S_COLOR_YCBCR444          = 28,
    GX6605S_COLOR_YCBCR444_Y_UV     = 29,
    GX6605S_COLOR_YCBCR444_Y_U_V    = 30,
    GX6605S_COLOR_YCBCR444_Y        = 31,
    GX6605S_COLOR_YCBCR444_U        = 32,
    GX6605S_COLOR_YCBCR444_V        = 33,
    GX6605S_COLOR_YCBCR444_UV       = 34,

    GX6605S_COLOR_YCBCR400          = 35,
    GX6605S_COLOR_A8                = 36,
    GX6605S_COLOR_ABGR4444          = 37,
    GX6605S_COLOR_ABGR1555          = 39,
    GX6605S_COLOR_Y8                = 40,
    GX6605S_COLOR_UV16              = 41,
    GX6605S_COLOR_YCBCR422v         = 42,
    GX6605S_COLOR_YCBCR422h         = 43,
};
