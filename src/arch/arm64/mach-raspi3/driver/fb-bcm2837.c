/*
 * driver/fb-bcm2837.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <framebuffer/framebuffer.h>
#include <bcm2837-mbox.h>

struct fb_bcm2837_pdata_t {
	int width;
	int height;
	int pwidth;
	int pheight;
	int bpp;
	int index;
	void * vram[2];
	int brightness;
};

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_bcm2837_pdata_t * pdat = (struct fb_bcm2837_pdata_t *)fb->priv;
	pdat->brightness = brightness;
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_bcm2837_pdata_t * pdat = (struct fb_bcm2837_pdata_t *)fb->priv;
	return pdat->brightness;
}

struct render_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_bcm2837_pdata_t * pdat = (struct fb_bcm2837_pdata_t *)fb->priv;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = pdat->width * pdat->height * (pdat->bpp / 8);
	pixels = memalign(4, pixlen);
	if(!pixels)
		return NULL;

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = pdat->width;
	render->height = pdat->height;
	render->pitch = (pdat->width * (pdat->bpp / 8) + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	return render;
}

void fb_destroy(struct framebuffer_t * fb, struct render_t * render)
{
	if(render)
	{
		free(render->pixels);
		free(render);
	}
}

void fb_present(struct framebuffer_t * fb, struct render_t * render)
{
	struct fb_bcm2837_pdata_t * pdat = (struct fb_bcm2837_pdata_t *)fb->priv;

	if(render && render->pixels)
	{
		pdat->index = (pdat->index + 1) & 0x1;
		memcpy(pdat->vram[pdat->index], render->pixels, render->pixlen);
		bcm2837_mbox_fb_present(0, pdat->index ? pdat->height : 0);
	}
}

static struct device_t * fb_bcm2837_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_bcm2837_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;

	pdat = malloc(sizeof(struct fb_bcm2837_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->width = dt_read_int(n, "width", 640);
	pdat->height = dt_read_int(n, "height", 480);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->bpp = dt_read_int(n, "bits-per-pixel", 32);
	pdat->index = 0;
	pdat->vram[0] = bcm2837_mbox_fb_alloc(pdat->width, pdat->height, pdat->bpp, 2);
	pdat->vram[1] = pdat->vram[0] + (pdat->width * pdat->height * (pdat->bpp / 8));
	pdat->brightness = 0;

	fb->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->pwidth = pdat->pwidth;
	fb->pheight = pdat->pheight;
	fb->bpp = pdat->bpp;
	fb->setbl = fb_setbl;
	fb->getbl = fb_getbl;
	fb->create = fb_create;
	fb->destroy = fb_destroy;
	fb->present = fb_present;
	fb->priv = pdat;

	if(!register_framebuffer(&dev, fb))
	{
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void fb_bcm2837_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;

	if(fb && unregister_framebuffer(fb))
	{
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_bcm2837_suspend(struct device_t * dev)
{
}

static void fb_bcm2837_resume(struct device_t * dev)
{
}

static struct driver_t fb_bcm2837 = {
	.name		= "fb-bcm2837",
	.probe		= fb_bcm2837_probe,
	.remove		= fb_bcm2837_remove,
	.suspend	= fb_bcm2837_suspend,
	.resume		= fb_bcm2837_resume,
};

static __init void fb_bcm2837_driver_init(void)
{
	register_driver(&fb_bcm2837);
}

static __exit void fb_bcm2837_driver_exit(void)
{
	unregister_driver(&fb_bcm2837);
}

//driver_initcall(fb_bcm2837_driver_init);
//driver_exitcall(fb_bcm2837_driver_exit);
