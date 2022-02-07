/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_VIDEO_H__
#define __GX6605S_VIDEO_H__

#include <gx6605s-vpu.h>

enum gx6605s_hdmi_encoding {
    RGB                     = 1,
    YCC444                  = 2,
    YCC422                  = 3,
};

enum gx6605s_hdmi_colorimetry {
    ITU601                  = 1,
    ITU709                  = 2,
    EXTENDED_COLORIMETRY    = 3,
};

struct gx6605s_hdmi_dtd {
    uint8_t mCode;
    uint16_t mPixelRepetitionInput;
    uint16_t mPixelClock;
    uint8_t mInterlaced;
    uint16_t mHActive;
    uint16_t mHBlanking;
    uint16_t mHBorder;
    uint16_t mHImageSize;
    uint16_t mHSyncOffset;
    uint16_t mHSyncPulseWidth;
    uint8_t mHSyncPolarity;
    uint16_t mVActive;
    uint16_t mVBlanking;
    uint16_t mVBorder;
    uint16_t mVImageSize;
    uint16_t mVSyncOffset;
    uint16_t mVSyncPulseWidth;
    uint8_t mVSyncPolarity;
};

struct gx6605s_hdmi_params {
    uint8_t mHdmi;
    enum gx6605s_hdmi_encoding mEncodingOut;
    enum gx6605s_hdmi_encoding mEncodingIn;
    uint8_t mColorResolution;
    uint8_t mPixelRepetitionFactor;
    struct gx6605s_hdmi_dtd mDtd;
    uint8_t mRgbQuantizationRange;
    uint8_t mPixelPackingDefaultPhase;
    uint8_t mColorimetry;
    uint8_t mScanInfo;
    uint8_t mActiveFormatAspectRatio;
    uint8_t mNonUniformScaling;
    uint8_t mExtColorimetry;
    uint8_t mItContent;
    uint16_t mEndTopBar;
    uint16_t mStartBottomBar;
    uint16_t mEndLeftBar;
    uint16_t mStartRightBar;
    uint16_t mCscFilter;
    uint16_t mCscA[4];
    uint16_t mCscC[4];
    uint16_t mCscB[4];
    uint16_t mCscScale;
    uint8_t mHdmiVideoFormat;
    uint8_t m3dStructure;
    uint8_t m3dExtData;
    uint8_t mHdmiVic;
};

enum gx6605s_hdmi_mode {
    GX6605S_HDMI_480I           = 0,
    GX6605S_HDMI_480P           = 1,
    GX6605S_HDMI_576I           = 2,
    GX6605S_HDMI_576P           = 3,
    GX6605S_HDMI_720P_50HZ      = 4,
    GX6605S_HDMI_720P_60HZ      = 5,
    GX6605S_HDMI_1080I_50HZ     = 6,
    GX6605S_HDMI_1080I_60HZ     = 7,
    GX6605S_HDMI_1080P_50HZ     = 8,
    GX6605S_HDMI_1080P_60HZ     = 9,
    GX6605S_HDMI_NR_MAX         = 10,
    GX6605S_HDMI_DISABLE        = 10,
};

enum gx6605s_cvbs_mode {
    GX6605S_CVBS_PAL            = 0,
    GX6605S_CVBS_PAL_M          = 1,
    GX6605S_CVBS_PAL_N          = 2,
    GX6605S_CVBS_PAL_NC         = 3,
    GX6605S_CVBS_NTSC_M         = 4,
    GX6605S_CVBS_NTSC_443       = 5,
    GX6605S_CVBS_NR_MAX         = 6,
    GX6605S_CVBS_DISABLE        = 6,
};

struct gx6605s_video_mode {
    const char *name;
    unsigned int refresh;
    unsigned int xres;
    unsigned int yres;
    bool_t interlaced;
};

struct gx6605s_device {
    enum gx6605s_hdmi_mode hdmi;
    enum gx6605s_cvbs_mode cvbs;
    unsigned int bpp;
    struct gx6605s_hdmi_params video;
    struct region_list_t *nrl, *orl;
    bool_t brightness;
    virtual_addr_t base;
    virtual_addr_t region;
    virtual_addr_t vram[2];
    virtual_addr_t svpu_buff;
    unsigned int index;
    int byteseq;
};

#define constant_swab32(x) ((uint32_t)(                 \
    (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |  \
    (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |  \
    (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |  \
    (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))

#define constant_swahw32(x) ((uint32_t)(                \
    (((uint32_t)(x) & (uint32_t)0x0000ffffUL) << 16) |  \
    (((uint32_t)(x) & (uint32_t)0xffff0000UL) >> 16)))

static inline uint32_t
gx6605s_readl(struct gx6605s_device *gdev, uint32_t reg)
{
    return read32(gdev->base + reg);
}

static inline void
gx6605s_writel(struct gx6605s_device *gdev, uint32_t reg, uint32_t val)
{
    write32(gdev->base + reg, val);
}

static inline void
gx6605s_mask(struct gx6605s_device *gdev, uint32_t reg, uint32_t clr, uint32_t set)
{
    uint32_t val = gx6605s_readl(gdev, reg);
    gx6605s_writel(gdev, reg, (val & ~clr) | set);
}

static inline uint32_t
gx6605s_config_readl(struct gx6605s_device *gdev, uint32_t reg)
{
    return read32(GCTL_BASE + reg);
}

static inline void
gx6605s_config_writel(struct gx6605s_device *gdev, uint32_t reg, uint32_t val)
{
    write32(GCTL_BASE + reg, val);
}

static inline void
gx6605s_config_mask(struct gx6605s_device *gdev, uint32_t reg, uint32_t clr, uint32_t set)
{
    uint32_t val = gx6605s_config_readl(gdev, reg);
    gx6605s_config_writel(gdev, reg, (val & ~clr) | set);
}

static inline uint32_t
gx6605s_region_get(struct gx6605s_device *gdev, uint16_t index)
{
    uint32_t val = read32(gdev->region + index);
    if (gdev->byteseq == GX6605S_ORDER_DCBA_HGFE)
        val = constant_swab32(val);
    else if (gdev->byteseq == GX6605S_ORDER_CDAB_GHEF)
        val = constant_swahw32(val);
    return val;
}

static inline void
gx6605s_region_set(struct gx6605s_device *gdev, uint16_t index, uint32_t val)
{
    if (gdev->byteseq == GX6605S_ORDER_DCBA_HGFE)
        val = constant_swab32(val);
    else if (gdev->byteseq == GX6605S_ORDER_CDAB_GHEF)
        val = constant_swahw32(val);
    write32(gdev->region + index, val);
}

static inline void
gx6605s_region_mask(struct gx6605s_device *gdev, uint16_t index, uint32_t clr, uint32_t set)
{
    uint32_t val = gx6605s_region_get(gdev, index);
    gx6605s_region_set(gdev, index, (val & ~clr) | set);
}

extern struct gx6605s_video_mode gx6605s_video_modes[GX6605S_HDMI_NR_MAX];
extern int dw_hdmi_configure(struct gx6605s_hdmi_params *video, uint16_t ceacode);
extern int gx6605s_vpu_hwinit(struct gx6605s_device *gdev);
extern void gx6605s_vpu_zoom(struct gx6605s_device *gdev, unsigned int layer, unsigned int source_width, unsigned int source_height, unsigned int dest_width, unsigned int dest_height, unsigned int start_x, unsigned int start_y);
extern void gx6605s_osd_enable(struct gx6605s_device *gdev, bool_t enable);
extern int gx6605s_osd_hwinit(struct gx6605s_device *gdev);

#endif  /* __GX6605S_VIDEO_H__ */
