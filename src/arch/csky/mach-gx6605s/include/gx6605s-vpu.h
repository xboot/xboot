/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_VIDEO_H__
#define __GX6605S_VIDEO_H__

#include <gx6605s.h>

enum gx6605s_jpeg_registers {
    GX6605S_JPEG_DEC_CTRL                       = 0x00,
    GX6605S_JPEG_BS_BUF_START_ADDR              = 0x04,
    GX6605S_JPEG_BS_BUF_SIZE                    = 0x08,
    GX6605S_JPEG_BS_BUFFER_ALMOST_EMPTY_TH      = 0x0c,
    GX6605S_JPEG_BS_BUFFER_WR_PTR               = 0x10,
    GX6605S_JPEG_BS_BUFFER_RD_PTR               = 0x14,
    GX6605S_JPEG_FRAME_BUFFER_Y_BASE_ADDR       = 0x18,
    GX6605S_JPEG_FRAME_BUFFER_CB_BASE_ADDR      = 0x1c,
    GX6605S_JPEG_FRAME_BUFFER_CR_BASE_ADDR      = 0x20,
    GX6605S_JPEG_FRAME_BUFFER_STRIDE            = 0x24,
    GX6605S_JPEG_FRAME_BUFFER_MAX_PIC_SIZE      = 0x28,
    GX6605S_JPEG_CLIP_UPPER_LEFT_COORDINATE     = 0x2c,
    GX6605S_JPEG_CLIP_LOWER_RIGHT_COORDINATE    = 0x30,
    GX6605S_JPEG_DECODE_DEAD_OVER_TIME_GATE     = 0x34,
    GX6605S_JPEG_PIC_LINE_CNT_GATE              = 0x38,
    GX6605S_JPEG_PIC_INFO                       = 0x3c,
    GX6605S_JPEG_PIC_ORIGION_SIZE_INFO          = 0x40,
    GX6605S_JPEG_PIC_WRITE_BACK_SIZE_INFO       = 0x44,
    GX6605S_JPEG_PIC_DISPLAY_SIZE_INFO          = 0x48,
    GX6605S_JPEG_PIC_DISPLAY_COORDINATE_INFO    = 0x4c,
    GX6605S_JPEG_PIC_WB_LINE_INFO               = 0x50,
    GX6605S_JPEG_INT_STATUS                     = 0x54,
    GX6605S_JPEG_INT_ENABLE_CPU0                = 0x58,
    GX6605S_JPEG_INT_ENABLE_CPU1                = 0x5c,
    GX6605S_JPEG_INT_ENABLE_CPU2                = 0x60,
    GX6605S_JPEG_INT_ENABLE_CPU3                = 0x64,
};

enum gx6605s_vpu_registers {
    GX6605S_VPU_PP_CTRL                         = 0x00,
    GX6605S_VPU_PP_V_PHASE                      = 0x04,
    GX6605S_VPU_PP_POSITION                     = 0x08,
    GX6605S_VPU_PP_SOURCE_SIZE                  = 0x0c,
    GX6605S_VPU_PP_VIEW_SIZE                    = 0x10,
    GX6605S_VPU_PP_ZOOM                         = 0x14,
    GX6605S_VPU_PP_FRAME_STRIDE                 = 0x18,
    GX6605S_VPU_PP_FILTER_SIGN                  = 0x1c,
    GX6605S_VPU_PP_PHASE_0_H                    = 0x20,
    GX6605S_VPU_PP_PHASE_0_V                    = 0x24,
    GX6605S_VPU_PP_DISP_CTRL                    = 0x28,
    GX6605S_VPU_PP_DISP_R_PTR                   = 0x2c,
    GX6605S_VPU_PP_BACK_COLOR                   = 0x30,

