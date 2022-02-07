/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include "gx6605s-video.h"

static uint32_t palette_buffer[2] = {
    0, 0xff00ff00
};

void gx6605s_osd_enable(struct gx6605s_device *gdev, bool_t enable)
{
    while ((gx6605s_readl(gdev, GX6605S_VPU_OSD_CTRL) & GX6605S_VPU_OSD_CTRL_EN) != enable)
        gx6605s_mask(gdev, GX6605S_VPU_OSD_CTRL, GX6605S_VPU_OSD_CTRL_EN, enable);
}

static void gx6605s_osd_endian_set(struct gx6605s_device *gdev, enum gx6605s_byte_order byte_seq)
{
    gdev->byteseq = byte_seq;

    if (byte_seq >> 2)
        gx6605s_mask(gdev, GX6605S_VPU_SYS_PARA, 0, GX6605S_VPU_SYS_PARA_BYTESEQ_HIGH);
    else
        gx6605s_mask(gdev, GX6605S_VPU_SYS_PARA, GX6605S_VPU_SYS_PARA_BYTESEQ_HIGH, 0);

    gx6605s_mask(gdev, GX6605S_VPU_SYS_PARA, GX6605S_VPU_SYS_PARA_BYTESEQ_LOW | 1,
                (byte_seq << 20) & GX6605S_VPU_SYS_PARA_BYTESEQ_LOW);
    gx6605s_mask(gdev, GX6605S_VPU_SYS_PARA, 0x07<<12, (byte_seq & 0x07) << 12);
}

static void gx6605s_osd_alpha_set(struct gx6605s_device *gdev, uint32_t alpha)
{
    gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_MIX_WEIGHT,
        GX6605S_OSDR_CTRL_GLOBAL_ALPHA_EN | (GX6605S_OSDR_CTRL_MIX_WEIGHT & (0xff << 16)));
    gx6605s_region_mask(gdev, GX6605S_OSDR_ALPHA, GX6605S_OSDR_ALPHA_RATIO_EN, 0);
}

static void gx6605s_osd_format_set(struct gx6605s_device *gdev, enum gx6605s_color_format format)
{
    if (format <= GX6605S_COLOR_FMT_CLUT8)
        gx6605s_mask(gdev, GX6605S_VPU_OSD_CTRL, GX6605S_VPU_OSD_CTRL_ZOOM_MODE_EN_IPS, 0);
    else
        gx6605s_mask(gdev, GX6605S_VPU_OSD_CTRL, 0, GX6605S_VPU_OSD_CTRL_ZOOM_MODE_EN_IPS);

    if ((GX6605S_COLOR_FMT_RGBA8888 <= format) && (format <= GX6605S_COLOR_FMT_BGR888)) {
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, 0, GX6605S_OSDR_CTRL_COLOR_MODE);
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_TRUE_COLOR_MODE,
                           (format - GX6605S_COLOR_FMT_RGBA8888) << 24);
    } else {
        if (format == GX6605S_COLOR_FMT_ARGB4444 || format == GX6605S_COLOR_FMT_ARGB1555 ||
            format == GX6605S_COLOR_FMT_ARGB8888)
            gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, 0, GX6605S_OSDR_CTRL_ARGB_CONVERT);
        else
            gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_ARGB_CONVERT, 0);

        switch (format) {
            case GX6605S_COLOR_FMT_ARGB4444:
                format = GX6605S_COLOR_FMT_RGBA4444;
                break;
            case GX6605S_COLOR_FMT_ARGB1555:
                format = GX6605S_COLOR_FMT_RGBA5551;
                break;
            case GX6605S_COLOR_FMT_ARGB8888:
                format = GX6605S_COLOR_FMT_RGBA8888;
                break;
            default:
                break;
        }

        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_COLOR_MODE,
                           (format << 10) & GX6605S_OSDR_CTRL_COLOR_MODE);
    }
}

