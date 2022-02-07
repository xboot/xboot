/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_VPU_H__
#define __GX6605S_VPU_H__

#include <gx6605s.h>
#include <gx6605s-gctl.h>

#define GX6605S_JPEG_OFFSET     0x400000
#define GX6605S_VPU_OFFSET      0x800000
#define GX6605S_VOUT_OFFSET     0x804000
#define GX6605S_SVPU_OFFSET     0x900000
#define GX6605S_SVOUT_OFFSET    0x904000
#define GX6605S_SPDIF_OFFSET    0xc00000
#define GX6605S_HDMI_OFFSET     0xf00000

enum gx6605s_jpeg_registers {
    GX6605S_JPEG_DEC_CTRL                       = GX6605S_JPEG_OFFSET + 0x00,
    GX6605S_JPEG_BS_BUF_START_ADDR              = GX6605S_JPEG_OFFSET + 0x04,
    GX6605S_JPEG_BS_BUF_SIZE                    = GX6605S_JPEG_OFFSET + 0x08,
    GX6605S_JPEG_BS_BUFFER_ALMOST_EMPTY_TH      = GX6605S_JPEG_OFFSET + 0x0c,
    GX6605S_JPEG_BS_BUFFER_WR_PTR               = GX6605S_JPEG_OFFSET + 0x10,
    GX6605S_JPEG_BS_BUFFER_RD_PTR               = GX6605S_JPEG_OFFSET + 0x14,
    GX6605S_JPEG_FRAME_BUFFER_Y_BASE_ADDR       = GX6605S_JPEG_OFFSET + 0x18,
    GX6605S_JPEG_FRAME_BUFFER_CB_BASE_ADDR      = GX6605S_JPEG_OFFSET + 0x1c,
    GX6605S_JPEG_FRAME_BUFFER_CR_BASE_ADDR      = GX6605S_JPEG_OFFSET + 0x20,
    GX6605S_JPEG_FRAME_BUFFER_STRIDE            = GX6605S_JPEG_OFFSET + 0x24,
    GX6605S_JPEG_FRAME_BUFFER_MAX_PIC_SIZE      = GX6605S_JPEG_OFFSET + 0x28,
    GX6605S_JPEG_CLIP_UPPER_LEFT_COORDINATE     = GX6605S_JPEG_OFFSET + 0x2c,
    GX6605S_JPEG_CLIP_LOWER_RIGHT_COORDINATE    = GX6605S_JPEG_OFFSET + 0x30,
    GX6605S_JPEG_DECODE_DEAD_OVER_TIME_GATE     = GX6605S_JPEG_OFFSET + 0x34,
    GX6605S_JPEG_PIC_LINE_CNT_GATE              = GX6605S_JPEG_OFFSET + 0x38,
    GX6605S_JPEG_PIC_INFO                       = GX6605S_JPEG_OFFSET + 0x3c,
    GX6605S_JPEG_PIC_ORIGION_SIZE_INFO          = GX6605S_JPEG_OFFSET + 0x40,
    GX6605S_JPEG_PIC_WRITE_BACK_SIZE_INFO       = GX6605S_JPEG_OFFSET + 0x44,
    GX6605S_JPEG_PIC_DISPLAY_SIZE_INFO          = GX6605S_JPEG_OFFSET + 0x48,
    GX6605S_JPEG_PIC_DISPLAY_COORDINATE_INFO    = GX6605S_JPEG_OFFSET + 0x4c,
    GX6605S_JPEG_PIC_WB_LINE_INFO               = GX6605S_JPEG_OFFSET + 0x50,
    GX6605S_JPEG_INT_STATUS                     = GX6605S_JPEG_OFFSET + 0x54,
    GX6605S_JPEG_INT_ENABLE_CPU0                = GX6605S_JPEG_OFFSET + 0x58,
    GX6605S_JPEG_INT_ENABLE_CPU1                = GX6605S_JPEG_OFFSET + 0x5c,
    GX6605S_JPEG_INT_ENABLE_CPU2                = GX6605S_JPEG_OFFSET + 0x60,
    GX6605S_JPEG_INT_ENABLE_CPU3                = GX6605S_JPEG_OFFSET + 0x64,
};