    GX6605S_VPU_PIC_CTRL                        = 0x40,
    GX6605S_VPU_PIC_V_PHASE                     = 0x44,
    GX6605S_VPU_PIC_POSITION                    = 0x48,
    GX6605S_VPU_PIC_SOURCE_SIZE                 = 0x4c,
    GX6605S_VPU_PIC_VIEW_SIZE                   = 0x50,
    GX6605S_VPU_PIC_ZOOM                        = 0x54,
    GX6605S_VPU_PIC_PARA                        = 0x58,
    GX6605S_VPU_PIC_FILTER_SIGN                 = 0x5c,
    GX6605S_VPU_PIC_PHASE_0_H                   = 0x60,
    GX6605S_VPU_PIC_PHASE_0_V                   = 0x64,
    GX6605S_VPU_PIC_Y_TOP_ADDR                  = 0x68,
    GX6605S_VPU_PIC_Y_BOTTOM_ADDR               = 0x6c,
    GX6605S_VPU_PIC_UV_TOP_ADDR                 = 0x70,
    GX6605S_VPU_PIC_UV_BOTTOM_ADDR              = 0x74,
    GX6605S_VPU_PIC_BACK_COLOR                  = 0x78,

    GX6605S_VPU_OSD_CTRL                        = 0x90,
    GX6605S_VPU_OSD_FIRST_HEAD_PTR              = 0x94,
    GX6605S_VPU_OSD_VIEW_SIZE                   = 0x98,
    GX6605S_VPU_OSD_ZOOM                        = 0x9c,
    GX6605S_VPU_OSD_COLOR_KEY                   = 0xa0,
    GX6605S_VPU_OSD_ALPHA_5551                  = 0xa4,
    GX6605S_VPU_OSD_PHASE_0                     = 0xa8,
    GX6605S_VPU_OSD_POSITION                    = 0xac,

    GX6605S_VPU_CAP_CTRL                        = 0xb0,
    GX6605S_VPU_CAP_ADDR                        = 0xb4,
    GX6605S_VPU_CAP_HEIGHT                      = 0xb8,
    GX6605S_VPU_CAP_WIDTH                       = 0xbc,

    GX6605S_VPU_VBI_CTRL                        = 0xd0,
    GX6605S_VPU_VBI_FIRST_ADDR                  = 0xd4,
    GX6605S_VPU_VBI_ADDR                        = 0xd8,

    GX6605S_VPU_MIX_CTRL                        = 0xdc,
    GX6605S_VPU_CHIPTEST                        = 0xe0,
    GX6605S_VPU_SCAN_LINE                       = 0xe4,
    GX6605S_VPU_SYS_PARA                        = 0xe8,
    GX6605S_VPU_BUFF_CTRL1                      = 0xec,
    GX6605S_VPU_BUFF_CTRL2                      = 0xf0,
    GX6605S_VPU_EMPTY_GATE_1                    = 0xf4,
    GX6605S_VPU_EMPTY_GATE_2                    = 0xf8,
    GX6605S_VPU_FULL_GATE                       = 0xfc,
    GX6605S_VPU_BUFFER_INT                      = 0x100,

    GX6605S_VPU_PP_PARA_H                       = 0x0200,   /* 0x0200 ~ 0x02fc */
    GX6605S_VPU_PP_PARA_V                       = 0x0300,   /* 0x0300 ~ 0x03fc */

    GX6605S_VPU_OSD_PARA                        = 0x0400,   /* 0x0400 ~ 0x04fc */

    GX6605S_VPU_DISP0_CTRL                      = 0x0500,   /* 0x0500 ~ 0x051c */
    GX6605S_VPU_DISP1_CTRL                      = 0x0520,   /* 0x0520 ~ 0x053c */
    GX6605S_VPU_DISP2_CTRL                      = 0x0540,   /* 0x0540 ~ 0x055c */
    GX6605S_VPU_DISP3_CTRL                      = 0x0560,   /* 0x0560 ~ 0x057c */
    GX6605S_VPU_DISP4_CTRL                      = 0x0580,   /* 0x0580 ~ 0x059c */
    GX6605S_VPU_DISP5_CTRL                      = 0x05a0,   /* 0x05a0 ~ 0x05bc */
    GX6605S_VPU_DISP6_CTRL                      = 0x05c0,   /* 0x05c0 ~ 0x05dc */
    GX6605S_VPU_DISP7_CTRL                      = 0x05e0,   /* 0x05e0 ~ 0x05fc */

