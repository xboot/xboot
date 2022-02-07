/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include "gx6605s-video.h"

#define GX6605S_SVPU_SURFACE_WIDTH      (720)
#define GX6605S_SVPU_SURFACE_HEIGHT     (576)
#define GX6605S_SVPU_SURFACE_BPP        (24)
#define GX6605S_SVPU_SURFACE_NUM        (3)

static uint32_t gx6605s_sphasenomal[64] = {
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

static uint32_t gx6605s_osd_filter_table[64] = {
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

static uint32_t gx6605s_svpu_table[4][15] = {
    {
           0,    0,  719,    0,  575,
        9830, 4096,  720,  480,    0,
        2867,    0,  719,    0,  477,
    },{
           0,    0,  719,    0,  479,
        6836, 4096,  720,  576,    0,
        1365,    0,  719,    0,  573,
    },{
           0,    0,  719,    0,  575,
        4915, 4096,  720,  480,    0,
         409,    0,  719,    0,  477,
    },{
           0,    0,  719,    0,  479,
        3413, 4096,  720,  576,  341,
           0,    0,  719,    0,  573,
    },
};

static uint32_t gx6605s_vout_table[GX6605S_HDMI_NR_MAX] = {
    [GX6605S_HDMI_480I] =
        ((4  <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((0  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_480P] =
        ((8  <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((0  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_576I] =
        ((0  <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((0  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_576P] =
        ((9  <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((0  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_720P_50HZ] =
        ((13 <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((1  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_720P_60HZ] =
        ((13 <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((0  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_1080I_50HZ] =
        ((6  <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((1  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_1080I_60HZ] =
        ((6  <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((0  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_1080P_50HZ] =
        ((14 <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((1  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
    [GX6605S_HDMI_1080P_60HZ] =
        ((14 <<  0) & GX6605S_VOUT_TV_FORMAT) |
        ((0  <<  5) & GX6605S_VOUT_FRAME_FRE) |
        ((3  << 17) & GX6605S_VOUT_DAC_OUT_MODE),
};

static uint32_t gx6605s_svout_table[GX6605S_CVBS_NR_MAX] = {
    [GX6605S_CVBS_PAL] =
        ((0 << 0) & GX6605S_VOUT_TV_FORMAT) |
        ((0 << 5) & GX6605S_VOUT_FRAME_FRE) |
        BIT(31),
    [GX6605S_CVBS_PAL_M] =
        ((1 << 0) & GX6605S_VOUT_TV_FORMAT) |
        ((0 << 5) & GX6605S_VOUT_FRAME_FRE) |
        BIT(31),
    [GX6605S_CVBS_PAL_N] =
        ((2 << 0) & GX6605S_VOUT_TV_FORMAT) |
        ((0 << 5) & GX6605S_VOUT_FRAME_FRE) |
        BIT(31),
    [GX6605S_CVBS_PAL_NC] =
        ((3 << 0) & GX6605S_VOUT_TV_FORMAT) |
        ((0 << 5) & GX6605S_VOUT_FRAME_FRE) |
        BIT(31),
    [GX6605S_CVBS_NTSC_M] =
        ((4 << 0) & GX6605S_VOUT_TV_FORMAT) |
        ((0 << 5) & GX6605S_VOUT_FRAME_FRE) |
        BIT(31),
    [GX6605S_CVBS_NTSC_443] =
        ((5 << 0) & GX6605S_VOUT_TV_FORMAT) |
        ((0 << 5) & GX6605S_VOUT_FRAME_FRE) |
        BIT(31),
};

static uint32_t gx6605s_hdmi_ceacode[GX6605S_HDMI_NR_MAX] = {
    [GX6605S_HDMI_480I]         =  2,
    [GX6605S_HDMI_480P]         =  6,
    [GX6605S_HDMI_576I]         = 21,
    [GX6605S_HDMI_576P]         = 17,
    [GX6605S_HDMI_720P_50HZ]    = 19,
    [GX6605S_HDMI_720P_60HZ]    =  4,
    [GX6605S_HDMI_1080I_50HZ]   = 20,
    [GX6605S_HDMI_1080I_60HZ]   =  5,
    [GX6605S_HDMI_1080P_50HZ]   = 31,
    [GX6605S_HDMI_1080P_60HZ]   = 16,
};

static inline void gx6605s_videoout_clockdiv(struct gx6605s_device *gdev, unsigned int clock, unsigned int div)
{
    gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, BIT(clock), div << clock);
}

static inline void gx6605s_videoout_clocksel(struct gx6605s_device *gdev, unsigned int clock, unsigned int source)
{
    gx6605s_config_mask(gdev, GX6605S_SOURCE_SEL0, BIT(clock), source << clock);
}

static inline void gx6605s_vpu_buffer_delay(struct gx6605s_device *gdev, unsigned int delay)
{
    gx6605s_mask(gdev, GX6605S_VPU_BUFF_CTRL2, GX6605S_VPU_BUFF_CTRL2_STATE_DELAY, delay << 16);
}

static void gx6605s_hdmi_setup(struct gx6605s_device *gdev)
{
    gx6605s_config_mask(gdev, GX6605S_MPEG_CLD_RST0_DAT, 0, BIT(7));
    mdelay(2);
    gx6605s_config_mask(gdev, GX6605S_MPEG_CLD_RST0_DAT, BIT(7), 0);
    mdelay(2);
}

static void gx6605s_videoout_clock(struct gx6605s_device *gdev, bool_t sub_id)
{
    if (!sub_id) {
        switch (gdev->hdmi) {
            case GX6605S_HDMI_480I:
            case GX6605S_HDMI_576I:
                gx6605s_vpu_buffer_delay(gdev, 0xc0);

                gx6605s_videoout_clocksel(gdev, 5, 0);  /* sel pll video 1.188GHz */
                gx6605s_videoout_clockdiv(gdev, 7, 0);  /* load_en -> 0 */
                gx6605s_videoout_clockdiv(gdev, 6, 1);  /* bypass -> 0 */

                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0x3f, 0);
                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0, 43);
                gx6605s_videoout_clockdiv(gdev, 7, 1);  /* load_en -> 1 */
                gx6605s_videoout_clockdiv(gdev, 7, 0);  /* load_en -> 0 */

                gx6605s_videoout_clocksel(gdev, 5, 0);  /* sel pll video 148.5M */
                gx6605s_videoout_clockdiv(gdev, 8, 0);  /* div hdmi */
                gx6605s_videoout_clockdiv(gdev, 9, 0);  /* div dcs */
                gx6605s_videoout_clockdiv(gdev, 10, 1); /* div pixel */
                gx6605s_videoout_clockdiv(gdev, 11, 0); /* 148.5M div rsts */
                gx6605s_videoout_clockdiv(gdev, 11, 1); /* 148.5M div rst */

                gx6605s_config_mask(gdev, GX6605S_SOURCE_SEL1, 0, 3 << 25);
                break;

            case GX6605S_HDMI_480P:
            case GX6605S_HDMI_576P:
                gx6605s_vpu_buffer_delay(gdev, 0x60);

                gx6605s_videoout_clocksel(gdev, 5, 0);  /* sel pll video 1.188GHz */
                gx6605s_videoout_clockdiv(gdev, 7, 0);  /* load_en -> 0 */
                gx6605s_videoout_clockdiv(gdev, 6, 1);  /* bypass -> 0 */

                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0x3f, 0);
                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0, 43);
                gx6605s_videoout_clockdiv(gdev, 7,1);   /* load_en -> 1 */
                gx6605s_videoout_clockdiv(gdev, 7,0);   /* load_en -> 0 */

                gx6605s_videoout_clocksel(gdev, 5, 1);  /* sel pll video 148.5M */
                gx6605s_videoout_clockdiv(gdev, 8, 0);  /* div hdmi */
                gx6605s_videoout_clockdiv(gdev, 9, 0);  /* div dcs */
                gx6605s_videoout_clockdiv(gdev, 10, 0); /* div pixel */
                gx6605s_videoout_clockdiv(gdev, 11, 0); /* 148.5M div rst */
                gx6605s_videoout_clockdiv(gdev, 11, 1); /* 148.5M div rst */

                gx6605s_config_mask(gdev, GX6605S_SOURCE_SEL1, 0, 3 << 25);
                break;

            case GX6605S_HDMI_720P_50HZ:
            case GX6605S_HDMI_720P_60HZ:
            case GX6605S_HDMI_1080I_50HZ:
            case GX6605S_HDMI_1080I_60HZ:
                gx6605s_vpu_buffer_delay(gdev, 0x30);

                gx6605s_videoout_clocksel(gdev, 5, 0);  /* sel pll video 1.188GHz */
                gx6605s_videoout_clockdiv(gdev, 7, 0);  /* load_en -> 0 */
                gx6605s_videoout_clockdiv(gdev, 6, 1);  /* bypass -> 0 */

                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0x3f, 0);
                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0, 15);
                gx6605s_videoout_clockdiv(gdev, 7, 1);  /* load_en -> 1 */
                gx6605s_videoout_clockdiv(gdev, 7, 0);  /* load_en -> 0 */

                gx6605s_videoout_clocksel(gdev, 5, 1);  /* sel pll video 148.5M */
                gx6605s_videoout_clockdiv(gdev, 8, 0);  /* div hdmi */
                gx6605s_videoout_clockdiv(gdev, 9, 0);  /* div dcs */
                gx6605s_videoout_clockdiv(gdev, 10, 0); /* div pixel */
                gx6605s_videoout_clockdiv(gdev, 11, 0); /* 148.5M div rst */
                gx6605s_videoout_clockdiv(gdev, 11, 1); /* 148.5M div rst */

                gx6605s_config_mask(gdev, GX6605S_SOURCE_SEL1, 0, 3 << 25);
                break;

            case GX6605S_HDMI_1080P_50HZ:
            case GX6605S_HDMI_1080P_60HZ:
                gx6605s_vpu_buffer_delay(gdev, 0x18);

                gx6605s_videoout_clocksel(gdev, 5, 0);  /* sel pll video 1.188GHz */
                gx6605s_videoout_clockdiv(gdev, 7, 0);  /* load_en -> 0 */
                gx6605s_videoout_clockdiv(gdev, 6, 1);  /* bypass -> 0 */

                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0x3f, 0);
                gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0, 7);
                gx6605s_videoout_clockdiv(gdev, 7, 1);  /* load_en -> 1 */
                gx6605s_videoout_clockdiv(gdev, 7, 0);  /* load_en -> 0 */

                gx6605s_videoout_clocksel(gdev, 5, 1);  /* sel pll video 148.5M */
                gx6605s_videoout_clockdiv(gdev, 8, 0);  /* div hdmi */
                gx6605s_videoout_clockdiv(gdev, 9, 0);  /* div dcs */
                gx6605s_videoout_clockdiv(gdev, 10, 0); /* div pixel */
                gx6605s_videoout_clockdiv(gdev, 11, 0); /* 148.5M div rsts */
                gx6605s_videoout_clockdiv(gdev, 11, 1); /* 148.5M div rst */

                gx6605s_config_mask(gdev, GX6605S_SOURCE_SEL1, 0, 3 << 25);
                break;

            default:
                break;
        }
    }

    else {
        gx6605s_videoout_clockdiv(gdev, 20, 0); /* rst -> 0 */
        gx6605s_videoout_clockdiv(gdev, 19, 0); /* load_en -> 0 */
        gx6605s_videoout_clockdiv(gdev, 20, 1); /* rst -> 1 */
        gx6605s_videoout_clockdiv(gdev, 18, 0); /* bypass -> 0 */
        gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0x3f000, 0);
        gx6605s_config_mask(gdev, GX6605S_CLOCK_DIV_CONFIG0, 0, 43 << 12);
        gx6605s_videoout_clockdiv(gdev, 19, 1); /* load_en -> 1 */
        gx6605s_videoout_clockdiv(gdev, 19, 0); /* load_en -> 0 */
        gx6605s_videoout_clocksel(gdev, 6, 0);  /* 1: pll 0: xtal */
    }
}

static inline void gx6605s_svpu_start(struct gx6605s_device *gdev)
{
    gx6605s_mask(gdev, GX6605S_SVPU_CAP_CTRL, 0, GX6605S_SVPU_CAP_CTRL_EN);
    gx6605s_mask(gdev, GX6605S_SVPU_DISP_CTRL, 0, GX6605S_SVPU_DISP_CTRL_EN);
    gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, GX6605S_SVPU_PARA_UPDATE_EN);
    gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, 0);
}

static inline void gx6605s_svpu_stop(struct gx6605s_device *gdev)
{
    gx6605s_mask(gdev, GX6605S_SVPU_CAP_CTRL, GX6605S_SVPU_CAP_CTRL_EN, 0);
    gx6605s_mask(gdev, GX6605S_SVPU_DISP_CTRL, GX6605S_SVPU_DISP_CTRL_EN, 0);
    gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, GX6605S_SVPU_PARA_UPDATE_EN);
    gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, 0);
}

static int gx6605s_svpu_capmode(struct gx6605s_device *gdev)
{
    int capmode;

    switch (gdev->hdmi) {
        case GX6605S_HDMI_480I:
            if (gdev->cvbs < GX6605S_CVBS_NTSC_M)
                return -1;
            capmode = 9;
            break;

        case GX6605S_HDMI_480P:
            if (gdev->cvbs < GX6605S_CVBS_NTSC_M)
                return -1;
            capmode = 7;
            break;

        case GX6605S_HDMI_576I:
            if (gdev->cvbs >= GX6605S_CVBS_NTSC_M)
                return -1;
            capmode = 8;
            break;

        case GX6605S_HDMI_576P:
            if (gdev->cvbs >= GX6605S_CVBS_NTSC_M)
                return -1;
            capmode = 6;
            break;

        case GX6605S_HDMI_720P_50HZ:
        case GX6605S_HDMI_720P_60HZ:
            if (gdev->cvbs < GX6605S_CVBS_NTSC_M)
                capmode = 5;
            else
                capmode = 4;
            break;

        case GX6605S_HDMI_1080I_50HZ:
        case GX6605S_HDMI_1080I_60HZ:
            if (gdev->cvbs < GX6605S_CVBS_NTSC_M)
                capmode = 3;
            else
                capmode = 2;
            break;

        case GX6605S_HDMI_1080P_50HZ:
        case GX6605S_HDMI_1080P_60HZ:
            if (gdev->cvbs < GX6605S_CVBS_NTSC_M)
                capmode = 1;
            else
                capmode = 0;
            break;

        default:
            return -1;
    }

    return capmode;
}

static int gx6605s_svpu_capindex(struct gx6605s_device *gdev)
{
    int capindex;

    switch (gdev->hdmi) {
        case GX6605S_HDMI_480I:
            if (gdev->cvbs >= GX6605S_CVBS_NTSC_M)
                return -1;
            capindex = 3;
            break;

        case GX6605S_HDMI_480P:
            if (gdev->cvbs >= GX6605S_CVBS_NTSC_M)
                return -1;
            capindex = 2;
            break;

        case GX6605S_HDMI_576I:
            if (gdev->cvbs < GX6605S_CVBS_NTSC_M)
                return -1;
            capindex = 1;
            break;

        case GX6605S_HDMI_576P:
            if (gdev->cvbs < GX6605S_CVBS_NTSC_M)
                return -1;
            capindex = 0;
            break;

        default:
            return -1;
    }

    return capindex;
}

static int gx6605s_svpu_reqblock(int width, int bpp)
{
    int line_byte = ((width * bpp) >> 3);
    int ret = 32, request = (line_byte / 4) / 128 * 128;

    if (request < 32)
        ret = 32;

    else {
        while (request < 896 && (line_byte % request && line_byte % request < 32))
            request += 8;
        if (request >= 896 || (line_byte%request && line_byte%request<32))
            ret = 32;
        else
            ret = request;
    }

    return ret;
}

static int gx6605s_svpu_config(struct gx6605s_device *gdev)
{
    unsigned int count, buff_block = GX6605S_SVPU_SURFACE_WIDTH * GX6605S_SVPU_SURFACE_HEIGHT;
    physical_addr_t phys_addr = virt_to_phys(gdev->svpu_buff);
    int capmode, reqblock;

    /* setup svpu buffers */
    for (count = 0; count < GX6605S_SVPU_SURFACE_NUM; ++count) {
        gx6605s_writel(gdev, (GX6605S_SVPU_BUF0 + (4 * count)) + GX6605S_BUFF_Y_TOPFIELD_ADDR,      phys_addr + (buff_block * 0));
        gx6605s_writel(gdev, (GX6605S_SVPU_BUF0 + (4 * count)) + GX6605S_BUFF_Y_BOTTOMFIELD_ADDR,   phys_addr + (buff_block * 0 + GX6605S_SVPU_SURFACE_WIDTH));
        gx6605s_writel(gdev, (GX6605S_SVPU_BUF0 + (4 * count)) + GX6605S_BUFF_U_TOPFIELD_ADDR,      phys_addr + (buff_block * 1));
        gx6605s_writel(gdev, (GX6605S_SVPU_BUF0 + (4 * count)) + GX6605S_BUFF_U_BOTTOMFIELD_ADDR,   phys_addr + (buff_block * 1 + GX6605S_SVPU_SURFACE_WIDTH));
    }

    /* setup svpu cap mode */
    capmode = gx6605s_svpu_capmode(gdev);
    if (capmode < 0) {
        int capindex;

        capindex = gx6605s_svpu_capindex(gdev);
        if (capindex < 0)
            return -EINVAL;

        gx6605s_mask(gdev, GX6605S_SVPU_CAP_CTRL, GX6605S_SVPU_CAP_CTRL_MODE_EN, 0);

        gx6605s_mask(gdev, GX6605S_SVPU_CAP_V,      GX6605S_SVPU_CAP_CTRL_VDOWNSCALE_EN,    gx6605s_svpu_table[capindex][ 0] <<  4);
        gx6605s_mask(gdev, GX6605S_SVPU_CAP_V,      GX6605S_SVPU_CAP_V_LEFT,                gx6605s_svpu_table[capindex][ 1] <<  0);
        gx6605s_mask(gdev, GX6605S_SVPU_CAP_V,      GX6605S_SVPU_CAP_V_RIGHT,               gx6605s_svpu_table[capindex][ 2] << 16);
        gx6605s_mask(gdev, GX6605S_SVPU_CAP_H,      GX6605S_SVPU_CAP_H_TOP,                 gx6605s_svpu_table[capindex][ 3] <<  0);
        gx6605s_mask(gdev, GX6605S_SVPU_CAP_H,      GX6605S_SVPU_CAP_H_BOTTOM,              gx6605s_svpu_table[capindex][ 4] << 16);
        gx6605s_mask(gdev, GX6605S_SVPU_SCE_ZOOM,   GX6605S_SVPU_SCE_ZOOM_V,                gx6605s_svpu_table[capindex][ 5] <<  0);
        gx6605s_mask(gdev, GX6605S_SVPU_SCE_ZOOM,   GX6605S_SVPU_SCE_ZOOM_H,                gx6605s_svpu_table[capindex][ 6] << 16);
        gx6605s_mask(gdev, GX6605S_SVPU_SCE_ZOOM1,  GX6605S_SVPU_SCE_ZOOM1_LENGTH,          gx6605s_svpu_table[capindex][ 7] <<  0);
        gx6605s_mask(gdev, GX6605S_SVPU_SCE_ZOOM1,  GX6605S_SVPU_SCE_ZOOM1_HEIGHT,          gx6605s_svpu_table[capindex][ 8] << 16);
        gx6605s_mask(gdev, GX6605S_SVPU_ZOOM_PHASE, GX6605S_SVPU_ZOOM_PHASE_VT,             gx6605s_svpu_table[capindex][ 9] <<  0);
        gx6605s_mask(gdev, GX6605S_SVPU_ZOOM_PHASE, GX6605S_SVPU_ZOOM_PHASE_VB,             gx6605s_svpu_table[capindex][10] << 16);
        gx6605s_mask(gdev, GX6605S_SVPU_VIEW_V,     GX6605S_SVPU_VIEW_V_RIGHT,              gx6605s_svpu_table[capindex][11] <<  0);
        gx6605s_mask(gdev, GX6605S_SVPU_VIEW_V,     GX6605S_SVPU_VIEW_V_LEFT,               gx6605s_svpu_table[capindex][12] << 16);
        gx6605s_mask(gdev, GX6605S_SVPU_VIEW_H,     GX6605S_SVPU_VIEW_H_BOTTOM,             gx6605s_svpu_table[capindex][13] <<  0);
        gx6605s_mask(gdev, GX6605S_SVPU_VIEW_H,     GX6605S_SVPU_VIEW_H_TOP,                gx6605s_svpu_table[capindex][14] << 16);
    } else {
        gx6605s_mask(gdev, GX6605S_SVPU_CAP_CTRL, GX6605S_SVPU_CAP_CTRL_MODE, capmode << 24);
        gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, GX6605S_SVPU_PARA_UPDATE_EN);
        gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, 0);
        gx6605s_mask(gdev, GX6605S_SVPU_CAP_CTRL, 0, GX6605S_SVPU_CAP_CTRL_MODE_EN);
        gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, GX6605S_SVPU_PARA_UPDATE_EN);
        gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, 0);
    }

    /* setup svpu cap level */
    gx6605s_mask(gdev, GX6605S_SVPU_CAP_CTRL, GX6605S_SVPU_CAP_CTRL_CAP_LEVEL, 0);

    /* setup svpu reqblock */
    reqblock = gx6605s_svpu_reqblock(GX6605S_SVPU_SURFACE_WIDTH, GX6605S_SVPU_SURFACE_BPP);
    gx6605s_mask(gdev, GX6605S_SVPU_REQ_LENGTH, GX6605S_SVPU_REQ_LENGTH_WRITE, reqblock << 16);
    gx6605s_mask(gdev, GX6605S_SVPU_DISP_REQ_BLOCK, GX6605S_SVPU_DISP_REQ_BLOCK_READ, reqblock << 16);
    gx6605s_mask(gdev, GX6605S_SVPU_VPU_FRAME_MODE, 0, GX6605S_SVPU_VPU_FRAME_MODE_23);

    /* update para final */
    gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, GX6605S_SVPU_PARA_UPDATE_EN);
    gx6605s_writel(gdev, GX6605S_SVPU_PARA_UPDATE, 0);

    return -ENOERR;
}

int gx6605s_vpu_hwinit(struct gx6605s_device *gdev)
{
    if ((
         gdev->hdmi == GX6605S_HDMI_720P_50HZ  ||
         gdev->hdmi == GX6605S_HDMI_1080I_50HZ ||
         gdev->hdmi == GX6605S_HDMI_1080P_50HZ
        ) && (
         gdev->cvbs == GX6605S_CVBS_NTSC_M ||
         gdev->cvbs == GX6605S_CVBS_NTSC_443
    )) {
        /* can't support 50HZ with NTSC */
        return -EINVAL;
    }

    if ((
         gdev->hdmi == GX6605S_HDMI_720P_60HZ  ||
         gdev->hdmi == GX6605S_HDMI_1080I_60HZ ||
         gdev->hdmi == GX6605S_HDMI_1080P_60HZ
        ) && (
         gdev->cvbs == GX6605S_CVBS_PAL    ||
         gdev->cvbs == GX6605S_CVBS_PAL_M  ||
         gdev->cvbs == GX6605S_CVBS_PAL_N  ||
         gdev->cvbs == GX6605S_CVBS_PAL_NC
    )) {
        /* can't support 60HZ with PAL */
        return -EINVAL;
    }

    /* config hdmi cvbs mode */
    if (gdev->hdmi != GX6605S_HDMI_DISABLE)
        gx6605s_writel(gdev, GX6605S_VOUT_OFFSET, gx6605s_vout_table[gdev->hdmi]);
    if (gdev->cvbs != GX6605S_CVBS_DISABLE)
        gx6605s_writel(gdev, GX6605S_SVOUT_OFFSET, gx6605s_svout_table[gdev->cvbs]);

    /* vpu hdmi clock config */
    gx6605s_videoout_clock(gdev, FALSE);

    if (gdev->hdmi != GX6605S_HDMI_DISABLE) {
        gx6605s_hdmi_setup(gdev);
        dw_hdmi_configure(&gdev->video, gx6605s_hdmi_ceacode[gdev->hdmi]);
    }

    /* video dac config */
    if (gdev->hdmi != GX6605S_HDMI_DISABLE)
        gx6605s_writel(gdev, GX6605S_VPU_OFFSET + 0x134, 0x1f);
    else
        gx6605s_writel(gdev, GX6605S_VPU_OFFSET + 0x134, 0x18);

    gx6605s_writel(gdev, GX6605S_VPU_OFFSET + 0x138, 0x01);

    /* svpu cvbs setup */
    if (gdev->cvbs != GX6605S_CVBS_DISABLE) {
        gx6605s_writel(gdev, GX6605S_SVPU_DISP_REQ_BLOCK, 0xffff0000);
        gx6605s_mask(gdev, GX6605S_SVPU_CAP_CTRL, 0, 3 << 8);
        gx6605s_svpu_stop(gdev);
        gx6605s_svpu_config(gdev);
    }

    return -ENOERR;
}

void gx6605s_vpu_zoom(struct gx6605s_device *gdev, unsigned int layer,
                      unsigned int source_width, unsigned int source_height,
                      unsigned int dest_width, unsigned int dest_height,
                      unsigned int start_x, unsigned int start_y)
{
    unsigned int hzoom = 4096, vzoom = 4096;
    unsigned int hdownscale = 0, vdownscale = 0;
    unsigned int start_x_here = 0, start_y_here = 0;
    unsigned int vtbais = 0, vbbais = 0;
    unsigned int vpu_mode = 1;
    unsigned int count;

    source_height = (source_height / 2) * 2;
    source_width = (source_width / 8) * 8;
    dest_height = (dest_height / 2) * 2;
    dest_width = (dest_width / 8) * 8;

    switch (gdev->hdmi) {
        case GX6605S_HDMI_480I:
        case GX6605S_HDMI_480P:
            if ((dest_height + start_y) > 480)
                return;
            if ((dest_width + start_x) > 720)
                return;
            break;

        case GX6605S_HDMI_576I:
        case GX6605S_HDMI_576P:
            if ((dest_height + start_y) > 576)
                return;
            if ((dest_width + start_x) > 720)
                return;
            break;

        case GX6605S_HDMI_720P_50HZ:
        case GX6605S_HDMI_720P_60HZ:
            if ((dest_height + start_y) > 720)
                return;
            if ((dest_width + start_x) > 1280)
                return;
            break;

        case GX6605S_HDMI_1080I_50HZ:
        case GX6605S_HDMI_1080I_60HZ:
        case GX6605S_HDMI_1080P_50HZ:
        case GX6605S_HDMI_1080P_60HZ:
            if ((dest_height + start_y) > 1080)
                return;
            if ((dest_width + start_x) > 1920)
                return;
            break;

        default:
            return;
    }

    if (source_width != dest_width)
        hzoom = (source_width - 1) * 4096 / (dest_width - 1 ) + 1;

    if (hzoom > 4096 * 4) {
        hdownscale = 1;
        hzoom = ((source_width/2) - 1) * 4096 / (dest_width - 1 ) + 1;
        if (hzoom > 4096 * 4)
            return;
    }

    if ((dest_width-1) > ((source_width / (1 + hdownscale) - 1) * 4096 / hzoom))
        start_x_here = ((dest_width - 1) - ((source_width / (1 + hdownscale) - 1) * 4096 / hzoom)) /2;

    start_y_here = 0;

    switch (gdev->hdmi) {
        case GX6605S_HDMI_480I:
        case GX6605S_HDMI_576I:
        case GX6605S_HDMI_1080I_50HZ:
        case GX6605S_HDMI_1080I_60HZ:
            if (source_height != dest_height)
                vzoom = (source_height - 1) * 4096 / (dest_height - 1 ) + 1;

            if (vzoom >= 4096) {
                vpu_mode = 0;
                if (vzoom > 4096 * 4) {
                    vdownscale = 1;
                    vzoom = ((source_height / 2) - 1) * 4096 / (dest_height - 1 ) + 1;
                    if (vzoom > 4096 * 4)
                        return;
                }
            } else {
                vtbais = 0;
                vbbais = vzoom / 2;
            }

            if ((dest_height - 1) > (source_height * (1 + vpu_mode)/(1 + vdownscale) - 1) * 4096/(vzoom * (1 + vpu_mode)) )
                start_y_here = ((dest_height - 1) - (source_height * (1 + vpu_mode)/(1 + vdownscale) - 1) * 4096/(vzoom * (1 + vpu_mode))) /2;

            break;

        case GX6605S_HDMI_480P:
        case GX6605S_HDMI_576P:
        case GX6605S_HDMI_720P_50HZ:
        case GX6605S_HDMI_720P_60HZ:
        case GX6605S_HDMI_1080P_50HZ:
        case GX6605S_HDMI_1080P_60HZ:
            vzoom = (source_height - 1) * 4096 / (dest_height - 1 ) + 1;
            if (vzoom >= 4096) {
                if(vzoom > 4096 * 4) {
                    vdownscale = 1;
                    vzoom = ((source_height / 2) - 1) * 4096 / (dest_height - 1) + 1;
                    if(vzoom > 4096 * 4)
                        return;
                }
            }

            vtbais = 0;
            vbbais = 0;

            if ((dest_height-1) > (source_height * (1 + vpu_mode) / (1 + vdownscale) - 1) * 4096 / (vzoom * (1 + vpu_mode) / 2))
                start_y_here = 0;

            break;

        default:
            return;
    }

    if (layer == 0) { /* spp */
        if (vdownscale)
            gx6605s_mask(gdev, GX6605S_VPU_PIC_CTRL, 0, BIT(2)); /* JPG_CTRL */
        else
            gx6605s_mask(gdev, GX6605S_VPU_PIC_CTRL, BIT(2), 0);

        if (hdownscale)
            gx6605s_mask(gdev, GX6605S_VPU_PIC_CTRL, 0, BIT(3));
        else
            gx6605s_mask(gdev, GX6605S_VPU_PIC_CTRL, BIT(3), 0);

        gx6605s_writel(gdev, GX6605S_VPU_PIC_V_PHASE, vtbais | vbbais << 16);                                       /* JPG_V_PHASE */
        gx6605s_writel(gdev, GX6605S_VPU_PIC_POSITION, (start_x + start_x_here) | (start_y + start_y_here) << 16);  /* JPG_POSTITION */
        gx6605s_writel(gdev, GX6605S_VPU_PIC_SOURCE_SIZE, source_width | source_height << 16);                      /* JPG_SOURCE_SIZE */
        gx6605s_writel(gdev, GX6605S_VPU_PIC_VIEW_SIZE, dest_width | dest_height << 16);                            /* JPG_VIEW_SIZE */
        gx6605s_writel(gdev, GX6605S_VPU_PIC_ZOOM, hzoom | vzoom << 16);                                            /* JPG_ZOOM */

        gx6605s_mask(gdev, GX6605S_VPU_PIC_PARA, 0x7ff << 16, 0);                                                   /* JPG_PARA */
        gx6605s_mask(gdev, GX6605S_VPU_PIC_PARA, 0, (((source_width >> 2) >> 3) << 3) << 16);
        gx6605s_mask(gdev, GX6605S_VPU_PIC_PARA, BIT(29), 0);
        gx6605s_mask(gdev, GX6605S_VPU_PIC_PARA, 0, vpu_mode << 29);

        for (count = 0; count < ARRAY_SIZE(gx6605s_sphasenomal); ++count)
            gx6605s_writel(gdev, GX6605S_PIC_PARA_H + count * 4, gx6605s_sphasenomal[count]);                       /* JPG_PARA_H_0~63 */

        for (count = 0; count < ARRAY_SIZE(gx6605s_sphasenomal); ++count)
            gx6605s_writel(gdev, GX6605S_PIC_PARA_V + count * 4, gx6605s_sphasenomal[count]);                       /* JPG_PARA_V_0~63 */
    }

    else { /* osd */
        gx6605s_writel(gdev, GX6605S_VPU_OSD_POSITION, (start_x + start_x_here) | (start_y + start_y_here) << 16);
        gx6605s_writel(gdev, GX6605S_VPU_OSD_VIEW_SIZE, dest_width | dest_height << 16);
        gx6605s_writel(gdev, GX6605S_VPU_OSD_ZOOM, hzoom | vzoom << 16);

        for (count = 0; count < ARRAY_SIZE(gx6605s_osd_filter_table); ++count)
            gx6605s_writel(gdev, GX6605S_VPU_OSD_PARA + count * 4, gx6605s_osd_filter_table[count]);  /* JPG_PARA_V_0~63 */
    }
}
