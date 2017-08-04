/*
 * driver/fb-v3s.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <fb/fb.h>
#include <dma/dma.h>
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <led/led.h>
#include <v3s/reg-de.h>
#include <v3s/reg-tcon.h>
#include <v3s-gpio.h>

struct fb_v3s_pdata_t
{
	virtual_addr_t virtde;
	virtual_addr_t virttcon;

	char * clk;
	int width;
	int height;
	int xdpi;
	int ydpi;
	int bits_per_pixel;
	int bytes_per_pixel;
	int index;
	void * vram[2];

	struct {
		int pixel_clock_hz;
		int	h_front_porch;
		int	h_back_porch;
		int	h_sync_len;
		int	v_front_porch;
		int	v_back_porch;
		int	v_sync_len;
		int	h_sync_active;
		int	v_sync_active;
		int den_active;
		int clk_active;
	} timing;

	struct led_t * backlight;
	int brightness;
};

static void v3s_fb_init(struct fb_v3s_pdata_t * pdat)
{
}

static void fb_setbl(struct fb_t * fb, int brightness)
{
	struct fb_v3s_pdata_t * pdat = (struct fb_v3s_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct fb_t * fb)
{
	struct fb_v3s_pdata_t * pdat = (struct fb_v3s_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct fb_v3s_pdata_t * pdat = (struct fb_v3s_pdata_t *)fb->priv;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = pdat->width * pdat->height * pdat->bytes_per_pixel;
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
	render->pitch = (pdat->width * pdat->bytes_per_pixel + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	if(render)
	{
		free(render->pixels);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct fb_v3s_pdata_t * pdat = (struct fb_v3s_pdata_t *)fb->priv;

	if(render && render->pixels)
	{
		pdat->index = (pdat->index + 1) & 0x1;
		memcpy(pdat->vram[pdat->index], render->pixels, render->pixlen);
		dma_cache_sync(pdat->vram[pdat->index], render->pixlen, DMA_TO_DEVICE);
	}
}

static struct device_t * fb_v3s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_v3s_pdata_t * pdat;
	struct fb_t * fb;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct fb_v3s_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct fb_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->virtde = virt;
	pdat->virttcon = phys_to_virt(V3S_TCON_BASE);
	pdat->clk = strdup(clk);
	pdat->width = dt_read_int(n, "width", 800);
	pdat->height = dt_read_int(n, "height", 400);
	pdat->xdpi = dt_read_int(n, "dots-per-inch-x", 160);
	pdat->ydpi = dt_read_int(n, "dots-per-inch-y", 160);
	pdat->bits_per_pixel = dt_read_int(n, "bits-per-pixel", 32);
	pdat->bytes_per_pixel = dt_read_int(n, "bytes-per-pixel", 4);
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes_per_pixel);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes_per_pixel);

	pdat->timing.pixel_clock_hz = dt_read_long(n, "clock-frequency", 33000000);
	pdat->timing.h_front_porch = dt_read_int(n, "hfront-porch", 87);
	pdat->timing.h_back_porch = dt_read_int(n, "hback-porch", 40);
	pdat->timing.h_sync_len = dt_read_int(n, "hsync-len", 1);
	pdat->timing.v_front_porch = dt_read_int(n, "vfront-porch", 31);
	pdat->timing.v_back_porch = dt_read_int(n, "vback-porch", 13);
	pdat->timing.v_sync_len = dt_read_int(n, "vsync-len", 1);
	pdat->timing.h_sync_active = dt_read_bool(n, "hsync-active", 0);
	pdat->timing.v_sync_active = dt_read_bool(n, "vsync-active", 0);
	pdat->timing.den_active = dt_read_bool(n, "den-active", 0);
	pdat->timing.clk_active = dt_read_bool(n, "clk-active", 0);
	pdat->backlight = search_led(dt_read_string(n, "backlight", NULL));

	fb->name = alloc_device_name(dt_read_name(n), -1);
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->xdpi = pdat->xdpi;
	fb->ydpi = pdat->ydpi;
	fb->bpp = pdat->bits_per_pixel;
	fb->setbl = fb_setbl,
	fb->getbl = fb_getbl,
	fb->create = fb_create,
	fb->destroy = fb_destroy,
	fb->present = fb_present,
	fb->priv = pdat;

	clk_enable(pdat->clk);
	v3s_fb_init(pdat);

	if(!register_fb(&dev, fb))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
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

static void fb_v3s_remove(struct device_t * dev)
{
	struct fb_t * fb = (struct fb_t *)dev->priv;
	struct fb_v3s_pdata_t * pdat = (struct fb_v3s_pdata_t *)fb->priv;

	if(fb && unregister_fb(fb))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_v3s_suspend(struct device_t * dev)
{
	struct fb_t * fb = (struct fb_t *)dev->priv;
	struct fb_v3s_pdata_t * pdat = (struct fb_v3s_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_v3s_resume(struct device_t * dev)
{
	struct fb_t * fb = (struct fb_t *)dev->priv;
	struct fb_v3s_pdata_t * pdat = (struct fb_v3s_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_v3s = {
	.name		= "fb-v3s",
	.probe		= fb_v3s_probe,
	.remove		= fb_v3s_remove,
	.suspend	= fb_v3s_suspend,
	.resume		= fb_v3s_resume,
};

static __init void fb_v3s_driver_init(void)
{
	register_driver(&fb_v3s);
}

static __exit void fb_v3s_driver_exit(void)
{
	unregister_driver(&fb_v3s);
}

driver_initcall(fb_v3s_driver_init);
driver_exitcall(fb_v3s_driver_exit);
