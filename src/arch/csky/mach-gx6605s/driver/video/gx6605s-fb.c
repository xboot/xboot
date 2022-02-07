/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <cache.h>
#include <dma/dma.h>
#include "gx6605s-video.h"

struct gx6605s_video_mode gx6605s_video_modes[GX6605S_HDMI_NR_MAX] = {
    [GX6605S_HDMI_480I] = {
        .name = "480I", .refresh = 60,
        .xres = 720, .yres = 480,
        .interlaced = TRUE,
    },
    [GX6605S_HDMI_480P] = {
        .name = "480P", .refresh = 60,
        .xres = 720, .yres = 480,
        .interlaced = FALSE,
    },
    [GX6605S_HDMI_576I] = {
        .name = "576I", .refresh = 60,
        .xres = 720, .yres = 576,
        .interlaced = TRUE,
    },
    [GX6605S_HDMI_576P] = {
        .name = "576P", .refresh = 60,
        .xres = 720, .yres = 576,
        .interlaced = FALSE,
    },
    [GX6605S_HDMI_720P_50HZ] = {
        .name = "720P_50HZ", .refresh = 50,
        .xres = 1280, .yres = 720,
        .interlaced = FALSE,
    },
    [GX6605S_HDMI_720P_60HZ] = {
        .name = "720P_60HZ", .refresh = 60,
        .xres = 1280, .yres = 720,
        .interlaced = FALSE,
    },
    [GX6605S_HDMI_1080I_50HZ] = {
        .name = "1080I_50HZ", .refresh = 50,
        .xres = 1920, .yres = 1080,
        .interlaced = TRUE,
    },
    [GX6605S_HDMI_1080I_60HZ] = {
        .name = "1080I_60HZ", .refresh = 60,
        .xres = 1920, .yres = 1080,
        .interlaced = TRUE,
    },
    [GX6605S_HDMI_1080P_50HZ] = {
        .name = "1080P_50HZ", .refresh = 50,
        .xres = 1920, .yres = 1080,
        .interlaced = FALSE,
    },
    [GX6605S_HDMI_1080P_60HZ] = {
        .name = "1080P_60HZ", .refresh = 60,
        .xres = 1920, .yres = 1080,
        .interlaced = FALSE,
    },
};

static enum gx6605s_hdmi_mode gx6605s_mode_best(unsigned int xres, unsigned int yres, unsigned int refresh, bool_t interlaced)
{
    unsigned int count;

    for (count = 0; count < GX6605S_HDMI_NR_MAX; ++count) {
        if (gx6605s_video_modes[count].xres >= xres &&
            gx6605s_video_modes[count].yres >= yres &&
            gx6605s_video_modes[count].refresh >= refresh &&
            gx6605s_video_modes[count].interlaced == interlaced)
            return count;
    }

    return GX6605S_HDMI_720P_50HZ;
}

static void fb_gx6605s_setbl(struct framebuffer_t *fb, int brightness)
{
    struct gx6605s_device *gdev = fb->priv;
    gx6605s_osd_enable(gdev, !!brightness);
    gdev->brightness = !!brightness;
}

static int fb_gx6605s_getbl(struct framebuffer_t *fb)
{
    struct gx6605s_device *gdev = fb->priv;
    return gdev->brightness;
}

static struct surface_t *fb_gx6605s_create(struct framebuffer_t *fb)
{
    struct gx6605s_device *gdev = fb->priv;
    struct gx6605s_video_mode *mode = &gx6605s_video_modes[gdev->hdmi];
    return surface_alloc(mode->xres, mode->yres, NULL);
}

static void fb_gx6605s_destroy(struct framebuffer_t *fb, struct surface_t *s)
{
    surface_free(s);
}

static void fb_gx6605s_present(struct framebuffer_t *fb, struct surface_t *s, struct region_list_t *rl)
{
    struct gx6605s_device *gdev = fb->priv;
    struct region_list_t *nrl = gdev->nrl;

    region_list_clear(nrl);
    region_list_merge(nrl, gdev->orl);
    region_list_merge(nrl, rl);
    region_list_clone(gdev->orl, rl);
    gdev->index = (gdev->index + 1) & 0x1;

    if (nrl->count > 0)
        present_surface((void *)gdev->vram[gdev->index], s, nrl);
    else
        memcpy((void *)gdev->vram[gdev->index], s->pixels, s->pixlen);

    dcache_writeback_all();
    gx6605s_region_set(gdev, GX6605S_OSDR_FBADDR, virt_to_phys(gdev->vram[gdev->index]));
}