enum gx6605s_vpu_registers {
    GX6605S_VPU_PP_CTRL                         = GX6605S_VPU_OFFSET + 0x00,
    GX6605S_VPU_PP_V_PHASE                      = GX6605S_VPU_OFFSET + 0x04,
    GX6605S_VPU_PP_POSITION                     = GX6605S_VPU_OFFSET + 0x08,
    GX6605S_VPU_PP_SOURCE_SIZE                  = GX6605S_VPU_OFFSET + 0x0c,
    GX6605S_VPU_PP_VIEW_SIZE                    = GX6605S_VPU_OFFSET + 0x10,
    GX6605S_VPU_PP_ZOOM                         = GX6605S_VPU_OFFSET + 0x14,
    GX6605S_VPU_PP_FRAME_STRIDE                 = GX6605S_VPU_OFFSET + 0x18,
    GX6605S_VPU_PP_FILTER_SIGN                  = GX6605S_VPU_OFFSET + 0x1c,
    GX6605S_VPU_PP_PHASE_0_H                    = GX6605S_VPU_OFFSET + 0x20,
    GX6605S_VPU_PP_PHASE_0_V                    = GX6605S_VPU_OFFSET + 0x24,
    GX6605S_VPU_PP_DISP_CTRL                    = GX6605S_VPU_OFFSET + 0x28,
    GX6605S_VPU_PP_DISP_R_PTR                   = GX6605S_VPU_OFFSET + 0x2c,
    GX6605S_VPU_PP_BACK_COLOR                   = GX6605S_VPU_OFFSET + 0x30,

    GX6605S_VPU_PIC_CTRL                        = GX6605S_VPU_OFFSET + 0x40,
    GX6605S_VPU_PIC_V_PHASE                     = GX6605S_VPU_OFFSET + 0x44,
    GX6605S_VPU_PIC_POSITION                    = GX6605S_VPU_OFFSET + 0x48,
    GX6605S_VPU_PIC_SOURCE_SIZE                 = GX6605S_VPU_OFFSET + 0x4c,
    GX6605S_VPU_PIC_VIEW_SIZE                   = GX6605S_VPU_OFFSET + 0x50,
    GX6605S_VPU_PIC_ZOOM                        = GX6605S_VPU_OFFSET + 0x54,
    GX6605S_VPU_PIC_PARA                        = GX6605S_VPU_OFFSET + 0x58,
    GX6605S_VPU_PIC_FILTER_SIGN                 = GX6605S_VPU_OFFSET + 0x5c,
    GX6605S_VPU_PIC_PHASE_0_H                   = GX6605S_VPU_OFFSET + 0x60,
    GX6605S_VPU_PIC_PHASE_0_V                   = GX6605S_VPU_OFFSET + 0x64,
    GX6605S_VPU_PIC_Y_TOP_ADDR                  = GX6605S_VPU_OFFSET + 0x68,
    GX6605S_VPU_PIC_Y_BOTTOM_ADDR               = GX6605S_VPU_OFFSET + 0x6c,
    GX6605S_VPU_PIC_UV_TOP_ADDR                 = GX6605S_VPU_OFFSET + 0x70,
    GX6605S_VPU_PIC_UV_BOTTOM_ADDR              = GX6605S_VPU_OFFSET + 0x74,
    GX6605S_VPU_PIC_BACK_COLOR                  = GX6605S_VPU_OFFSET + 0x78,

    GX6605S_VPU_OSD_CTRL                        = GX6605S_VPU_OFFSET + 0x90,
    GX6605S_VPU_OSD_FIRST_HEAD_PTR              = GX6605S_VPU_OFFSET + 0x94,
    GX6605S_VPU_OSD_VIEW_SIZE                   = GX6605S_VPU_OFFSET + 0x98,
    GX6605S_VPU_OSD_ZOOM                        = GX6605S_VPU_OFFSET + 0x9c,
    GX6605S_VPU_OSD_COLOR_KEY                   = GX6605S_VPU_OFFSET + 0xa0,
    GX6605S_VPU_OSD_ALPHA_5551                  = GX6605S_VPU_OFFSET + 0xa4,
    GX6605S_VPU_OSD_PHASE_0                     = GX6605S_VPU_OFFSET + 0xa8,
    GX6605S_VPU_OSD_POSITION                    = GX6605S_VPU_OFFSET + 0xac,

