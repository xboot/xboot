/*
 * driver/fb-f1c200s.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <gpio/gpio.h>
#include <led/led.h>
#include <framebuffer/framebuffer.h>
#include <f1c200s-gpio.h>
#include <f1c200s/reg-tcon.h>
#include <f1c200s/reg-defe.h>
#include <f1c200s/reg-debe.h>

#define F1C200S_GPIO_BASE		(0x01c20800)
#define F1C200S_GPIOD_CFG0		((3 * 0x24) + 0x00)
#define F1C200S_GPIOD_DATA		((3 * 0x24) + 0x10)

struct fb_f1c200s_pdata_t
{
	virtual_addr_t virtdefe;
	virtual_addr_t virtdebe;
	virtual_addr_t virttcon;
	virtual_addr_t virtgpio;

	char * clkdefe;
	char * clkdebe;
	char * clktcon;
	int rstdefe;
	int rstdebe;
	int rsttcon;
	int width;
	int height;
	int pwidth;
	int pheight;
	int bits_per_pixel;
	int bytes_per_pixel;
	int pixlen;
	int index;
	void * vram[2];
	struct region_list_t * nrl, * orl;

	struct {
		int pixel_clock_hz;
		int h_front_porch;
		int h_back_porch;
		int h_sync_len;
		int v_front_porch;
		int v_back_porch;
		int v_sync_len;
		int h_sync_active;
		int v_sync_active;
		int den_active;
		int clk_active;
	} timing;

	struct led_t * backlight;
	int brightness;
};

static inline void spi_9bits_write(struct fb_f1c200s_pdata_t * pdat, uint32_t val)
{
	uint8_t cnt = 0;
	uint32_t tmp = read32(pdat->virtgpio + F1C200S_GPIOD_DATA);

	tmp &= ~(1 << 11);
	write32(pdat->virtgpio + F1C200S_GPIOD_DATA, tmp);
	for(cnt = 0; cnt < 9; cnt++)
	{
		tmp &= ~(1 << 10);
		if(val & 0x100)
		{
			tmp |= (1 << 10);
		}
		val <<= 1;
		tmp &= ~(1 << 9);
		write32(pdat->virtgpio + F1C200S_GPIOD_DATA, tmp);
		tmp |= (1 << 9);
		write32(pdat->virtgpio + F1C200S_GPIOD_DATA, tmp);
	}
	tmp |= (1 << 11);
	write32(pdat->virtgpio + F1C200S_GPIOD_DATA, tmp);
}

static inline void gc9308_init(struct fb_f1c200s_pdata_t * pdat)
{
	spi_9bits_write(pdat, 0x00fe);
	spi_9bits_write(pdat, 0x00ef);
	spi_9bits_write(pdat, 0x0036);
	spi_9bits_write(pdat, 0x0140);
	spi_9bits_write(pdat, 0x003a);
	spi_9bits_write(pdat, 0x0155);
	spi_9bits_write(pdat, 0x0084);
	spi_9bits_write(pdat, 0x0104);
	spi_9bits_write(pdat, 0x0086);
	spi_9bits_write(pdat, 0x01fb);
	spi_9bits_write(pdat, 0x0087);
	spi_9bits_write(pdat, 0x0179);
	spi_9bits_write(pdat, 0x0089);
	spi_9bits_write(pdat, 0x010b);
	spi_9bits_write(pdat, 0x008a);
	spi_9bits_write(pdat, 0x0120);
	spi_9bits_write(pdat, 0x008b);
	spi_9bits_write(pdat, 0x0180);
	spi_9bits_write(pdat, 0x008d);
	spi_9bits_write(pdat, 0x013b);
	spi_9bits_write(pdat, 0x008e);
	spi_9bits_write(pdat, 0x01cf);
	spi_9bits_write(pdat, 0x00ec);
	spi_9bits_write(pdat, 0x0133);
	spi_9bits_write(pdat, 0x0102);
	spi_9bits_write(pdat, 0x014c);
	spi_9bits_write(pdat, 0x0098);
	spi_9bits_write(pdat, 0x013e);
	spi_9bits_write(pdat, 0x009c);
	spi_9bits_write(pdat, 0x014b);
	spi_9bits_write(pdat, 0x0099);
	spi_9bits_write(pdat, 0x013e);
	spi_9bits_write(pdat, 0x009d);
	spi_9bits_write(pdat, 0x014b);
	spi_9bits_write(pdat, 0x009b);
	spi_9bits_write(pdat, 0x0155);
	spi_9bits_write(pdat, 0x00e8);
	spi_9bits_write(pdat, 0x0111);
	spi_9bits_write(pdat, 0x0100);
	spi_9bits_write(pdat, 0x00ff);
	spi_9bits_write(pdat, 0x0162);
	spi_9bits_write(pdat, 0x00c3);
	spi_9bits_write(pdat, 0x0120);
	spi_9bits_write(pdat, 0x00c4);
	spi_9bits_write(pdat, 0x0103);
	spi_9bits_write(pdat, 0x00c9);
	spi_9bits_write(pdat, 0x010a);
	spi_9bits_write(pdat, 0x003a);
	spi_9bits_write(pdat, 0x0155);
	spi_9bits_write(pdat, 0x0084);
	spi_9bits_write(pdat, 0x0161);
	spi_9bits_write(pdat, 0x008a);
	spi_9bits_write(pdat, 0x0140);
	spi_9bits_write(pdat, 0x00f6);
	spi_9bits_write(pdat, 0x01c7);
	spi_9bits_write(pdat, 0x00b0);
	spi_9bits_write(pdat, 0x0163);
	spi_9bits_write(pdat, 0x00b5);
	spi_9bits_write(pdat, 0x0102);
	spi_9bits_write(pdat, 0x0102);
	spi_9bits_write(pdat, 0x0114);
	spi_9bits_write(pdat, 0x00f0);
	spi_9bits_write(pdat, 0x014a);
	spi_9bits_write(pdat, 0x0110);
	spi_9bits_write(pdat, 0x010a);
	spi_9bits_write(pdat, 0x010a);
	spi_9bits_write(pdat, 0x0126);
	spi_9bits_write(pdat, 0x0139);
	spi_9bits_write(pdat, 0x00f2);
	spi_9bits_write(pdat, 0x014a);
	spi_9bits_write(pdat, 0x0110);
	spi_9bits_write(pdat, 0x010a);
	spi_9bits_write(pdat, 0x010a);
	spi_9bits_write(pdat, 0x0126);
	spi_9bits_write(pdat, 0x0139);
	spi_9bits_write(pdat, 0x00f1);
	spi_9bits_write(pdat, 0x0150);
	spi_9bits_write(pdat, 0x018f);
	spi_9bits_write(pdat, 0x01af);
	spi_9bits_write(pdat, 0x013b);
	spi_9bits_write(pdat, 0x013f);
	spi_9bits_write(pdat, 0x017f);
	spi_9bits_write(pdat, 0x00f3);
	spi_9bits_write(pdat, 0x0150);
	spi_9bits_write(pdat, 0x018f);
	spi_9bits_write(pdat, 0x01af);
	spi_9bits_write(pdat, 0x013b);
	spi_9bits_write(pdat, 0x013f);
	spi_9bits_write(pdat, 0x017f);
	spi_9bits_write(pdat, 0x00ba);
	spi_9bits_write(pdat, 0x010a);
	spi_9bits_write(pdat, 0x0035);
	spi_9bits_write(pdat, 0x0100);
	spi_9bits_write(pdat, 0x0021);
	spi_9bits_write(pdat, 0x00fe);
	spi_9bits_write(pdat, 0x00ee);
	spi_9bits_write(pdat, 0x0011);
	spi_9bits_write(pdat, 0x0029);
	spi_9bits_write(pdat, 0x002c);
}

static inline void f1c200s_debe_set_mode(struct fb_f1c200s_pdata_t * pdat)
{
	struct f1c200s_debe_reg_t * debe = (struct f1c200s_debe_reg_t *)(pdat->virtdebe);
	u32_t val;

	val = read32((virtual_addr_t)&debe->mode);
	val |= (1 << 0);
	write32((virtual_addr_t)&debe->mode, val);

	write32((virtual_addr_t)&debe->disp_size, (((pdat->height) - 1) << 16) | (((pdat->width) - 1) << 0));
	write32((virtual_addr_t)&debe->layer0_size, (((pdat->height) - 1) << 16) | (((pdat->width) - 1) << 0));
	write32((virtual_addr_t)&debe->layer0_stride, ((pdat->width) << 5));
	write32((virtual_addr_t)&debe->layer0_addr_low32b, (u32_t)(pdat->vram[pdat->index]) << 3);
	write32((virtual_addr_t)&debe->layer0_addr_high4b, (u32_t)(pdat->vram[pdat->index]) >> 29);
	write32((virtual_addr_t)&debe->layer0_attr1_ctrl, 0x09 << 8);

	val = read32((virtual_addr_t)&debe->mode);
	val |= (1 << 8);
	write32((virtual_addr_t)&debe->mode, val);

	val = read32((virtual_addr_t)&debe->reg_ctrl);
	val |= (1 << 0);
	write32((virtual_addr_t)&debe->reg_ctrl, val);

	val = read32((virtual_addr_t)&debe->mode);
	val |= (1 << 1);
	write32((virtual_addr_t)&debe->mode, val);
}

static inline void f1c200s_debe_set_address(struct fb_f1c200s_pdata_t * pdat, void * vram)
{
	struct f1c200s_debe_reg_t * debe = (struct f1c200s_debe_reg_t *)(pdat->virtdebe);

	write32((virtual_addr_t)&debe->layer0_addr_low32b, (u32_t)vram << 3);
	write32((virtual_addr_t)&debe->layer0_addr_high4b, (u32_t)vram >> 29);
}

static inline void f1c200s_tcon_enable(struct fb_f1c200s_pdata_t * pdat)
{
	struct f1c200s_tcon_reg_t * tcon = (struct f1c200s_tcon_reg_t *)pdat->virttcon;
	u32_t val;

	val = read32((virtual_addr_t)&tcon->ctrl);
	val |= (1 << 31);
	write32((virtual_addr_t)&tcon->ctrl, val);
}

static inline void f1c200s_tcon_disable(struct fb_f1c200s_pdata_t * pdat)
{
	struct f1c200s_tcon_reg_t * tcon = (struct f1c200s_tcon_reg_t *)pdat->virttcon;
	u32_t val;

	write32((virtual_addr_t)&tcon->ctrl, 0);
	write32((virtual_addr_t)&tcon->int0, 0);

	val = read32((virtual_addr_t)&tcon->tcon0_dclk);
	val &= ~(0xf << 28);
	write32((virtual_addr_t)&tcon->tcon0_dclk, val);

	write32((virtual_addr_t)&tcon->tcon0_io_tristate, 0xffffffff);
	write32((virtual_addr_t)&tcon->tcon1_io_tristate, 0xffffffff);
}

static inline void f1c200s_tcon_set_mode(struct fb_f1c200s_pdata_t * pdat)
{
	struct f1c200s_tcon_reg_t * tcon = (struct f1c200s_tcon_reg_t *)pdat->virttcon;
	int bp, total;
	u32_t val;

	val = read32((virtual_addr_t)&tcon->ctrl);
	val &= ~(0x1 << 0);
	write32((virtual_addr_t)&tcon->ctrl, val);

	val = (pdat->timing.v_front_porch + pdat->timing.v_back_porch + pdat->timing.v_sync_len);
	write32((virtual_addr_t)&tcon->tcon0_ctrl, (1 << 31) | ((val & 0x1f) << 4));
	val = clk_get_rate(pdat->clktcon) / pdat->timing.pixel_clock_hz;
	write32((virtual_addr_t)&tcon->tcon0_dclk, (0xf << 28) | (val << 0));
	write32((virtual_addr_t)&tcon->tcon0_timing_active, ((pdat->width - 1) << 16) | ((pdat->height - 1) << 0));

	bp = pdat->timing.h_sync_len + pdat->timing.h_back_porch;
	total = pdat->width * 3 + pdat->timing.h_front_porch + bp;
	write32((virtual_addr_t)&tcon->tcon0_timing_h, ((total - 1) << 16) | ((bp - 1) << 0));
	bp = pdat->timing.v_sync_len + pdat->timing.v_back_porch;
	total = pdat->height + pdat->timing.v_front_porch + bp;
	write32((virtual_addr_t)&tcon->tcon0_timing_v, ((total * 2) << 16) | ((bp - 1) << 0));
	write32((virtual_addr_t)&tcon->tcon0_timing_sync, ((pdat->timing.h_sync_len - 1) << 16) | ((pdat->timing.v_sync_len - 1) << 0));

	write32((virtual_addr_t)&tcon->tcon0_hv_intf, (1 << 31));
	write32((virtual_addr_t)&tcon->tcon0_cpu_intf, 0);

	write32((virtual_addr_t)&tcon->tcon0_io_polarity, (1 << 28));
	write32((virtual_addr_t)&tcon->tcon0_io_tristate, 0);
}

static inline void fb_f1c200s_cfg_gpios(int base, int n, int cfg, enum gpio_pull_t pull, enum gpio_drv_t drv)
{
	for(; n > 0; n--, base++)
	{
		gpio_set_cfg(base, cfg);
		gpio_set_pull(base, pull);
		gpio_set_drv(base, drv);
	}
}

static inline void fb_f1c200s_init(struct fb_f1c200s_pdata_t * pdat)
{
	fb_f1c200s_cfg_gpios(F1C200S_GPIOD9, 3, 1, GPIO_PULL_NONE, GPIO_DRV_STRONG);
	write32(pdat->virtgpio + F1C200S_GPIOD_DATA, 0xffffffff);
	gc9308_init(pdat);

	fb_f1c200s_cfg_gpios(F1C200S_GPIOD3, 6, 2, GPIO_PULL_NONE, GPIO_DRV_STRONG);
	fb_f1c200s_cfg_gpios(F1C200S_GPIOD18, 1, 2, GPIO_PULL_NONE, GPIO_DRV_STRONG);
	fb_f1c200s_cfg_gpios(F1C200S_GPIOD20, 2, 2, GPIO_PULL_NONE, GPIO_DRV_STRONG);
	f1c200s_tcon_disable(pdat);
	f1c200s_debe_set_mode(pdat);
	f1c200s_tcon_set_mode(pdat);
	f1c200s_tcon_enable(pdat);
}

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_f1c200s_pdata_t * pdat = (struct fb_f1c200s_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_f1c200s_pdata_t * pdat = (struct fb_f1c200s_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

static struct surface_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_f1c200s_pdata_t * pdat = (struct fb_f1c200s_pdata_t *)fb->priv;
	return surface_alloc(pdat->width, pdat->height, NULL);
}

static void fb_destroy(struct framebuffer_t * fb, struct surface_t * s)
{
	surface_free(s);
}

static void fb_present(struct framebuffer_t * fb, struct surface_t * s, struct region_list_t * rl)
{
	struct fb_f1c200s_pdata_t * pdat = (struct fb_f1c200s_pdata_t *)fb->priv;
	struct region_list_t * nrl = pdat->nrl;

	region_list_clear(nrl);
	region_list_merge(nrl, pdat->orl);
	region_list_merge(nrl, rl);
	region_list_clone(pdat->orl, rl);

	pdat->index = (pdat->index + 1) & 0x1;
	if(nrl->count > 0)
		present_surface(pdat->vram[pdat->index], s, nrl);
	else
		memcpy(pdat->vram[pdat->index], s->pixels, s->pixlen);
	dma_cache_sync(pdat->vram[pdat->index], pdat->pixlen, DMA_TO_DEVICE);
	f1c200s_debe_set_address(pdat, pdat->vram[pdat->index]);
}

static struct device_t * fb_f1c200s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_f1c200s_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	char * clkdefe = dt_read_string(n, "clock-name-defe", NULL);
	char * clkdebe = dt_read_string(n, "clock-name-debe", NULL);
	char * clktcon = dt_read_string(n, "clock-name-tcon", NULL);
	int i;

	if(!search_clk(clkdefe) || !search_clk(clkdebe) || !search_clk(clktcon))
		return NULL;

	pdat = malloc(sizeof(struct fb_f1c200s_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->virtdefe = phys_to_virt(F1C200S_DEFE_BASE);
	pdat->virtdebe = phys_to_virt(F1C200S_DEBE_BASE);
	pdat->virttcon = phys_to_virt(F1C200S_TCON_BASE);
	pdat->virtgpio = phys_to_virt(F1C200S_GPIO_BASE);
	pdat->clkdefe = strdup(clkdefe);
	pdat->clkdebe = strdup(clkdebe);
	pdat->clktcon = strdup(clktcon);
	pdat->rstdefe = dt_read_int(n, "reset-defe", -1);
	pdat->rstdebe = dt_read_int(n, "reset-debe", -1);
	pdat->rsttcon = dt_read_int(n, "reset-tcon", -1);
	pdat->width = dt_read_int(n, "width", 800);
	pdat->height = dt_read_int(n, "height", 480);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->bits_per_pixel = 18;
	pdat->bytes_per_pixel = 4;
	pdat->pixlen = pdat->width * pdat->height * pdat->bytes_per_pixel;
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->pixlen);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->pixlen);
	pdat->nrl = region_list_alloc(0);
	pdat->orl = region_list_alloc(0);

	pdat->timing.pixel_clock_hz = dt_read_long(n, "clock-frequency", 33000000);
	pdat->timing.h_front_porch = dt_read_int(n, "hfront-porch", 40);
	pdat->timing.h_back_porch = dt_read_int(n, "hback-porch", 87);
	pdat->timing.h_sync_len = dt_read_int(n, "hsync-len", 1);
	pdat->timing.v_front_porch = dt_read_int(n, "vfront-porch", 13);
	pdat->timing.v_back_porch = dt_read_int(n, "vback-porch", 31);
	pdat->timing.v_sync_len = dt_read_int(n, "vsync-len", 1);
	pdat->timing.h_sync_active = dt_read_bool(n, "hsync-active", 0);
	pdat->timing.v_sync_active = dt_read_bool(n, "vsync-active", 0);
	pdat->timing.den_active = dt_read_bool(n, "den-active", 0);
	pdat->timing.clk_active = dt_read_bool(n, "clk-active", 0);
	pdat->backlight = search_led(dt_read_string(n, "backlight", NULL));

	fb->name = alloc_device_name(dt_read_name(n), -1);
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->pwidth = pdat->pwidth;
	fb->pheight = pdat->pheight;
	fb->setbl = fb_setbl;
	fb->getbl = fb_getbl;
	fb->create = fb_create;
	fb->destroy = fb_destroy;
	fb->present = fb_present;
	fb->priv = pdat;

	clk_enable(pdat->clkdefe);
	clk_enable(pdat->clkdebe);
	clk_enable(pdat->clktcon);
	if(pdat->rstdefe >= 0)
		reset_deassert(pdat->rstdefe);
	if(pdat->rstdebe >= 0)
		reset_deassert(pdat->rstdebe);
	if(pdat->rsttcon >= 0)
		reset_deassert(pdat->rsttcon);
	for(i = 0x0800; i < 0x1000; i += 4)
		write32(pdat->virtdebe + i, 0);
	fb_f1c200s_init(pdat);

	if(!(dev = register_framebuffer(fb, drv)))
	{
		clk_disable(pdat->clkdefe);
		clk_disable(pdat->clkdebe);
		clk_disable(pdat->clktcon);
		free(pdat->clkdefe);
		free(pdat->clkdebe);
		free(pdat->clktcon);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);
		region_list_free(pdat->nrl);
		region_list_free(pdat->orl);
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	return dev;
}

static void fb_f1c200s_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_f1c200s_pdata_t * pdat = (struct fb_f1c200s_pdata_t *)fb->priv;

	if(fb)
	{
		unregister_framebuffer(fb);
		clk_disable(pdat->clkdefe);
		clk_disable(pdat->clkdebe);
		clk_disable(pdat->clktcon);
		free(pdat->clkdefe);
		free(pdat->clkdebe);
		free(pdat->clktcon);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);
		region_list_free(pdat->nrl);
		region_list_free(pdat->orl);
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_f1c200s_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_f1c200s_pdata_t * pdat = (struct fb_f1c200s_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_f1c200s_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_f1c200s_pdata_t * pdat = (struct fb_f1c200s_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_f1c200s = {
	.name		= "fb-f1c200s",
	.probe		= fb_f1c200s_probe,
	.remove		= fb_f1c200s_remove,
	.suspend	= fb_f1c200s_suspend,
	.resume		= fb_f1c200s_resume,
};

static __init void fb_f1c200s_driver_init(void)
{
	register_driver(&fb_f1c200s);
}

static __exit void fb_f1c200s_driver_exit(void)
{
	unregister_driver(&fb_f1c200s);
}

driver_initcall(fb_f1c200s_driver_init);
driver_exitcall(fb_f1c200s_driver_exit);