static struct device_t *fb_gx6605s_probe(struct driver_t *drv, struct dtnode_t *node)
{
    struct gx6605s_device *gdev;
    struct gx6605s_video_mode *mode;
    struct framebuffer_t *fb;
    struct device_t *dev;
    unsigned int width, height, refresh;
    bool_t interlaced;

    gdev = malloc(sizeof(*gdev));
    if (!gdev)
        return NULL;

    fb = malloc(sizeof(*fb));
    if (!fb)
        goto error_fb;

    gdev->vram[0] = cache_to_dma((virtual_size_t)dma_alloc_noncoherent(1920 * 1080 * 4));
    if (!gdev->vram[0])
        goto error_vram0;

    gdev->vram[1] = cache_to_dma((virtual_size_t)dma_alloc_noncoherent(1920 * 1080 * 4));
    if (!gdev->vram[1])
        goto error_vram1;

    gdev->svpu_buff = cache_to_dma((virtual_size_t)dma_alloc_noncoherent(720 * 576 * 3));
    if (!gdev->svpu_buff)
        goto error_svpu;

    gdev->region = cache_to_dma((virtual_size_t)dma_alloc_noncoherent(SZ_4K));
    if (!gdev->region)
        goto error_region;

    gdev->nrl = region_list_alloc(0);
    if (!gdev->nrl)
        goto error_nrl;

    gdev->orl = region_list_alloc(0);
    if (!gdev->orl)
        goto error_orl;

    width = dt_read_int(node, "width", 1280);
    height = dt_read_int(node, "height", 720);
    refresh = dt_read_int(node, "refresh", 50);
    interlaced = dt_read_int(node, "interlaced", 1);

    gdev->base = phys_to_virt(dt_read_address(node));
    gdev->hdmi = gx6605s_mode_best(width, height, refresh, interlaced);
    gdev->cvbs = GX6605S_CVBS_DISABLE;
    gdev->bpp = 32;
    gdev->brightness = TRUE;

    mode = &gx6605s_video_modes[gdev->hdmi];
    width = mode->xres;
    height = mode->yres;

    fb->name = alloc_device_name(dt_read_name(node), -1);
    fb->width = width;
    fb->height = height;
    fb->pwidth = width;
    fb->pheight = height;
    fb->setbl = fb_gx6605s_setbl;
    fb->getbl = fb_gx6605s_getbl;
    fb->create = fb_gx6605s_create;
    fb->destroy = fb_gx6605s_destroy;
    fb->present = fb_gx6605s_present;
    fb->priv = gdev;

    gx6605s_vpu_hwinit(gdev);
    gx6605s_osd_hwinit(gdev);
    gx6605s_vpu_zoom(gdev, 1, width, height, width, height, 0, 0);
    gx6605s_osd_enable(gdev, TRUE);

    if (!(dev = register_framebuffer(fb, drv))) {
        free((void *)gdev->orl);
        goto error_orl;
    }

    return dev;

error_orl:
    free((void *)gdev->nrl);
error_nrl:
    dma_free_noncoherent((void *)gdev->region);
error_region:
    dma_free_noncoherent((void *)gdev->svpu_buff);
error_svpu:
    dma_free_noncoherent((void *)gdev->vram[1]);
error_vram1:
    dma_free_noncoherent((void *)gdev->vram[0]);
error_vram0:
    free(fb);
error_fb:
    free(gdev);
    return NULL;
}

static void fb_gx6605s_remove(struct device_t * dev)
{
    struct framebuffer_t *fb = dev->priv;
    struct gx6605s_device *gdev = fb->priv;

    if (fb) {
        unregister_framebuffer(fb);
        region_list_free(gdev->nrl);
        region_list_free(gdev->orl);
        dma_free_noncoherent((void *)gdev->region);
        dma_free_noncoherent((void *)gdev->vram[1]);
        dma_free_noncoherent((void *)gdev->vram[0]);
        free_device_name(fb->name);
        free(fb->priv);
        free(fb);
    }
}

static void fb_gx6605s_suspend(struct device_t * dev) {}
static void fb_gx6605s_resume(struct device_t * dev) {}

static struct driver_t fb_gx6605s = {
    .name = "fb-gx6605s",
    .probe = fb_gx6605s_probe,
    .remove = fb_gx6605s_remove,
    .suspend = fb_gx6605s_suspend,
    .resume = fb_gx6605s_resume,
};

static __init void fb_gx6605s_driver_init(void)
{
    register_driver(&fb_gx6605s);
}

static __exit void fb_gx6605s_driver_exit(void)
{
    unregister_driver(&fb_gx6605s);
}

driver_initcall(fb_gx6605s_driver_init);
driver_exitcall(fb_gx6605s_driver_exit);