    GX6605S_VPU_CAP_CTRL                        = GX6605S_VPU_OFFSET + 0xb0,
    GX6605S_VPU_CAP_ADDR                        = GX6605S_VPU_OFFSET + 0xb4,
    GX6605S_VPU_CAP_HEIGHT                      = GX6605S_VPU_OFFSET + 0xb8,
    GX6605S_VPU_CAP_WIDTH                       = GX6605S_VPU_OFFSET + 0xbc,

    GX6605S_VPU_VBI_CTRL                        = GX6605S_VPU_OFFSET + 0xd0,
    GX6605S_VPU_VBI_FIRST_ADDR                  = GX6605S_VPU_OFFSET + 0xd4,
    GX6605S_VPU_VBI_ADDR                        = GX6605S_VPU_OFFSET + 0xd8,

    GX6605S_VPU_MIX_CTRL                        = GX6605S_VPU_OFFSET + 0xdc,
    GX6605S_VPU_CHIPTEST                        = GX6605S_VPU_OFFSET + 0xe0,
    GX6605S_VPU_SCAN_LINE                       = GX6605S_VPU_OFFSET + 0xe4,
    GX6605S_VPU_SYS_PARA                        = GX6605S_VPU_OFFSET + 0xe8,
    GX6605S_VPU_BUFF_CTRL1                      = GX6605S_VPU_OFFSET + 0xec,
    GX6605S_VPU_BUFF_CTRL2                      = GX6605S_VPU_OFFSET + 0xf0,
    GX6605S_VPU_EMPTY_GATE_1                    = GX6605S_VPU_OFFSET + 0xf4,
    GX6605S_VPU_EMPTY_GATE_2                    = GX6605S_VPU_OFFSET + 0xf8,
    GX6605S_VPU_FULL_GATE                       = GX6605S_VPU_OFFSET + 0xfc,
    GX6605S_VPU_BUFFER_INT                      = GX6605S_VPU_OFFSET + 0x100,

    GX6605S_VPU_PP_PARA_H                       = GX6605S_VPU_OFFSET + 0x0200,   /* 0x0200 ~ 0x02fc */
    GX6605S_VPU_PP_PARA_V                       = GX6605S_VPU_OFFSET + 0x0300,   /* 0x0300 ~ 0x03fc */

    GX6605S_VPU_OSD_PARA                        = GX6605S_VPU_OFFSET + 0x0400,   /* 0x0400 ~ 0x04fc */

    GX6605S_VPU_DISP0_CTRL                      = GX6605S_VPU_OFFSET + 0x0500,   /* 0x0500 ~ 0x051c */
    GX6605S_VPU_DISP1_CTRL                      = GX6605S_VPU_OFFSET + 0x0520,   /* 0x0520 ~ 0x053c */
    GX6605S_VPU_DISP2_CTRL                      = GX6605S_VPU_OFFSET + 0x0540,   /* 0x0540 ~ 0x055c */
    GX6605S_VPU_DISP3_CTRL                      = GX6605S_VPU_OFFSET + 0x0560,   /* 0x0560 ~ 0x057c */
    GX6605S_VPU_DISP4_CTRL                      = GX6605S_VPU_OFFSET + 0x0580,   /* 0x0580 ~ 0x059c */
    GX6605S_VPU_DISP5_CTRL                      = GX6605S_VPU_OFFSET + 0x05a0,   /* 0x05a0 ~ 0x05bc */
    GX6605S_VPU_DISP6_CTRL                      = GX6605S_VPU_OFFSET + 0x05c0,   /* 0x05c0 ~ 0x05dc */
    GX6605S_VPU_DISP7_CTRL                      = GX6605S_VPU_OFFSET + 0x05e0,   /* 0x05e0 ~ 0x05fc */

    GX6605S_PIC_PARA_H                          = GX6605S_VPU_OFFSET + 0x0600,   /* 0x0600 ~ 0x06fc */
    GX6605S_PIC_PARA_V                          = GX6605S_VPU_OFFSET + 0x0700,   /* 0x0700 ~ 0x07fc */
};

