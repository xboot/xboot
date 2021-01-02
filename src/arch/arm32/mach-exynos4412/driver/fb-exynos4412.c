/*
 * driver/fb-exynos4412.c
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
#include <exynos4412-gpio.h>
#include <exynos4412/reg-lcd.h>
#include <exynos4412/reg-sys.h>

enum exynos4412_fb_output_t
{
	EXYNOS4412_FB_OUTPUT_RGB,
	EXYNOS4412_FB_OUTPUT_I80LDI0,
	EXYNOS4412_FB_OUTPUT_I80LDI1,
	EXYNOS4412_FB_OUTPUT_WB_RGB,
	EXYNOS4412_FB_OUTPUT_WB_I80LDI0,
	EXYNOS4412_FB_OUTPUT_WB_I80LDI1,
};

enum exynos4412_fb_rgb_mode_t
{
	EXYNOS4412_FB_MODE_RGB_P 			= 0,
	EXYNOS4412_FB_MODE_BGR_P 			= 1,
	EXYNOS4412_FB_MODE_RGB_S 			= 2,
	EXYNOS4412_FB_MODE_BGR_S 			= 3,
};

enum exynos4412_bpp_mode_t
{
	EXYNOS4412_FB_BPP_MODE_1BPP			= 0x0,
	EXYNOS4412_FB_BPP_MODE_2BPP			= 0x1,
	EXYNOS4412_FB_BPP_MODE_4BPP			= 0x2,
	EXYNOS4412_FB_BPP_MODE_8BPP_PAL		= 0x3,
	EXYNOS4412_FB_BPP_MODE_8BPP			= 0x4,
	EXYNOS4412_FB_BPP_MODE_16BPP_565	= 0x5,
	EXYNOS4412_FB_BPP_MODE_16BPP_A555	= 0x6,
	EXYNOS4412_FB_BPP_MODE_16BPP_X555	= 0x7,
	EXYNOS4412_FB_BPP_MODE_18BPP_666	= 0x8,
	EXYNOS4412_FB_BPP_MODE_18BPP_A665	= 0x9,
	EXYNOS4412_FB_BPP_MODE_19BPP_A666	= 0xa,
	EXYNOS4412_FB_BPP_MODE_24BPP_888	= 0xb,
	EXYNOS4412_FB_BPP_MODE_24BPP_A887	= 0xc,
	EXYNOS4412_FB_BPP_MODE_32BPP		= 0xd,
	EXYNOS4412_FB_BPP_MODE_16BPP_A444	= 0xe,
	EXYNOS4412_FB_BPP_MODE_15BPP_555	= 0xf,
};

enum {
	EXYNOS4412_FB_SWAP_WORD				= (0x1 << 0),
	EXYNOS4412_FB_SWAP_HWORD			= (0x1 << 1),
	EXYNOS4412_FB_SWAP_BYTE				= (0x1 << 2),
	EXYNOS4412_FB_SWAP_BIT				= (0x1 << 3),
};

struct fb_exynos4412_pdata_t
{
	virtual_addr_t virt;

	char * clk;
	int width;
	int height;
	int pwidth;
	int pheight;
	int pixlen;
	int index;
	void * vram[2];

	enum exynos4412_fb_output_t output;
	enum exynos4412_fb_rgb_mode_t rgb_mode;
	enum exynos4412_bpp_mode_t bpp_mode;
	int swap;

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

static void exynos4412_fb_set_output(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virt + VIDCON0);
	cfg &= ~VIDCON0_VIDOUT_MASK;

	if(pdat->output == EXYNOS4412_FB_OUTPUT_RGB)
		cfg |= VIDCON0_VIDOUT_RGB;
	else if(pdat->output == EXYNOS4412_FB_OUTPUT_I80LDI0)
		cfg |= VIDCON0_VIDOUT_I80LDI0;
	else if(pdat->output == EXYNOS4412_FB_OUTPUT_I80LDI1)
		cfg |= VIDCON0_VIDOUT_I80LDI1;
	else if(pdat->output == EXYNOS4412_FB_OUTPUT_WB_RGB)
		cfg |= VIDCON0_VIDOUT_WB_RGB;
	else if(pdat->output == EXYNOS4412_FB_OUTPUT_WB_I80LDI0)
		cfg |= VIDCON0_VIDOUT_WB_I80LDI0;
	else if(pdat->output == EXYNOS4412_FB_OUTPUT_WB_I80LDI1)
		cfg |= VIDCON0_VIDOUT_WB_I80LDI1;
	write32(pdat->virt + VIDCON0, cfg);

	cfg = read32(pdat->virt + VIDCON2);
	cfg |= 0x3 << 14;
	write32(pdat->virt + VIDCON2, cfg);
}

static void exynos4412_fb_set_display_mode(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virt + VIDCON0);
	cfg &= ~(3 << 17);
	cfg |= (pdat->rgb_mode << 17);
	write32(pdat->virt + VIDCON0, cfg);
}

static void exynos4412_fb_display_on(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virt + VIDCON0);
	cfg |= 0x3 << 0;
	write32(pdat->virt + VIDCON0, cfg);
}

static void exynos4412_fb_display_off(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virt + VIDCON0);
	cfg &= ~(1 << 1);
	write32(pdat->virt + VIDCON0, cfg);

	cfg &= ~(1 << 0);
	write32(pdat->virt + VIDCON0, cfg);
}

static void exynos4412_fb_set_clock(struct fb_exynos4412_pdata_t * pdat)
{
	u64_t rate;
	u32_t div;
	u32_t cfg;

	rate = clk_get_rate(pdat->clk);
	div = (u32_t)(rate / pdat->timing.pixel_clock_hz);
	if((rate % pdat->timing.pixel_clock_hz) > 0)
		div++;

	cfg = read32(pdat->virt + VIDCON0);
	cfg &= ~((1 << 16) | (1 << 5));
	cfg |= VIDCON0_CLKVAL_F(div - 1);
	write32(pdat->virt + VIDCON0, cfg);
}

static void exynos4412_fb_set_polarity(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg = 0;

	if(pdat->timing.clk_active)
		cfg |= (1 << 7);
	if(pdat->timing.h_sync_active)
		cfg |= (1 << 6);
	if(pdat->timing.v_sync_active)
		cfg |= (1 << 5);
	if(pdat->timing.den_active)
		cfg |= (1 << 4);
	write32(pdat->virt + VIDCON1, cfg);
}

static void exynos4412_fb_set_timing(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg;

	cfg = 0;
	cfg |= VIDTCON0_VBPDE(1 - 1);
	cfg |= VIDTCON0_VBPD(pdat->timing.v_back_porch - 1);
	cfg |= VIDTCON0_VFPD(pdat->timing.v_front_porch - 1);
	cfg |= VIDTCON0_VSPW(pdat->timing.v_sync_len - 1);
	write32(pdat->virt + VIDTCON0, cfg);

	cfg = 0;
	cfg |= VIDTCON1_VFPDE(1 - 1);
	cfg |= VIDTCON1_HBPD(pdat->timing.h_back_porch - 1);
	cfg |= VIDTCON1_HFPD(pdat->timing.h_front_porch - 1);
	cfg |= VIDTCON1_HSPW(pdat->timing.h_sync_len - 1);
	write32(pdat->virt + VIDTCON1, cfg);
}

static void exynos4412_fb_set_lcd_size(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg = 0;

	cfg |= VIDTCON2_HOZVAL(pdat->width - 1);
	cfg |= VIDTCON2_LINEVAL(pdat->height - 1);
	write32(pdat->virt + VIDTCON2, cfg);
}

static void exynos4412_fb_set_buffer_address(struct fb_exynos4412_pdata_t * pdat, int id, void * vram)
{
	u32_t start, end;
	u32_t shw;

	start = (u32_t)(vram);
	end = (u32_t)((start + pdat->pixlen) & 0x00ffffff);

	shw = read32(pdat->virt + SHADOWCON);
	shw |= SHADOWCON_PROTECT(id);
	write32(pdat->virt + SHADOWCON, shw);

	switch(id)
	{
	case 0:
		write32(pdat->virt + VIDW00ADD0B0, start);
		write32(pdat->virt + VIDW00ADD1B0, end);
		break;
	case 1:
		write32(pdat->virt + VIDW01ADD0B0, start);
		write32(pdat->virt + VIDW01ADD1B0, end);
		break;
	case 2:
		write32(pdat->virt + VIDW02ADD0B0, start);
		write32(pdat->virt + VIDW02ADD1B0, end);
		break;
	case 3:
		write32(pdat->virt + VIDW03ADD0B0, start);
		write32(pdat->virt + VIDW03ADD1B0, end);
		break;
	case 4:
		write32(pdat->virt + VIDW04ADD0B0, start);
		write32(pdat->virt + VIDW04ADD1B0, end);
		break;
	default:
		break;
	}

	shw = read32(pdat->virt + SHADOWCON);
	shw &= ~(SHADOWCON_PROTECT(id));
	write32(pdat->virt + SHADOWCON, shw);
}

static void exynos4412_fb_set_buffer_size(struct fb_exynos4412_pdata_t * pdat, int id)
{
	u32_t cfg = 0;

	cfg = VIDADDR_PAGEWIDTH(pdat->width * 4);
	cfg |= VIDADDR_OFFSIZE(0);

	switch(id)
	{
	case 0:
		write32(pdat->virt + VIDW00ADD2, cfg);
		break;
	case 1:
		write32(pdat->virt + VIDW01ADD2, cfg);
		break;
	case 2:
		write32(pdat->virt + VIDW02ADD2, cfg);
		break;
	case 3:
		write32(pdat->virt + VIDW03ADD2, cfg);
		break;
	case 4:
		write32(pdat->virt + VIDW04ADD2, cfg);
		break;
	default:
		break;
	}
}

static void exynos4412_fb_set_window_position(struct fb_exynos4412_pdata_t * pdat, int id)
{
	u32_t cfg, shw;

	shw = read32(pdat->virt + SHADOWCON);
	shw |= SHADOWCON_PROTECT(id);
	write32(pdat->virt + SHADOWCON, shw);

	switch(id)
	{
	case 0:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		write32(pdat->virt + VIDOSD0A, cfg);
		cfg = VIDOSD_RIGHT_X(pdat->width - 1) | VIDOSD_BOTTOM_Y(pdat->height - 1);
		write32(pdat->virt + VIDOSD0B, cfg);
		break;
	case 1:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		write32(pdat->virt + VIDOSD1A, cfg);
		cfg = VIDOSD_RIGHT_X(pdat->width - 1) | VIDOSD_BOTTOM_Y(pdat->height - 1);
		write32(pdat->virt + VIDOSD1B, cfg);
		break;
	case 2:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		write32(pdat->virt + VIDOSD2A, cfg);
		cfg = VIDOSD_RIGHT_X(pdat->width - 1) | VIDOSD_BOTTOM_Y(pdat->height - 1);
		write32(pdat->virt + VIDOSD2B, cfg);
		break;
	case 3:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		write32(pdat->virt + VIDOSD3A, cfg);
		cfg = VIDOSD_RIGHT_X(pdat->width - 1) | VIDOSD_BOTTOM_Y(pdat->height - 1);
		write32(pdat->virt + VIDOSD3B, cfg);
		break;
	case 4:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		write32(pdat->virt + VIDOSD4A, cfg);
		cfg = VIDOSD_RIGHT_X(pdat->width - 1) | VIDOSD_BOTTOM_Y(pdat->height - 1);
		write32(pdat->virt + VIDOSD4B, cfg);
		break;
	default:
		break;
	}

	shw = read32(pdat->virt + SHADOWCON);
	shw &= ~(SHADOWCON_PROTECT(id));
	write32(pdat->virt + SHADOWCON, shw);
}

static void exynos4412_fb_set_window_size(struct fb_exynos4412_pdata_t * pdat, int id)
{
	u32_t cfg;

	if(id > 2)
		return;

	cfg = VIDOSD_SIZE(pdat->width * pdat->height);
	switch(id)
	{
	case 0:
		write32(pdat->virt + VIDOSD0C, cfg);
		break;
	case 1:
		write32(pdat->virt + VIDOSD1D, cfg);
		break;
	case 2:
		write32(pdat->virt + VIDOSD2D, cfg);
		break;
	default:
		break;
	}
}

static void exynos4412_fb_window0_enable(struct fb_exynos4412_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virt + WINCON0);
	cfg &= ~((1 << 18) | (1 << 17) |
			(1 << 16) | (1 << 15) |
			(3 << 9) | (0xf << 2) |
			(1 << 13) | (1 << 22) |
			(1 << 1));
	cfg |= (0 << 1);
	cfg |= (0 << 13);
	cfg |= (0 << 22);
	cfg |= (1 << 0);

	if(pdat->swap & EXYNOS4412_FB_SWAP_WORD)
		cfg |= (1 << 15);
	if(pdat->swap & EXYNOS4412_FB_SWAP_HWORD)
		cfg |= (1 << 16);
	if(pdat->swap & EXYNOS4412_FB_SWAP_BYTE)
		cfg |= (1 << 17);
	if(pdat->swap & EXYNOS4412_FB_SWAP_BIT)
		cfg |= (1 << 18);
	cfg |= (pdat->bpp_mode << 2);
	write32(pdat->virt + WINCON0, cfg);

	cfg = read32(pdat->virt + SHADOWCON);
	cfg |= 1 << 0;
	write32(pdat->virt + SHADOWCON, cfg);
}

static void exynos4412_fb_cfg_gpios(int base, int n, int cfg, enum gpio_pull_t pull, enum gpio_drv_t drv)
{
	for(; n > 0; n--, base++)
	{
		gpio_set_cfg(base, cfg);
		gpio_set_pull(base, pull);
		gpio_set_drv(base, drv);
	}
}

static inline void fb_exynos4412_init(struct fb_exynos4412_pdata_t * pdat)
{
	virtual_addr_t virt;

	/*
	 * Enable lcd clk
	 */
	clk_enable(pdat->clk);

	/*
	 * Initial lcd port
	 */
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF0(0), 8, 0x2, GPIO_PULL_NONE, GPIO_DRV_STRONG);
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF1(0), 8, 0x2, GPIO_PULL_NONE, GPIO_DRV_STRONG);
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF2(0), 8, 0x2, GPIO_PULL_NONE, GPIO_DRV_STRONG);
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF3(0), 4, 0x2, GPIO_PULL_NONE, GPIO_DRV_STRONG);

	/*
	 * Display path selection
	 */
	virt = phys_to_virt(EXYNOS4412_SYS_BASE);
	write32(virt + LCDBLK_CFG, (read32(virt + LCDBLK_CFG) & ~(0x3 << 0)) | (0x2 << 0));
	write32(virt + LCDBLK_CFG2, (read32(virt + LCDBLK_CFG2) & ~(0x1 << 0)) | (0x1 << 0));

	/*
	 * Turn off all windows
	 */
	write32(pdat->virt + WINCON0, (read32(pdat->virt + WINCON0) & ~0x1));
	write32(pdat->virt + WINCON1, (read32(pdat->virt + WINCON1) & ~0x1));
	write32(pdat->virt + WINCON2, (read32(pdat->virt + WINCON2) & ~0x1));
	write32(pdat->virt + WINCON3, (read32(pdat->virt + WINCON3) & ~0x1));
	write32(pdat->virt + WINCON4, (read32(pdat->virt + WINCON4) & ~0x1));

	/*
	 * Turn off all windows color map
	 */
	write32(pdat->virt + WIN0MAP, (read32(pdat->virt + WIN0MAP) & ~(1 << 24)));
	write32(pdat->virt + WIN1MAP, (read32(pdat->virt + WIN1MAP) & ~(1 << 24)));
	write32(pdat->virt + WIN2MAP, (read32(pdat->virt + WIN2MAP) & ~(1 << 24)));
	write32(pdat->virt + WIN3MAP, (read32(pdat->virt + WIN3MAP) & ~(1 << 24)));
	write32(pdat->virt + WIN4MAP, (read32(pdat->virt + WIN4MAP) & ~(1 << 24)));

	/*
	 * Turn off all windows color key and blending
	 */
	write32(pdat->virt + W1KEYCON0, (read32(pdat->virt + W1KEYCON0) & ~(3 << 25)));
	write32(pdat->virt + W2KEYCON0, (read32(pdat->virt + W2KEYCON0) & ~(3 << 25)));
	write32(pdat->virt + W3KEYCON0, (read32(pdat->virt + W3KEYCON0) & ~(3 << 25)));
	write32(pdat->virt + W4KEYCON0, (read32(pdat->virt + W4KEYCON0) & ~(3 << 25)));

	/*
	 * Initial lcd controller
	 */
	exynos4412_fb_set_output(pdat);
	exynos4412_fb_set_display_mode(pdat);
	exynos4412_fb_display_off(pdat);
	exynos4412_fb_set_polarity(pdat);
	exynos4412_fb_set_timing(pdat);
	exynos4412_fb_set_lcd_size(pdat);
	exynos4412_fb_set_clock(pdat);

	/*
	 * Set lcd video buffer
	 */
	exynos4412_fb_set_buffer_size(pdat, 0);
	exynos4412_fb_set_window_position(pdat, 0);
	exynos4412_fb_set_window_size(pdat, 0);

	/*
	 * Enable window 0 for main display area
	 */
	exynos4412_fb_window0_enable(pdat);

	/*
	 * Display on
	 */
	exynos4412_fb_display_on(pdat);

	/*
	 * Enable backlight
	 */
	gpio_set_pull(EXYNOS4412_GPD0(0), GPIO_PULL_UP);
	gpio_set_cfg(EXYNOS4412_GPD0(0), 1);
	gpio_direction_output(EXYNOS4412_GPD0(0), 0);

	gpio_set_pull(EXYNOS4412_GPX3(5), GPIO_PULL_UP);
	gpio_set_cfg(EXYNOS4412_GPX3(5), 1);
	gpio_direction_output(EXYNOS4412_GPX3(5), 1);
}

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_exynos4412_pdata_t * pdat = (struct fb_exynos4412_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_exynos4412_pdata_t * pdat = (struct fb_exynos4412_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

static struct surface_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_exynos4412_pdata_t * pdat = (struct fb_exynos4412_pdata_t *)fb->priv;
	return surface_alloc(pdat->width, pdat->height, NULL);
}

static void fb_destroy(struct framebuffer_t * fb, struct surface_t * s)
{
	surface_free(s);
}

static void fb_present(struct framebuffer_t * fb, struct surface_t * s, struct region_list_t * rl)
{
	struct fb_exynos4412_pdata_t * pdat = (struct fb_exynos4412_pdata_t *)fb->priv;

	pdat->index = (pdat->index + 1) & 0x1;
	memcpy(pdat->vram[pdat->index], s->pixels, s->pixlen);
	dma_cache_sync(pdat->vram[pdat->index], pdat->pixlen, DMA_TO_DEVICE);
	exynos4412_fb_set_buffer_address(pdat, 0, pdat->vram[pdat->index]);
}

static struct device_t * fb_exynos4412_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_exynos4412_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct fb_exynos4412_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->width = dt_read_int(n, "width", 1024);
	pdat->height = dt_read_int(n, "height", 600);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->pixlen = pdat->width * pdat->height * 4;
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->pixlen);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->pixlen);

	pdat->output = EXYNOS4412_FB_OUTPUT_RGB;
	pdat->rgb_mode = EXYNOS4412_FB_MODE_RGB_P;
	pdat->bpp_mode = EXYNOS4412_FB_BPP_MODE_32BPP;
	pdat->swap = EXYNOS4412_FB_SWAP_WORD;

	pdat->timing.pixel_clock_hz = dt_read_long(n, "clock-frequency", 52000000);
	pdat->timing.h_front_porch = dt_read_int(n, "hfront-porch", 160);
	pdat->timing.h_back_porch = dt_read_int(n, "hback-porch", 140);
	pdat->timing.h_sync_len = dt_read_int(n, "hsync-len", 20);
	pdat->timing.v_front_porch = dt_read_int(n, "vfront-porch", 12);
	pdat->timing.v_back_porch = dt_read_int(n, "vback-porch", 20);
	pdat->timing.v_sync_len = dt_read_int(n, "vsync-len", 3);
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
	fb_exynos4412_init(pdat);

	if(!(dev = register_framebuffer(fb, drv)))
	{
		clk_disable(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	return dev;
}

static void fb_exynos4412_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_exynos4412_pdata_t * pdat = (struct fb_exynos4412_pdata_t *)fb->priv;

	if(fb)
	{
		unregister_framebuffer(fb);
		clk_disable(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);
		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_exynos4412_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_exynos4412_pdata_t * pdat = (struct fb_exynos4412_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_exynos4412_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_exynos4412_pdata_t * pdat = (struct fb_exynos4412_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_exynos4412 = {
	.name		= "fb-exynos4412",
	.probe		= fb_exynos4412_probe,
	.remove		= fb_exynos4412_remove,
	.suspend	= fb_exynos4412_suspend,
	.resume		= fb_exynos4412_resume,
};

static __init void fb_exynos4412_driver_init(void)
{
	register_driver(&fb_exynos4412);
}

static __exit void fb_exynos4412_driver_exit(void)
{
	unregister_driver(&fb_exynos4412);
}

driver_initcall(fb_exynos4412_driver_init);
driver_exitcall(fb_exynos4412_driver_exit);
