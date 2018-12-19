/*
 * driver/fb-pl111.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <dma/dma.h>
#include <led/led.h>
#include <framebuffer/framebuffer.h>

enum {
	CLCD_TIM0	= 0x000,
	CLCD_TIM1	= 0x004,
	CLCD_TIM2	= 0x008,
	CLCD_TIM3	= 0x00c,
	CLCD_UBAS	= 0x010,
	CLCD_LBAS	= 0x014,
	CLCD_CNTL	= 0x018,
	CLCD_IMSC	= 0x01c,
	CLCD_RIS	= 0x020,
	CLCD_MIS	= 0x024,
	CLCD_ICR	= 0x028,
	CLCD_UCUR	= 0x02c,
	CLCD_LCUR	= 0x030,
};

struct fb_pl111_pdata_t {
	virtual_addr_t virt;
	int width;
	int height;
	int pwidth;
	int pheight;
	int bpp;
	int hfp;
	int hbp;
	int hsl;
	int vfp;
	int vbp;
	int vsl;
	int index;
	void * vram[2];
	struct led_t * backlight;
	int brightness;
};

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_pl111_pdata_t * pdat = (struct fb_pl111_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_pl111_pdata_t * pdat = (struct fb_pl111_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

static struct render_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_pl111_pdata_t * pdat = (struct fb_pl111_pdata_t *)fb->priv;
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

static void fb_destroy(struct framebuffer_t * fb, struct render_t * render)
{
	if(render)
	{
		free(render->pixels);
		free(render);
	}
}

static void fb_present(struct framebuffer_t * fb, struct render_t * render, struct dirty_rect_t * rect, int nrect)
{
	struct fb_pl111_pdata_t * pdat = (struct fb_pl111_pdata_t *)fb->priv;

	if(render && render->pixels)
	{
		pdat->index = (pdat->index + 1) & 0x1;
		memcpy(pdat->vram[pdat->index], render->pixels, render->pixlen);
		dma_cache_sync(pdat->vram[pdat->index], render->pixlen, DMA_TO_DEVICE);
		write32(pdat->virt + CLCD_UBAS, ((u32_t)pdat->vram[pdat->index]));
		write32(pdat->virt + CLCD_LBAS, ((u32_t)pdat->vram[pdat->index] + pdat->width * pdat->height * (pdat->bpp / 8)));
	}
}

static struct device_t * fb_pl111_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_pl111_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	u32_t id = (((read32(virt + 0xfec) & 0xff) << 24) |
				((read32(virt + 0xfe8) & 0xff) << 16) |
				((read32(virt + 0xfe4) & 0xff) <<  8) |
				((read32(virt + 0xfe0) & 0xff) <<  0));

	if(((id >> 12) & 0xff) != 0x41 || (id & 0xfff) != 0x111)
		return NULL;

	pdat = malloc(sizeof(struct fb_pl111_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->width = dt_read_int(n, "width", 640);
	pdat->height = dt_read_int(n, "height", 480);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->bpp = dt_read_int(n, "bits-per-pixel", 32);
	pdat->hfp = dt_read_int(n, "hfront-porch", 1);
	pdat->hbp = dt_read_int(n, "hback-porch", 1);
	pdat->hsl = dt_read_int(n, "hsync-len", 1);
	pdat->vfp = dt_read_int(n, "vfront-porch", 1);
	pdat->vbp = dt_read_int(n, "vback-porch", 1);
	pdat->vsl = dt_read_int(n, "vsync-len", 1);
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bpp / 8);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bpp / 8);
	pdat->backlight = search_led(dt_read_string(n, "backlight", NULL));

	fb->name = alloc_device_name(dt_read_name(n), -1);
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

	write32(pdat->virt + CLCD_TIM0, (pdat->hbp<<24) | (pdat->hfp<<16) | (pdat->hsl<<8) | ((pdat->width/16-1)<<2));
	write32(pdat->virt + CLCD_TIM1, (pdat->vbp<<24) | (pdat->vfp<<16) | (pdat->vsl<<10) | ((pdat->height-1)<<0));
	write32(pdat->virt + CLCD_TIM2, (1<<26) | ((pdat->width/16-1)<<16) | (1<<5) | (1<<0));
	write32(pdat->virt + CLCD_TIM3, (0<<0));
	write32(pdat->virt + CLCD_IMSC, 0x0);
	write32(pdat->virt + CLCD_CNTL, (5 << 1) | (1 << 5) | (1 << 8));
	write32(pdat->virt + CLCD_CNTL, (read32(pdat->virt + CLCD_CNTL) | (1 << 0) | (1 << 11)));

	if(!register_framebuffer(&dev, fb))
	{
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void fb_pl111_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_pl111_pdata_t * pdat = (struct fb_pl111_pdata_t *)fb->priv;

	if(fb && unregister_framebuffer(fb))
	{
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_pl111_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_pl111_pdata_t * pdat = (struct fb_pl111_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_pl111_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_pl111_pdata_t * pdat = (struct fb_pl111_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_pl111 = {
	.name		= "fb-pl111",
	.probe		= fb_pl111_probe,
	.remove		= fb_pl111_remove,
	.suspend	= fb_pl111_suspend,
	.resume		= fb_pl111_resume,
};

static __init void fb_pl111_driver_init(void)
{
	register_driver(&fb_pl111);
}

static __exit void fb_pl111_driver_exit(void)
{
	unregister_driver(&fb_pl111);
}

driver_initcall(fb_pl111_driver_init);
driver_exitcall(fb_pl111_driver_exit);