enum gx6605s_svpu_registers {
    GX6605S_SVPU_BUF0                           = GX6605S_SVPU_OFFSET + 0x0000,
    GX6605S_SVPU_BUF1                           = GX6605S_SVPU_OFFSET + 0x0018,
    GX6605S_SVPU_BUF2                           = GX6605S_SVPU_OFFSET + 0x0030,
    GX6605S_SVPU_CAP_CTRL                       = GX6605S_SVPU_OFFSET + 0x0050,
    GX6605S_SVPU_CAP_H                          = GX6605S_SVPU_OFFSET + 0x0058,
    GX6605S_SVPU_CAP_V                          = GX6605S_SVPU_OFFSET + 0x005c,
    GX6605S_SVPU_SCE_ZOOM                       = GX6605S_SVPU_OFFSET + 0x0060,
    GX6605S_SVPU_SCE_ZOOM1                      = GX6605S_SVPU_OFFSET + 0x0064,
    GX6605S_SVPU_REQ_LENGTH                     = GX6605S_SVPU_OFFSET + 0x0070,
    GX6605S_SVPU_ZOOM_PHASE                     = GX6605S_SVPU_OFFSET + 0x0074,
    GX6605S_SVPU_SYS_PARA                       = GX6605S_SVPU_OFFSET + 0x0078,
    GX6605S_SVPU_PARA_UPDATE                    = GX6605S_SVPU_OFFSET + 0x007c,
    GX6605S_SVPU_DISP_REQ_BLOCK                 = GX6605S_SVPU_OFFSET + 0x0080,
    GX6605S_SVPU_DISP_CTRL                      = GX6605S_SVPU_OFFSET + 0x0084,
    GX6605S_SVPU_VIEW_V                         = GX6605S_SVPU_OFFSET + 0x0088,
    GX6605S_SVPU_VIEW_H                         = GX6605S_SVPU_OFFSET + 0x008c,
    GX6605S_SVPU_VPU_FRAME_MODE                 = GX6605S_SVPU_OFFSET + 0x0090,
    GX6605S_SVPU_SCE_PHASE_PARA                 = GX6605S_SVPU_OFFSET + 0x0100,
};

enum gx6605s_hdmi_bases {
    GX6605S_HDMI_TX_BASE                        = GX6605S_HDMI_OFFSET + (0x0200 << 2),
    GX6605S_HDMI_VP_BASE                        = GX6605S_HDMI_OFFSET + (0x0800 << 2),
    GX6605S_HDMI_FC_BASE                        = GX6605S_HDMI_OFFSET + (0x1000 << 2),
    GX6605S_HDMI_MC_BASE                        = GX6605S_HDMI_OFFSET + (0x4000 << 2),
    GX6605S_HDMI_CSC_BASE                       = GX6605S_HDMI_OFFSET + (0x4100 << 2),
};

enum gx6605s_hdmi_resgierts {
    GX6605S_HDMI_VP_PR_CD                       = GX6605S_HDMI_VP_BASE + (0x0001 << 2),
    GX6605S_HDMI_VP_STUFF                       = GX6605S_HDMI_VP_BASE + (0x0002 << 2),
    GX6605S_HDMI_VP_REMAP                       = GX6605S_HDMI_VP_BASE + (0x0003 << 2),
    GX6605S_HDMI_VP_CONF                        = GX6605S_HDMI_VP_BASE + (0x0004 << 2),

