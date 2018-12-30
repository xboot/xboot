/*
 * driver/fb-rk3288.c
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
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <led/led.h>
#include <regulator/regulator.h>
#include <framebuffer/framebuffer.h>
#include <rk3288/reg-vop.h>
#include <rk3288/reg-grf.h>
#include <rk3288/reg-lvds.h>
#include <rk3288-gpio.h>

enum rk3288_vop_interface_t {
	RK3288_VOP_INTERFACE_RGB_LVDS	= 0,
	RK3288_VOP_INTERFACE_HDMI		= 1,
	RK3288_VOP_INTERFACE_EDP		= 2,
	RK3288_VOP_INTERFACE_MIPI		= 3,
};

enum rk3288_lvds_output_t {
	RK3288_LVDS_OUTPUT_RGB			= 0,
	RK3288_LVDS_OUTPUT_SINGLE		= 1,
	RK3288_LVDS_OUTPUT_DUAL			= 2,
};

enum rk3288_lvds_format_t {
	RK3288_LVDS_FORMAT_VESA			= 0,
	RK3288_LVDS_FORMAT_JEIDA		= 1,
};

struct fb_rk3288_pdata_t
{
	virtual_addr_t virtvop;
	virtual_addr_t virtgrf;
	virtual_addr_t virtlvds;

	char * lcd_avdd_3v3;
	char * lcd_avdd_1v8;
	char * lcd_avdd_1v0;
	char * clk;
	int width;
	int height;
	int pwidth;
	int pheight;
	int bits_per_pixel;
	int bytes_per_pixel;
	int index;
	void * vram[2];

	enum rk3288_vop_interface_t interface;
	enum rk3288_lvds_output_t output;
	enum rk3288_lvds_format_t format;

	struct {
		int mirrorx;
		int mirrory;
		int swaprg;
		int swaprb;
		int swapbg;
	} mode;

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

static inline void rk3288_vop_set_interface(struct fb_rk3288_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtvop + VOP_SYS_CTRL);
	cfg &= ~(0xf << 12);
	cfg |= (0x1 << (pdat->interface + 12));
	write32(pdat->virtvop + VOP_SYS_CTRL, cfg);
}

static inline void rk3288_vop_set_mode(struct fb_rk3288_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtvop + VOP_DSP_CTRL0);
	cfg &= ~(0xf << 0);
	cfg |= (0x0 << 0);

	cfg &= ~(0x1 << 22);
	cfg |= ((pdat->mode.mirrorx ? 1 : 0) << 22);
	cfg &= ~(0x1 << 23);
	cfg |= ((pdat->mode.mirrory ? 1 : 0) << 23);
	cfg &= ~(0x1 << 14);
	cfg |= ((pdat->mode.swaprg ? 1 : 0) << 14);
	cfg &= ~(0x1 << 13);
	cfg |= ((pdat->mode.swaprb ? 1 : 0) << 13);
	cfg &= ~(0x1 << 12);
	cfg |= ((pdat->mode.swapbg ? 1 : 0) << 12);
	write32(pdat->virtvop + VOP_DSP_CTRL0, cfg);
}

static inline void rk3288_vop_set_timing(struct fb_rk3288_pdata_t * pdat)
{
	u32_t cfg;

	cfg = (pdat->timing.h_sync_len & 0x1fff) << 0;
	cfg |= ((pdat->timing.h_sync_len + pdat->timing.h_back_porch + pdat->width + pdat->timing.h_front_porch) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_DSP_HTOTAL_HS_END, cfg);

	cfg = ((pdat->timing.h_sync_len + pdat->timing.h_back_porch + pdat->width) & 0x1fff) << 0;
	cfg |= ((pdat->timing.h_sync_len + pdat->timing.h_back_porch) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_DSP_HACT_ST_END, cfg);

	cfg = (pdat->timing.v_sync_len & 0x1fff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch + pdat->height + pdat->timing.v_front_porch) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_DSP_VTOTAL_VS_END, cfg);

	cfg = ((pdat->timing.v_sync_len + pdat->timing.v_back_porch + pdat->height) & 0x1fff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_DSP_VACT_ST_END, cfg);

	cfg = ((pdat->timing.h_sync_len + pdat->timing.h_back_porch + pdat->width) & 0x1fff) << 0;
	cfg |= ((pdat->timing.h_sync_len + pdat->timing.h_back_porch) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_POST_DSP_HACT_INFO, cfg);

	cfg = ((pdat->timing.v_sync_len + pdat->timing.v_back_porch + pdat->height) & 0x1fff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_POST_DSP_VACT_INFO, cfg);
}

static inline void rk3288_vop_set_polarity(struct fb_rk3288_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtvop + VOP_DSP_CTRL0);
	cfg &= ~(0xf << 4);
	if(pdat->timing.h_sync_active)
		cfg |= (0x1 << 4);
	if(pdat->timing.v_sync_active)
		cfg |= (0x1 << 5);
	if(pdat->timing.den_active)
		cfg |= (0x1 << 6);
	if(pdat->timing.clk_active)
		cfg |= (0x1 << 7);
	write32(pdat->virtvop + VOP_DSP_CTRL0, cfg);
}

static inline void rk3288_vop_set_win0(struct fb_rk3288_pdata_t * pdat)
{
	u32_t cfg;
	u32_t lb, fmt;

	cfg = ((pdat->width - 1) & 0x1fff) << 0;
	cfg |= ((pdat->height - 1) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_WIN0_ACT_INFO, cfg);

	cfg = ((pdat->width - 1) & 0x1fff) << 0;
	cfg |= ((pdat->height - 1) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_WIN0_DSP_INFO, cfg);

	cfg = ((pdat->timing.h_sync_len + pdat->timing.h_back_porch) & 0x1fff) << 0;
	cfg |= ((pdat->timing.v_sync_len + pdat->timing.v_back_porch) & 0x1fff) << 16;
	write32(pdat->virtvop + VOP_WIN0_DSP_ST, cfg);

	write32(pdat->virtvop + VOP_WIN0_COLOR_KEY, 0);
	switch(pdat->bits_per_pixel)
	{
	case 16:
		fmt = 2;
		write32(pdat->virtvop + VOP_WIN0_VIR, (((pdat->width / 2) & 0x3fff) << 0));
		break;
	case 24:
		fmt = 1;
		write32(pdat->virtvop + VOP_WIN0_VIR, (((((pdat->width * 3) >> 2)+((pdat->width) % 3)) & 0x3fff) << 0));
		break;
	case 32:
		fmt = 0;
		write32(pdat->virtvop + VOP_WIN0_VIR, (((pdat->width) & 0x3fff) << 0));
		break;
	default:
		fmt = 0;
		write32(pdat->virtvop + VOP_WIN0_VIR, (((pdat->width) & 0x3fff) << 0));
		break;
	}

	if(pdat->width > 2560)
		lb = 2;
	else if(pdat->width > 1920)
		lb = 3;
	else if(pdat->width > 1280)
		lb = 4;
	else
		lb = 5;

	cfg = read32(pdat->virtvop + VOP_WIN0_CTRL0);
	cfg &= ~((0x7 << 5) | (0x7 << 1) | (0x1 << 0));
	cfg |= (lb & 0x7) << 5;
	cfg |= (fmt & 0x7) << 1;
	cfg |= 0x1 << 0;
	write32(pdat->virtvop + VOP_WIN0_CTRL0, cfg);
}

static inline void rk3288_vop_set_win0_address(struct fb_rk3288_pdata_t * pdat, void * vram)
{
	write32(pdat->virtvop + VOP_WIN0_YRGB_MST, (u32_t)vram);
}

static inline void rk3288_vop_update_config(struct fb_rk3288_pdata_t * pdat)
{
	write32(pdat->virtvop + VOP_REG_CFG_DONE, 0x01);
}

static inline void rk3288_lvds_enable(struct fb_rk3288_pdata_t * pdat, int vop)
{
	u32_t cfg;

	if(vop == 0)
		cfg = (1 << (16 + 3)) | (0 << 3);
	else
		cfg = (1 << (16 + 3)) | (1 << 3);
	write32(pdat->virtgrf + GRF_SOC_CON6, cfg);

	switch(pdat->output)
	{
	case RK3288_LVDS_OUTPUT_RGB:
		cfg = (0xffff << 16) | (1 << 6) | (1 << 11) | (1 << 12) | (pdat->format << 0);
		break;
	case RK3288_LVDS_OUTPUT_SINGLE:
		cfg = (0xffff << 16) | (1 << 11) | (pdat->format << 0);
		break;
	case RK3288_LVDS_OUTPUT_DUAL:
		cfg = (0xffff << 16) | (1 << 4) | (1 << 11) | (1 << 12) | (pdat->format << 0);
		break;
	default:
		cfg = (0xffff << 16) | (1 << 6) | (1 << 11) | (1 << 12) | (pdat->format << 0);
		break;
	}
	write32(pdat->virtgrf + GRF_SOC_CON7, cfg);

	if(pdat->output == RK3288_LVDS_OUTPUT_RGB)
	{
		write32(pdat->virtlvds + LVDS_CH0_REG0, 0x7f);	write32(pdat->virtlvds + LVDS_CH1_REG0, 0x7f);
		write32(pdat->virtlvds + LVDS_CH0_REG1, 0x40);	write32(pdat->virtlvds + LVDS_CH1_REG1, 0x40);
		write32(pdat->virtlvds + LVDS_CH0_REG2, 0x00);	write32(pdat->virtlvds + LVDS_CH1_REG2, 0x00);
		write32(pdat->virtlvds + LVDS_CH0_REG3, 0x46);	write32(pdat->virtlvds + LVDS_CH1_REG3, 0x46);
		write32(pdat->virtlvds + LVDS_CH0_REG4, 0x3f);	write32(pdat->virtlvds + LVDS_CH1_REG4, 0x3f);
		write32(pdat->virtlvds + LVDS_CH0_REG5, 0x3f);	write32(pdat->virtlvds + LVDS_CH1_REG5, 0x3f);
		write32(pdat->virtlvds + LVDS_CH0_REGD, 0x0a);	write32(pdat->virtlvds + LVDS_CH1_REGD, 0x0a);
		write32(pdat->virtlvds + LVDS_CH0_REG20, 0x44);	write32(pdat->virtlvds + LVDS_CH1_REG20, 0x44);
		write32(pdat->virtlvds + LVDS_CFG_REGC, 0x00);
		write32(pdat->virtlvds + LVDS_CFG_REG21, 0x92);
	}
	else
	{
		write32(pdat->virtlvds + LVDS_CH0_REG0, 0xbf);	write32(pdat->virtlvds + LVDS_CH1_REG0, 0xbf);
		write32(pdat->virtlvds + LVDS_CH0_REG1, 0x3f);	write32(pdat->virtlvds + LVDS_CH1_REG1, 0x3f);
		write32(pdat->virtlvds + LVDS_CH0_REG2, 0xfe);	write32(pdat->virtlvds + LVDS_CH1_REG2, 0xfe);
		write32(pdat->virtlvds + LVDS_CH0_REG3, 0x46);	write32(pdat->virtlvds + LVDS_CH1_REG3, 0x46);
		write32(pdat->virtlvds + LVDS_CH0_REG4, 0x00);	write32(pdat->virtlvds + LVDS_CH1_REG4, 0x00);
		write32(pdat->virtlvds + LVDS_CH0_REG5, 0x00);	write32(pdat->virtlvds + LVDS_CH1_REG5, 0x00);
		write32(pdat->virtlvds + LVDS_CH0_REGD, 0x0a);	write32(pdat->virtlvds + LVDS_CH1_REGD, 0x0a);
		write32(pdat->virtlvds + LVDS_CH0_REG20, 0x44);	write32(pdat->virtlvds + LVDS_CH1_REG20, 0x44);
		write32(pdat->virtlvds + LVDS_CFG_REGC, 0x00);
		write32(pdat->virtlvds + LVDS_CFG_REG21, 0x92);
	}
}

static void rk3288_fb_init(struct fb_rk3288_pdata_t * pdat)
{
	gpio_set_cfg(RK3288_GPIO1_D0, 0x1);
	gpio_set_cfg(RK3288_GPIO1_D1, 0x1);
	gpio_set_cfg(RK3288_GPIO1_D2, 0x1);
	gpio_set_cfg(RK3288_GPIO1_D3, 0x1);

	gpio_set_rate(RK3288_GPIO1_D0, GPIO_RATE_FAST);
	gpio_set_rate(RK3288_GPIO1_D1, GPIO_RATE_FAST);
	gpio_set_rate(RK3288_GPIO1_D2, GPIO_RATE_FAST);
	gpio_set_rate(RK3288_GPIO1_D3, GPIO_RATE_FAST);

	gpio_set_drv(RK3288_GPIO1_D0, GPIO_DRV_STRONG);
	gpio_set_drv(RK3288_GPIO1_D1, GPIO_DRV_STRONG);
	gpio_set_drv(RK3288_GPIO1_D2, GPIO_DRV_STRONG);
	gpio_set_drv(RK3288_GPIO1_D3, GPIO_DRV_STRONG);

	rk3288_vop_set_interface(pdat);
	rk3288_vop_set_mode(pdat);
	rk3288_vop_set_timing(pdat);
	rk3288_vop_set_polarity(pdat);
	rk3288_vop_update_config(pdat);

	rk3288_vop_set_win0(pdat);
	rk3288_vop_set_win0_address(pdat, pdat->vram[0]);
	rk3288_vop_update_config(pdat);

	rk3288_lvds_enable(pdat, 0);
}

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_rk3288_pdata_t * pdat = (struct fb_rk3288_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_rk3288_pdata_t * pdat = (struct fb_rk3288_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

static struct render_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_rk3288_pdata_t * pdat = (struct fb_rk3288_pdata_t *)fb->priv;
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
	struct fb_rk3288_pdata_t * pdat = (struct fb_rk3288_pdata_t *)fb->priv;

	if(render && render->pixels)
	{
		pdat->index = (pdat->index + 1) & 0x1;
		memcpy(pdat->vram[pdat->index], render->pixels, render->pixlen);
		dma_cache_sync(pdat->vram[pdat->index], render->pixlen, DMA_TO_DEVICE);
		rk3288_vop_set_win0_address(pdat, pdat->vram[pdat->index]);
		rk3288_vop_update_config(pdat);
	}
}

static struct device_t * fb_rk3288_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_rk3288_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct fb_rk3288_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->virtvop = virt;
	pdat->virtgrf = phys_to_virt(RK3288_GRF_BASE);
	pdat->virtlvds = phys_to_virt(RK3288_LVDS_BASE);
	pdat->lcd_avdd_3v3 = strdup(dt_read_string(n, "regulator-lcd-avdd-3v3", NULL));
	pdat->lcd_avdd_1v8 = strdup(dt_read_string(n, "regulator-lcd-avdd-1v8", NULL));
	pdat->lcd_avdd_1v0 = strdup(dt_read_string(n, "regulator-lcd-avdd-1v0", NULL));
	pdat->clk = strdup(clk);
	pdat->width = dt_read_int(n, "width", 1024);
	pdat->height = dt_read_int(n, "height", 600);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->bits_per_pixel = dt_read_int(n, "bits-per-pixel", 32);
	pdat->bytes_per_pixel = dt_read_int(n, "bytes-per-pixel", 4);
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes_per_pixel);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes_per_pixel);

	pdat->interface = RK3288_VOP_INTERFACE_RGB_LVDS;
	pdat->output = RK3288_LVDS_OUTPUT_RGB;
	pdat->format = RK3288_LVDS_FORMAT_JEIDA;

	pdat->mode.mirrorx = 0;
	pdat->mode.mirrory = 0;
	pdat->mode.swaprg = 0;
	pdat->mode.swaprb = 0;
	pdat->mode.swapbg = 0;

	pdat->timing.pixel_clock_hz = dt_read_long(n, "clock-frequency", 52000000);
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
	fb->bpp = pdat->bits_per_pixel;
	fb->setbl = fb_setbl;
	fb->getbl = fb_getbl;
	fb->create = fb_create;
	fb->destroy = fb_destroy;
	fb->present = fb_present;
	fb->priv = pdat;

	regulator_set_voltage(pdat->lcd_avdd_3v3, 3300000);
	regulator_enable(pdat->lcd_avdd_3v3);
	regulator_set_voltage(pdat->lcd_avdd_1v8, 1800000);
	regulator_enable(pdat->lcd_avdd_1v8);
	regulator_set_voltage(pdat->lcd_avdd_1v0, 1000000);
	regulator_enable(pdat->lcd_avdd_1v0);
	clk_enable(pdat->clk);
	rk3288_fb_init(pdat);

	if(!register_framebuffer(&dev, fb))
	{
		regulator_disable(pdat->lcd_avdd_3v3);
		free(pdat->lcd_avdd_3v3);
		regulator_disable(pdat->lcd_avdd_1v8);
		free(pdat->lcd_avdd_1v8);
		regulator_disable(pdat->lcd_avdd_1v0);
		free(pdat->lcd_avdd_1v0);
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

static void fb_rk3288_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_rk3288_pdata_t * pdat = (struct fb_rk3288_pdata_t *)fb->priv;

	if(fb && unregister_framebuffer(fb))
	{
		regulator_disable(pdat->lcd_avdd_3v3);
		free(pdat->lcd_avdd_3v3);
		regulator_disable(pdat->lcd_avdd_1v8);
		free(pdat->lcd_avdd_1v8);
		regulator_disable(pdat->lcd_avdd_1v0);
		free(pdat->lcd_avdd_1v0);
		clk_disable(pdat->clk);
		free(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_rk3288_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_rk3288_pdata_t * pdat = (struct fb_rk3288_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_rk3288_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_rk3288_pdata_t * pdat = (struct fb_rk3288_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_rk3288 = {
	.name		= "fb-rk3288",
	.probe		= fb_rk3288_probe,
	.remove		= fb_rk3288_remove,
	.suspend	= fb_rk3288_suspend,
	.resume		= fb_rk3288_resume,
};

static __init void fb_rk3288_driver_init(void)
{
	register_driver(&fb_rk3288);
}

static __exit void fb_rk3288_driver_exit(void)
{
	unregister_driver(&fb_rk3288);
}

driver_initcall(fb_rk3288_driver_init);
driver_exitcall(fb_rk3288_driver_exit);
