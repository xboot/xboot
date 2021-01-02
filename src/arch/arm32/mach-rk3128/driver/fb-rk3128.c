/*
 * driver/fb-rk3128.c
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
#include <gpio/gpio.h>
#include <led/led.h>
#include <regulator/regulator.h>
#include <framebuffer/framebuffer.h>
#include <rk3128/reg-grf.h>
#include <rk3128/reg-lcd.h>
#include <rk3128-gpio.h>

struct fb_rk3128_pdata_t
{
	virtual_addr_t virtlcd;
	virtual_addr_t virtgrf;

	char * regulator;
	char * clk;
	int lcdrst;
	int lcdrstcfg;
	int lcden;
	int lcdencfg;
	int width;
	int height;
	int pwidth;
	int pheight;
	int bits_per_pixel;
	int pixlen;
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

static inline void rk3128_lcd_update_config(struct fb_rk3128_pdata_t * pdat)
{
	write32(pdat->virtlcd + LCD_REG_CFG_DONE, 0x01);
}

static inline void rk3128_lcd_set_timing(struct fb_rk3128_pdata_t * pdat)
{
	u32_t cfg;

	cfg = (pdat->timing.h_sync_len & 0xfff) << 0;
	cfg |= ((pdat->timing.h_sync_len + pdat->timing.h_back_porch + pdat->width + pdat->timing.h_front_porch) & 0xfff) << 16;
	write32(pdat->virtlcd + LCD_DSP_HTOTAL_HS_END, cfg);

	cfg = ((pdat->timing.h_sync_len + pdat->timing.h_back_porch + pdat->width) & 0xfff) << 0;
	cfg |= ((pdat->timing.h_sync_len + pdat->timing.h_back_porch) & 0xfff) << 16;
	write32(pdat->virtlcd + LCD_DSP_HACT_ST_END, cfg);

	cfg = (pdat->timing.v_sync_len & 0xfff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch + pdat->height + pdat->timing.v_front_porch) & 0xfff) << 16;
	write32(pdat->virtlcd + LCD_DSP_VTOTAL_VS_END, cfg);

	cfg = ((pdat->timing.v_sync_len + pdat->timing.v_back_porch + pdat->height) & 0xfff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch) & 0xfff) << 16;
	write32(pdat->virtlcd + LCD_DSP_VACT_ST_END, cfg);

	cfg = ((pdat->timing.h_sync_len + pdat->timing.h_back_porch + pdat->width) & 0xfff) << 0;
	cfg |= ((pdat->timing.h_sync_len + pdat->timing.h_back_porch) & 0xfff) << 16;
	write32(pdat->virtlcd + LCD_DSP_VS_ST_END_F1, cfg);

	cfg = ((pdat->timing.v_sync_len + pdat->timing.v_back_porch + pdat->height) & 0xfff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch) & 0xfff) << 16;
	write32(pdat->virtlcd + LCD_DSP_VACT_ST_END_F1, cfg);
}

static inline void rk3128_lcd_set_polarity(struct fb_rk3128_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtlcd + LCD_DSP_CTRL0);
	cfg &= ~(0xf << 4);
	if(pdat->timing.h_sync_active)
		cfg |= (0x1 << 4);
	if(pdat->timing.v_sync_active)
		cfg |= (0x1 << 5);
	if(pdat->timing.den_active)
		cfg |= (0x1 << 6);
	if(pdat->timing.clk_active)
		cfg |= (0x1 << 7);
	write32(pdat->virtlcd + LCD_DSP_CTRL0, cfg);
}

static inline void rk3128_lcd_set_win0(struct fb_rk3128_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtlcd + LCD_SYS_CTRL);
	cfg &= ~((7 << 3) | (1 << 0));
	cfg |= (0 << 3) | (1 << 0);
	write32(pdat->virtlcd + LCD_SYS_CTRL, cfg);

	cfg = ((((pdat->width - 1) * 0x1000) / (pdat->width - 1)) & 0xffff) << 0;
	cfg |= ((((pdat->height - 1) * 0x1000) / (pdat->height - 1)) & 0xffff) << 16;
	write32(pdat->virtlcd + LCD_WIN0_SCL_FACTOR_YRGB, cfg);

	cfg = ((pdat->width - 1) & 0x1fff) << 0;
	cfg |= ((pdat->height - 1) & 0x1fff) << 16;
	write32(pdat->virtlcd + LCD_WIN0_ACT_INFO, cfg);

	cfg = ((pdat->width - 1) & 0x7ff) << 0;
	cfg |= ((pdat->height - 1) & 0x7ff) << 16;
	write32(pdat->virtlcd + LCD_WIN0_DSP_INFO, cfg);

	cfg = ((pdat->timing.h_sync_len + pdat->timing.h_back_porch) & 0xfff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch) & 0xfff) << 16;
	write32(pdat->virtlcd + LCD_WIN0_DSP_ST, cfg);

	write32(pdat->virtlcd + LCD_WIN0_COLOR_KEY, 0);
	switch(pdat->bits_per_pixel)
	{
	case 16:
		write32(pdat->virtlcd + LCD_WIN0_VIR, (((pdat->width / 2) & 0x1fff) << 0));
		break;
	case 24:
		write32(pdat->virtlcd + LCD_WIN0_VIR, (((((pdat->width * 3) >> 2)+((pdat->width) % 3)) & 0x1fff) << 0));
		break;
	case 32:
		write32(pdat->virtlcd + LCD_WIN0_VIR, (((pdat->width) & 0x1fff) << 0));
		break;
	default:
		write32(pdat->virtlcd + LCD_WIN0_VIR, (((pdat->width) & 0x1fff) << 0));
		break;
	}
	rk3128_lcd_update_config(pdat);
}

static inline void rk3128_lcd_set_win0_address(struct fb_rk3128_pdata_t * pdat, void * vram)
{
	write32(pdat->virtlcd + LCD_WIN0_YRGB_MST, (u32_t)vram);
}

static inline void rk3128_select_mipi(struct fb_rk3128_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtlcd + LCD_AXI_BUS_CTRL);
	cfg &= ~(1 << 28);
	cfg |= (1 << 28);
	write32(pdat->virtlcd + LCD_SYS_CTRL, cfg);
}

static void rk3128_fb_init(struct fb_rk3128_pdata_t * pdat)
{
	rk3128_lcd_set_timing(pdat);
	rk3128_lcd_set_polarity(pdat);
	rk3128_lcd_update_config(pdat);

	rk3128_lcd_set_win0(pdat);
	rk3128_lcd_set_win0_address(pdat, pdat->vram[0]);
	rk3128_lcd_update_config(pdat);

	rk3128_select_mipi(pdat);
	rk3128_lcd_update_config(pdat);
}

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_rk3128_pdata_t * pdat = (struct fb_rk3128_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_rk3128_pdata_t * pdat = (struct fb_rk3128_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

static struct surface_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_rk3128_pdata_t * pdat = (struct fb_rk3128_pdata_t *)fb->priv;
	return surface_alloc(pdat->width, pdat->height, NULL);
}

static void fb_destroy(struct framebuffer_t * fb, struct surface_t * s)
{
	surface_free(s);
}

static void fb_present(struct framebuffer_t * fb, struct surface_t * s, struct region_list_t * rl)
{
	struct fb_rk3128_pdata_t * pdat = (struct fb_rk3128_pdata_t *)fb->priv;

	pdat->index = (pdat->index + 1) & 0x1;
	memcpy(pdat->vram[pdat->index], s->pixels, s->pixlen);
	dma_cache_sync(pdat->vram[pdat->index], pdat->pixlen, DMA_TO_DEVICE);
	rk3128_lcd_set_win0_address(pdat, pdat->vram[pdat->index]);
	rk3128_lcd_update_config(pdat);
}

static struct device_t * fb_rk3128_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_rk3128_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct fb_rk3128_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->virtlcd = virt;
	pdat->virtgrf = phys_to_virt(RK3128_GRF_BASE);
	pdat->regulator = strdup(dt_read_string(n, "regulator-name", NULL));
	pdat->lcdrst = dt_read_int(n, "lcd-reset-gpio", -1);
	pdat->lcdrstcfg = dt_read_int(n, "lcd-reset-gpio-config", -1);
	pdat->lcden = dt_read_int(n, "lcd-enable-gpio", -1);
	pdat->lcdencfg = dt_read_int(n, "lcd-enable-gpio-config", -1);
	pdat->clk = strdup(clk);
	pdat->width = dt_read_int(n, "width", 1024);
	pdat->height = dt_read_int(n, "height", 600);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->bits_per_pixel = 32;
	pdat->pixlen = pdat->width * pdat->height * 4;
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->pixlen);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->pixlen);

	pdat->timing.pixel_clock_hz = dt_read_long(n, "clock-frequency", 48000000);
	pdat->timing.h_front_porch = dt_read_int(n, "hfront-porch", 1);
	pdat->timing.h_back_porch = dt_read_int(n, "hback-porch", 1);
	pdat->timing.h_sync_len = dt_read_int(n, "hsync-len", 1);
	pdat->timing.v_front_porch = dt_read_int(n, "vfront-porch", 1);
	pdat->timing.v_back_porch = dt_read_int(n, "vback-porch", 1);
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

	regulator_enable(pdat->regulator);
	clk_enable(pdat->clk);
	if(pdat->lcdrst >= 0)
	{
		if(pdat->lcdrstcfg >= 0)
			gpio_set_cfg(pdat->lcdrst, pdat->lcdrstcfg);
		gpio_set_pull(pdat->lcdrst, GPIO_PULL_UP);
		gpio_set_value(pdat->lcdrst, 1);
	}
	if(pdat->lcden >= 0)
	{
		if(pdat->lcdencfg >= 0)
			gpio_set_cfg(pdat->lcden, pdat->lcdencfg);
		gpio_set_pull(pdat->lcden, GPIO_PULL_UP);
		gpio_set_value(pdat->lcden, 1);
	}
	rk3128_fb_init(pdat);

	if(!(dev = register_framebuffer(fb, drv)))
	{
		regulator_disable(pdat->regulator);
		free(pdat->regulator);
		clk_disable(pdat->clk);
		free(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	return dev;
}

static void fb_rk3128_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_rk3128_pdata_t * pdat = (struct fb_rk3128_pdata_t *)fb->priv;

	if(fb)
	{
		unregister_framebuffer(fb);
		regulator_disable(pdat->regulator);
		free(pdat->regulator);
		clk_disable(pdat->clk);
		free(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_rk3128_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_rk3128_pdata_t * pdat = (struct fb_rk3128_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_rk3128_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_rk3128_pdata_t * pdat = (struct fb_rk3128_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_rk3128 = {
	.name		= "fb-rk3128",
	.probe		= fb_rk3128_probe,
	.remove		= fb_rk3128_remove,
	.suspend	= fb_rk3128_suspend,
	.resume		= fb_rk3128_resume,
};

static __init void fb_rk3128_driver_init(void)
{
	register_driver(&fb_rk3128);
}

static __exit void fb_rk3128_driver_exit(void)
{
	unregister_driver(&fb_rk3128);
}

driver_initcall(fb_rk3128_driver_init);
driver_exitcall(fb_rk3128_driver_exit);