    GX6605S_HDMI_FC_INVIDCONF                   = GX6605S_HDMI_FC_BASE + (0x0000 << 2),
    GX6605S_HDMI_FC_INHACTV0                    = GX6605S_HDMI_FC_BASE + (0x0001 << 2),
    GX6605S_HDMI_FC_INHACTV1                    = GX6605S_HDMI_FC_BASE + (0x0002 << 2),
    GX6605S_HDMI_FC_INHBLANK0                   = GX6605S_HDMI_FC_BASE + (0x0003 << 2),
    GX6605S_HDMI_FC_INHBLANK1                   = GX6605S_HDMI_FC_BASE + (0x0004 << 2),
    GX6605S_HDMI_FC_INVACTV0                    = GX6605S_HDMI_FC_BASE + (0x0005 << 2),
    GX6605S_HDMI_FC_INVACTV1                    = GX6605S_HDMI_FC_BASE + (0x0006 << 2),
    GX6605S_HDMI_FC_INVBLANK                    = GX6605S_HDMI_FC_BASE + (0x0007 << 2),
    GX6605S_HDMI_FC_HSYNCINDELAY0               = GX6605S_HDMI_FC_BASE + (0x0008 << 2),
    GX6605S_HDMI_FC_HSYNCINDELAY1               = GX6605S_HDMI_FC_BASE + (0x0009 << 2),
    GX6605S_HDMI_FC_HSYNCINWIDTH0               = GX6605S_HDMI_FC_BASE + (0x000a << 2),
    GX6605S_HDMI_FC_HSYNCINWIDTH1               = GX6605S_HDMI_FC_BASE + (0x000b << 2),
    GX6605S_HDMI_FC_VSYNCINDELAY                = GX6605S_HDMI_FC_BASE + (0x000c << 2),
    GX6605S_HDMI_FC_VSYNCINWIDTH                = GX6605S_HDMI_FC_BASE + (0x000d << 2),
    GX6605S_HDMI_FC_CTRLDUR                     = GX6605S_HDMI_FC_BASE + (0x0011 << 2),
    GX6605S_HDMI_FC_EXCTRLDUR                   = GX6605S_HDMI_FC_BASE + (0x0012 << 2),
    GX6605S_HDMI_FC_EXCTRLSPAC                  = GX6605S_HDMI_FC_BASE + (0x0013 << 2),
    GX6605S_HDMI_FC_CH0PREAM                    = GX6605S_HDMI_FC_BASE + (0x0014 << 2),
    GX6605S_HDMI_FC_CH1PREAM                    = GX6605S_HDMI_FC_BASE + (0x0015 << 2),
    GX6605S_HDMI_FC_CH2PREAM                    = GX6605S_HDMI_FC_BASE + (0x0016 << 2),
    GX6605S_HDMI_FC_PRCONF                      = GX6605S_HDMI_FC_BASE + (0x00e0 << 2),
    GX6605S_HDMI_FC_DBGFORCE                    = GX6605S_HDMI_FC_BASE + (0x0200 << 2),
    GX6605S_HDMI_FC_DBGTMDS0                    = GX6605S_HDMI_FC_BASE + (0x0219 << 2),
    GX6605S_HDMI_FC_DBGTMDS1                    = GX6605S_HDMI_FC_BASE + (0x021a << 2),
    GX6605S_HDMI_FC_DBGTMDS2                    = GX6605S_HDMI_FC_BASE + (0x021b << 2),

    GX6605S_HDMI_MC_CLKDIS                      = GX6605S_HDMI_MC_BASE + (0x0001 << 2),
    GX6605S_HDMI_MC_FLOWCTRL                    = GX6605S_HDMI_MC_BASE + (0x0004 << 2),
    GX6605S_HDMI_MC_PHYRSTZ                     = GX6605S_HDMI_MC_BASE + (0x0005 << 2),
    GX6605S_HDMI_MC_HEACPHY_RST                 = GX6605S_HDMI_MC_BASE + (0x0007 << 2),