int gx6605s_osd_hwinit(struct gx6605s_device *gdev)
{
    unsigned int byte_seq, request_block;
    struct gx6605s_video_mode *mode = &gx6605s_video_modes[gdev->hdmi];

    /* set vpu byte-sequence (both data and command) */
    if (gdev->bpp == 32)
        byte_seq = GX6605S_ORDER_ABCD_EFGH;
    else if (gdev->bpp == 16)
        byte_seq = GX6605S_ORDER_CDAB_GHEF;
    else if (gdev->bpp <= 8)
        byte_seq = GX6605S_ORDER_DCBA_HGFE;
    gx6605s_osd_endian_set(gdev, byte_seq);

    if (gdev->bpp == 1)
        request_block = mode->xres / 4 / 128 * 128 / 8;
    else
        request_block = mode->xres * (gdev->bpp >> 3) / 4 / 128 * 128;
    request_block = clamp(request_block, 128U, 896U);

    gx6605s_mask(gdev, GX6605S_VPU_BUFF_CTRL2, GX6605S_VPU_BUFF_CTRL2_REQ_LEN,
                 request_block & GX6605S_VPU_BUFF_CTRL2_REQ_LEN);

    /* we only need one layer to display */
    gx6605s_region_mask(gdev, GX6605S_OSDR_WIDTH, GX6605S_OSDR_WIDTH_LEFT, 0);
    gx6605s_region_mask(gdev, GX6605S_OSDR_WIDTH, GX6605S_OSDR_WIDTH_RIGHT, (mode->xres - 1) << 16);
    gx6605s_region_mask(gdev, GX6605S_OSDR_HIGHT, GX6605S_OSDR_HIGHT_TOP, 0);
    gx6605s_region_mask(gdev, GX6605S_OSDR_HIGHT, GX6605S_OSDR_HIGHT_BOTTOM, (mode->yres - 1) << 16);
    gx6605s_writel(gdev, GX6605S_VPU_OSD_POSITION, 0);

    gx6605s_osd_alpha_set(gdev, 0xff);

    if (gdev->bpp == 1) {
        gx6605s_osd_format_set(gdev, GX6605S_COLOR_FMT_CLUT1);
        gx6605s_region_set(gdev, GX6605S_OSDR_CLUT_PTR, virt_to_phys((virtual_size_t)palette_buffer));
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, GX6605S_OSDR_CTRL_CLUT_LENGTH, 0);
        gx6605s_region_mask(gdev, GX6605S_OSDR_CTRL, 0, GX6605S_OSDR_CTRL_CLUT_UPDATA_EN);
    } else if (gdev->bpp == 16)
        gx6605s_osd_format_set(gdev, GX6605S_COLOR_FMT_RGB565);
    else if (gdev->bpp == 32)
        gx6605s_osd_format_set(gdev, GX6605S_COLOR_FMT_RGB888);

    /* configure the address of the layer frame buffer */
    gx6605s_region_set(gdev, GX6605S_OSDR_FBADDR, virt_to_phys(gdev->vram[0]));
    gx6605s_region_set(gdev, GX6605S_OSDR_NEXT_PTR, virt_to_phys(gdev->region));

    gx6605s_region_mask(gdev, GX6605S_OSDR_ALPHA, 0, GX6605S_OSDR_LIST_END);
    gx6605s_region_mask(gdev, GX6605S_OSDR_ALPHA, GX6605S_OSDR_BASELINE, mode->xres);

    gx6605s_mask(gdev, GX6605S_VPU_OSD_CTRL, GX6605S_VPU_OSD_CTRL_ANTI_FLICKER_CBCR, 0);
    gx6605s_writel(gdev, GX6605S_VPU_OSD_VIEW_SIZE, (mode->yres << 16) | mode->xres);
    gx6605s_writel(gdev, GX6605S_VPU_OSD_ZOOM, 0x10001000);

    /* set the first child of the layer list */
    gx6605s_writel(gdev, GX6605S_VPU_OSD_FIRST_HEAD_PTR, virt_to_phys(gdev->region));

    return -ENOERR;
}
