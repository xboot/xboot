/*
 * driver/fb-s5l8930.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
	LCD_ADDR	= 0x044,
	LCD_SIZE	= 0x060,
};

struct fb_s5l8930_pdata_t {
	virtual_addr_t virt;
	int width;
	int height;
	int pwidth;
	int pheight;
	int bytes;
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
	struct fb_s5l8930_pdata_t * pdat = (struct fb_s5l8930_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_s5l8930_pdata_t * pdat = (struct fb_s5l8930_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

static struct render_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_s5l8930_pdata_t * pdat = (struct fb_s5l8930_pdata_t *)fb->priv;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = pdat->width * pdat->height * pdat->bytes;
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
	render->pitch = (pdat->width * pdat->bytes + 0x3) & ~0x3;
	render->bytes = pdat->bytes;
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

static void fb_present(struct framebuffer_t * fb, struct render_t * render, struct region_list_t * rl)
{
	struct fb_s5l8930_pdata_t * pdat = (struct fb_s5l8930_pdata_t *)fb->priv;

	if(render && render->pixels)
	{
		pdat->index = (pdat->index + 1) & 0x1;
		memcpy(pdat->vram[pdat->index], render->pixels, render->pixlen);
		dma_cache_sync(pdat->vram[pdat->index], render->pixlen, DMA_TO_DEVICE);
		write32(pdat->virt + LCD_ADDR, ((u32_t)pdat->vram[pdat->index]));
	}
}

static struct device_t * fb_s5l8930_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_s5l8930_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));

	pdat = malloc(sizeof(struct fb_s5l8930_pdata_t));
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
	pdat->height = dt_read_int(n, "height", 960);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->bytes = dt_read_int(n, "bytes-per-pixel", 4);
	pdat->hfp = dt_read_int(n, "hfront-porch", 1);
	pdat->hbp = dt_read_int(n, "hback-porch", 1);
	pdat->hsl = dt_read_int(n, "hsync-len", 1);
	pdat->vfp = dt_read_int(n, "vfront-porch", 1);
	pdat->vbp = dt_read_int(n, "vback-porch", 1);
	pdat->vsl = dt_read_int(n, "vsync-len", 1);
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes);
	pdat->backlight = search_led(dt_read_string(n, "backlight", NULL));

	fb->name = alloc_device_name(dt_read_name(n), -1);
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->pwidth = pdat->pwidth;
	fb->pheight = pdat->pheight;
	fb->bytes = pdat->bytes;
	fb->setbl = fb_setbl;
	fb->getbl = fb_getbl;
	fb->create = fb_create;
	fb->destroy = fb_destroy;
	fb->present = fb_present;
	fb->priv = pdat;

	write32(pdat->virt + LCD_SIZE, (pdat->width << 16) | (pdat->height << 0));

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

static void fb_s5l8930_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_s5l8930_pdata_t * pdat = (struct fb_s5l8930_pdata_t *)fb->priv;

	if(fb && unregister_framebuffer(fb))
	{
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_s5l8930_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_s5l8930_pdata_t * pdat = (struct fb_s5l8930_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_s5l8930_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_s5l8930_pdata_t * pdat = (struct fb_s5l8930_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_s5l8930 = {
	.name		= "fb-s5l8930",
	.probe		= fb_s5l8930_probe,
	.remove		= fb_s5l8930_remove,
	.suspend	= fb_s5l8930_suspend,
	.resume		= fb_s5l8930_resume,
};

static __init void fb_s5l8930_driver_init(void)
{
	register_driver(&fb_s5l8930);
}

static __exit void fb_s5l8930_driver_exit(void)
{
	unregister_driver(&fb_s5l8930);
}

driver_initcall(fb_s5l8930_driver_init);
driver_exitcall(fb_s5l8930_driver_exit);