    GX6605S_HDMI_CSC_CFG                        = GX6605S_HDMI_CSC_BASE + (0x0000 << 2),
    GX6605S_HDMI_CSC_SCALE                      = GX6605S_HDMI_CSC_BASE + (0x0001 << 2),
    GX6605S_HDMI_CSC_COEF_A1_MSB                = GX6605S_HDMI_CSC_BASE + (0x0002 << 2),
    GX6605S_HDMI_CSC_COEF_A1_LSB                = GX6605S_HDMI_CSC_BASE + (0x0003 << 2),
    GX6605S_HDMI_CSC_COEF_A2_MSB                = GX6605S_HDMI_CSC_BASE + (0x0004 << 2),
    GX6605S_HDMI_CSC_COEF_A2_LSB                = GX6605S_HDMI_CSC_BASE + (0x0005 << 2),
    GX6605S_HDMI_CSC_COEF_A3_MSB                = GX6605S_HDMI_CSC_BASE + (0x0006 << 2),
    GX6605S_HDMI_CSC_COEF_A3_LSB                = GX6605S_HDMI_CSC_BASE + (0x0007 << 2),
    GX6605S_HDMI_CSC_COEF_A4_MSB                = GX6605S_HDMI_CSC_BASE + (0x0008 << 2),
    GX6605S_HDMI_CSC_COEF_A4_LSB                = GX6605S_HDMI_CSC_BASE + (0x0009 << 2),
    GX6605S_HDMI_CSC_COEF_B1_MSB                = GX6605S_HDMI_CSC_BASE + (0x000a << 2),
    GX6605S_HDMI_CSC_COEF_B1_LSB                = GX6605S_HDMI_CSC_BASE + (0x000b << 2),
    GX6605S_HDMI_CSC_COEF_B2_MSB                = GX6605S_HDMI_CSC_BASE + (0x000c << 2),
    GX6605S_HDMI_CSC_COEF_B2_LSB                = GX6605S_HDMI_CSC_BASE + (0x000d << 2),
    GX6605S_HDMI_CSC_COEF_B3_MSB                = GX6605S_HDMI_CSC_BASE + (0x000e << 2),
    GX6605S_HDMI_CSC_COEF_B3_LSB                = GX6605S_HDMI_CSC_BASE + (0x000f << 2),
    GX6605S_HDMI_CSC_COEF_B4_MSB                = GX6605S_HDMI_CSC_BASE + (0x0010 << 2),
    GX6605S_HDMI_CSC_COEF_B4_LSB                = GX6605S_HDMI_CSC_BASE + (0x0011 << 2),
    GX6605S_HDMI_CSC_COEF_C1_MSB                = GX6605S_HDMI_CSC_BASE + (0x0012 << 2),
    GX6605S_HDMI_CSC_COEF_C1_LSB                = GX6605S_HDMI_CSC_BASE + (0x0013 << 2),
    GX6605S_HDMI_CSC_COEF_C2_MSB                = GX6605S_HDMI_CSC_BASE + (0x0014 << 2),
    GX6605S_HDMI_CSC_COEF_C2_LSB                = GX6605S_HDMI_CSC_BASE + (0x0015 << 2),
    GX6605S_HDMI_CSC_COEF_C3_MSB                = GX6605S_HDMI_CSC_BASE + (0x0016 << 2),
    GX6605S_HDMI_CSC_COEF_C3_LSB                = GX6605S_HDMI_CSC_BASE + (0x0017 << 2),
    GX6605S_HDMI_CSC_COEF_C4_MSB                = GX6605S_HDMI_CSC_BASE + (0x0018 << 2),
    GX6605S_HDMI_CSC_COEF_C4_LSB                = GX6605S_HDMI_CSC_BASE + (0x0019 << 2),
};

enum gx6605s_svpu_buffer {
    GX6605S_BUFF_Y_TOPFIELD_ADDR                = 0x00,
    GX6605S_BUFF_Y_BOTTOMFIELD_ADDR             = 0x04,
    GX6605S_BUFF_U_TOPFIELD_ADDR                = 0x08,
    GX6605S_BUFF_U_BOTTOMFIELD_ADDR             = 0x0c,
    GX6605S_BUFF_V_TOPFIELD_ADDR                = 0x10,
    GX6605S_BUFF_V_BOTTOMFIELD_ADDR             = 0x14,
};

enum gx6605s_osd_region {
    GX6605S_OSDR_CTRL                           = 0x00,
    GX6605S_OSDR_CLUT_PTR                       = 0x04,
    GX6605S_OSDR_WIDTH                          = 0x08,
    GX6605S_OSDR_HIGHT                          = 0x0c,
    GX6605S_OSDR_FBADDR                         = 0x10,
    GX6605S_OSDR_NEXT_PTR                       = 0x14,
    GX6605S_OSDR_ALPHA                          = 0x18,
};

enum gx6605s_color_format {
    GX6605S_COLOR_FMT_CLUT1                     = 0,
    GX6605S_COLOR_FMT_CLUT2                     = 1,
    GX6605S_COLOR_FMT_CLUT4                     = 2,
    GX6605S_COLOR_FMT_CLUT8                     = 3,
    GX6605S_COLOR_FMT_RGBA4444                  = 4,
    GX6605S_COLOR_FMT_RGBA5551                  = 5,
    GX6605S_COLOR_FMT_RGB565                    = 6,
    GX6605S_COLOR_FMT_RGBA8888                  = 7,
    GX6605S_COLOR_FMT_RGB888                    = 8,
    GX6605S_COLOR_FMT_BGR888                    = 9,

    GX6605S_COLOR_FMT_ARGB4444                  = 10,
    GX6605S_COLOR_FMT_ARGB1555                  = 11,
    GX6605S_COLOR_FMT_ARGB8888                  = 12,