    GX6605S_PIC_PARA_H                          = 0x0600,   /* 0x0600 ~ 0x06fc */
    GX6605S_PIC_PARA_V                          = 0x0700,   /* 0x0700 ~ 0x07fc */
};

enum gx6605s_svpu_registers {
    GX6605S_SVPU_CTRL                           = 0x00,
    GX6605S_SVPU_CTRL1                          = 0x04,
    GX6605S_SVPU_CAP_PARA_T                     = 0x08,
    GX6605S_SVPU_CAP_PARA_B                     = 0x0c,
    GX6605S_SVPU_RST_CTRL                       = 0x10,
    GX6605S_SVPU_ZOOM                           = 0x14,
    GX6605S_SVPU_V_PHASE                        = 0x18,
    GX6605S_SVPU_ZOOM_CTRL                      = 0x1c,
    GX6605S_SVPU_VBI_CTRL                       = 0x20,
    GX6605S_SVPU_VBI_FIRST_ADDR                 = 0x24,
    GX6605S_SVPU_VBI_ADDR                       = 0x28,
    GX6605S_SVPU_GAIN_DAC                       = 0x80,
    GX6605S_SVPU_POWER_DOWN                     = 0x84,
    GX6605S_SVPU_POWER_DOWN_BYSELF              = 0x88,
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

/************************************************************************************************/
/*      Mnemonic                                value        meaning/usage                      */

#define GX6605S_VPU_OSD_CTRL_EN                 BIT(0)
#define GX6605S_VPU_OSD_CTRL_ZOOM_MODE_EN_IPS   BIT(25)
#define GX6605S_VPU_OSD_CTRL_ANTI_FLICKER       BIT(29)
#define GX6605S_VPU_OSD_CTRL_ANTI_FLICKER_CBCR  BIT(31)

#define GX6605S_VPU_SYS_PARA_BYTESEQ_LOW        (0x03 << 20)
#define GX6605S_VPU_SYS_PARA_BYTESEQ_HIGH       BIT(28)

#define GX6605S_VPU_BUFF_CTRL2_REQ_LEN          (0x7ff << 0)
#define GX6605S_VPU_BUFF_CTRL2_STATE_DELAY      (0xff << 16)

#define GX6605S_VPU_OSD_POSITION_X              (0x3FF << 0)
#define GX6605S_VPU_OSD_POSITION_Y              (0x3FF << 16)

#define GX6605S_OSDR_CTRL_CLUT_SWITCH           BIT_RANGE(7, 0)
#define GX6605S_OSDR_CTRL_CLUT_LENGTH           BIT_RANGE(9, 8)
#define GX6605S_OSDR_CTRL_COLOR_MODE            BIT_RANGE(12, 10)
#define GX6605S_OSDR_CTRL_CLUT_UPDATA_EN        BIT(13)
#define GX6605S_OSDR_CTRL_FLIKER_FLITER_EN      BIT(14)
#define GX6605S_OSDR_CTRL_COLOR_KEY_EN          BIT(15)
#define GX6605S_OSDR_CTRL_MIX_WEIGHT            (0x7f << 16)
#define GX6605S_OSDR_CTRL_GLOBAL_ALPHA_EN       BIT(23)
#define GX6605S_OSDR_CTRL_TRUE_COLOR_MODE       BIT_RANGE(25, 24)
#define GX6605S_OSDR_CTRL_ARGB_CONVERT          BIT(26)

#define GX6605S_OSDR_WIDTH_LEFT                 (0x7FF << 0)
#define GX6605S_OSDR_WIDTH_RIGHT                (0x7FF << 16)

#define GX6605S_OSDR_HIGHT_TOP                  (0x7FF << 0)
#define GX6605S_OSDR_HIGHT_BOTTOM               (0x7FF << 16)

#define GX6605S_OSDR_BASELINE                   (0x1fff << 0)
#define GX6605S_OSDR_ALPHA_RATIO                (0xff << 16)
#define GX6605S_OSDR_ALPHA_RATIO_EN             BIT(24)
#define GX6605S_OSDR_LIST_END                   BIT(31)

#endif  /* __GX6605S_VIDEO_H__ */