    GX6605S_COLOR_FMT_YCBCR422                  = 13,
    GX6605S_COLOR_FMT_YCBCRA6442                = 14,
    GX6605S_COLOR_FMT_YCBCR420                  = 15,

    GX6605S_COLOR_FMT_YCBCR420_Y_UV             = 16,
    GX6605S_COLOR_FMT_YCBCR420_Y_U_V            = 17,
    GX6605S_COLOR_FMT_YCBCR420_Y                = 18,
    GX6605S_COLOR_FMT_YCBCR420_U                = 19,
    GX6605S_COLOR_FMT_YCBCR420_V                = 20,
    GX6605S_COLOR_FMT_YCBCR420_UV               = 21,

    GX6605S_COLOR_FMT_YCBCR422_Y_UV             = 22,
    GX6605S_COLOR_FMT_YCBCR422_Y_U_V            = 23,
    GX6605S_COLOR_FMT_YCBCR422_Y                = 24,
    GX6605S_COLOR_FMT_YCBCR422_U                = 25,
    GX6605S_COLOR_FMT_YCBCR422_V                = 26,
    GX6605S_COLOR_FMT_YCBCR422_UV               = 27,

    GX6605S_COLOR_FMT_YCBCR444                  = 28,
    GX6605S_COLOR_FMT_YCBCR444_Y_UV             = 29,
    GX6605S_COLOR_FMT_YCBCR444_Y_U_V            = 30,
    GX6605S_COLOR_FMT_YCBCR444_Y                = 31,
    GX6605S_COLOR_FMT_YCBCR444_U                = 32,
    GX6605S_COLOR_FMT_YCBCR444_V                = 33,
    GX6605S_COLOR_FMT_YCBCR444_UV               = 34,

    GX6605S_COLOR_FMT_YCBCR400                  = 35,
    GX6605S_COLOR_FMT_A8                        = 36,
    GX6605S_COLOR_FMT_ABGR4444                  = 37,
    GX6605S_COLOR_FMT_ABGR1555                  = 39,
    GX6605S_COLOR_FMT_Y8                        = 40,
    GX6605S_COLOR_FMT_UV16                      = 41,
    GX6605S_COLOR_FMT_YCBCR422v                 = 42,
    GX6605S_COLOR_FMT_YCBCR422h                 = 43,
};

enum gx6605s_byte_order {
    GX6605S_ORDER_DCBA_HGFE                     = 0,
    GX6605S_ORDER_EFGH_ABCD                     = 1,
    GX6605S_ORDER_HGFE_DCBA                     = 2,
    GX6605S_ORDER_ABCD_EFGH                     = 3,
    GX6605S_ORDER_CDAB_GHEF                     = 4,
    GX6605S_ORDER_FEHG_BADC                     = 5,
    GX6605S_ORDER_GHEF_CDAB                     = 6,
    GX6605S_ORDER_BADC_FEHG                     = 7,
};

/************************************************************************************************/
/*      Mnemonic                                value        meaning/usage                      */

#define GX6605S_VPU_OSD_CTRL_EN                 BIT(0)
#define GX6605S_VPU_OSD_CTRL_ZOOM_MODE_EN_IPS   BIT(25)
#define GX6605S_VPU_OSD_CTRL_ANTI_FLICKER       BIT(29)
#define GX6605S_VPU_OSD_CTRL_ANTI_FLICKER_CBCR  BIT(31)
#define GX6605S_VPU_SYS_PARA_BYTESEQ_LOW        BIT_RANGE(22, 20)
#define GX6605S_VPU_SYS_PARA_BYTESEQ_HIGH       BIT(28)
#define GX6605S_VPU_BUFF_CTRL2_REQ_LEN          BIT_RANGE(10, 0)
#define GX6605S_VPU_BUFF_CTRL2_STATE_DELAY      BIT_RANGE(23, 16)
#define GX6605S_VPU_OSD_POSITION_X              BIT_RANGE(9, 0)
#define GX6605S_VPU_OSD_POSITION_Y              BIT_RANGE(25, 16)

#define GX6605S_VOUT_DAC_OUT_MODE               BIT_RANGE(18, 17)
#define GX6605S_VOUT_FRAME_FRE                  BIT(5)
#define GX6605S_VOUT_TV_FORMAT                  BIT_RANGE(4, 0)

#define GX6605S_SVPU_CAP_CTRL_EN                BIT(0)
#define GX6605S_SVPU_CAP_CTRL_CAP_LEVEL         BIT(1)
#define GX6605S_SVPU_CAP_CTRL_VDOWNSCALE_EN     BIT(4)
#define GX6605S_SVPU_CAP_CTRL_MODE              BIT_RANGE(28, 24)
#define GX6605S_SVPU_CAP_CTRL_MODE_EN           BIT(31)
#define GX6605S_SVPU_CAP_V_LEFT                 BIT_RANGE(10, 0)
#define GX6605S_SVPU_CAP_V_RIGHT                BIT_RANGE(26, 16)
#define GX6605S_SVPU_CAP_H_TOP                  BIT_RANGE(10, 0)
#define GX6605S_SVPU_CAP_H_BOTTOM               BIT_RANGE(26, 16)
#define GX6605S_SVPU_SCE_ZOOM_V                 BIT_RANGE(10, 0)
#define GX6605S_SVPU_SCE_ZOOM_H                 BIT_RANGE(26, 16)
#define GX6605S_SVPU_SCE_ZOOM1_LENGTH           BIT_RANGE(10, 0)
#define GX6605S_SVPU_SCE_ZOOM1_HEIGHT           BIT_RANGE(26, 16)
#define GX6605S_SVPU_REQ_LENGTH_WRITE           BIT_RANGE(26, 16)
#define GX6605S_SVPU_ZOOM_PHASE_VT              BIT_RANGE(10, 0)
#define GX6605S_SVPU_ZOOM_PHASE_VB              BIT_RANGE(26, 16)
#define GX6605S_SVPU_PARA_UPDATE_EN             BIT(0)
#define GX6605S_SVPU_DISP_REQ_BLOCK_READ        BIT_RANGE(26, 16)
#define GX6605S_SVPU_DISP_CTRL_EN               BIT(0)
#define GX6605S_SVPU_VIEW_V_RIGHT               BIT_RANGE(10, 0)
#define GX6605S_SVPU_VIEW_V_LEFT                BIT_RANGE(26, 16)
#define GX6605S_SVPU_VIEW_H_BOTTOM              BIT_RANGE(10, 0)
#define GX6605S_SVPU_VIEW_H_TOP                 BIT_RANGE(26, 16)
#define GX6605S_SVPU_VPU_FRAME_MODE_23          BIT(0)

#define GX6605S_OSDR_CTRL_CLUT_SWITCH           BIT_RANGE(7, 0)
#define GX6605S_OSDR_CTRL_CLUT_LENGTH           BIT_RANGE(9, 8)
#define GX6605S_OSDR_CTRL_COLOR_MODE            BIT_RANGE(12, 10)
#define GX6605S_OSDR_CTRL_CLUT_UPDATA_EN        BIT(13)
#define GX6605S_OSDR_CTRL_FLIKER_FLITER_EN      BIT(14)
#define GX6605S_OSDR_CTRL_COLOR_KEY_EN          BIT(15)
#define GX6605S_OSDR_CTRL_MIX_WEIGHT            BIT_RANGE(22, 16)
#define GX6605S_OSDR_CTRL_GLOBAL_ALPHA_EN       BIT(23)
#define GX6605S_OSDR_CTRL_TRUE_COLOR_MODE       BIT_RANGE(25, 24)
#define GX6605S_OSDR_CTRL_ARGB_CONVERT          BIT(26)
#define GX6605S_OSDR_WIDTH_LEFT                 BIT_RANGE(10, 0)
#define GX6605S_OSDR_WIDTH_RIGHT                BIT_RANGE(26, 16)
#define GX6605S_OSDR_HIGHT_TOP                  BIT_RANGE(10, 0)
#define GX6605S_OSDR_HIGHT_BOTTOM               BIT_RANGE(26, 16)
#define GX6605S_OSDR_BASELINE                   BIT_RANGE(12, 0)
#define GX6605S_OSDR_ALPHA_RATIO                BIT_RANGE(23, 16)
#define GX6605S_OSDR_ALPHA_RATIO_EN             BIT(24)
#define GX6605S_OSDR_LIST_END                   BIT(31)

#endif  /* __GX6605S_VPU_H__ */
