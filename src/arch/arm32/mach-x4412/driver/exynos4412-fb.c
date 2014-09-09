/*
 * driver/exynos4412-fb.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <exynos4412-fb.h>

static bool_t exynos4412_fb_set_output(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(dat->regbase + VIDCON0);
	cfg &= ~VIDCON0_VIDOUT_MASK;

	if(dat->output == EXYNOS4412_FB_OUTPUT_RGB)
		cfg |= VIDCON0_VIDOUT_RGB;
	else if(dat->output == EXYNOS4412_FB_OUTPUT_I80LDI0)
		cfg |= VIDCON0_VIDOUT_I80LDI0;
	else if(dat->output == EXYNOS4412_FB_OUTPUT_I80LDI1)
		cfg |= VIDCON0_VIDOUT_I80LDI1;
	else if(dat->output == EXYNOS4412_FB_OUTPUT_WB_RGB)
		cfg |= VIDCON0_VIDOUT_WB_RGB;
	else if(dat->output == EXYNOS4412_FB_OUTPUT_WB_I80LDI0)
		cfg |= VIDCON0_VIDOUT_WB_I80LDI0;
	else if(dat->output == EXYNOS4412_FB_OUTPUT_WB_I80LDI1)
		cfg |= VIDCON0_VIDOUT_WB_I80LDI1;
	else
		return FALSE;
	writel(dat->regbase + VIDCON0, cfg);

	cfg = readl(dat->regbase + VIDCON2);
	cfg |= 0x3 << 14;
	writel(dat->regbase + VIDCON2, cfg);

	return TRUE;
}

static bool_t exynos4412_fb_set_display_mode(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(dat->regbase + VIDCON0);
	cfg &= ~(3 << 17);
	cfg |= (dat->rgb_mode << 17);
	writel(dat->regbase + VIDCON0, cfg);

	return TRUE;
}

static bool_t exynos4412_fb_display_on(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(dat->regbase + VIDCON0);
	cfg |= 0x3 << 0;
	writel(dat->regbase + VIDCON0, cfg);

	return TRUE;
}

static bool_t exynos4412_fb_display_off(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(dat->regbase + VIDCON0);
	cfg &= ~(1 << 1);
	writel(dat->regbase + VIDCON0, cfg);

	cfg &= ~(1 << 0);
	writel(dat->regbase + VIDCON0, cfg);

	return TRUE;
}

static bool_t exynos4412_fb_set_clock(struct exynos4412_fb_data_t * dat)
{
	u64_t fimd, pixel_clock;
	u32_t div;
	u32_t cfg;

	fimd = clk_get_rate("GATE-FIMD");
	if(!fimd)
		return FALSE;

	pixel_clock = ( dat->freq * (dat->timing.h_fp + dat->timing.h_bp + dat->timing.h_sw + dat->width) *
			(dat->timing.v_fp + dat->timing.v_bp + dat->timing.v_sw + dat->height) );

	div = (u32_t)(fimd / pixel_clock);
	if((fimd % pixel_clock) > 0)
		div++;

	cfg = readl(dat->regbase + VIDCON0);
	cfg &= ~( (1 << 16) | (1 << 5) );
	cfg |= VIDCON0_CLKVAL_F(div - 1);
	writel(dat->regbase + VIDCON0, cfg);

	return TRUE;
}

static bool_t exynos4412_fb_set_polarity(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg = 0;

	if(dat->polarity.rise_vclk)
		cfg |= (1 << 7);

	if(dat->polarity.inv_hsync)
		cfg |= (1 << 6);

	if(dat->polarity.inv_vsync)
		cfg |= (1 << 5);

	if(dat->polarity.inv_vden)
		cfg |= (1 << 4);

	writel(dat->regbase + VIDCON1, cfg);
	return TRUE;
}

static bool_t exynos4412_fb_set_timing(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg;

	cfg = 0;
	cfg |= VIDTCON0_VBPDE(dat->timing.v_bpe - 1);
	cfg |= VIDTCON0_VBPD(dat->timing.v_bp - 1);
	cfg |= VIDTCON0_VFPD(dat->timing.v_fp - 1);
	cfg |= VIDTCON0_VSPW(dat->timing.v_sw - 1);
	writel(dat->regbase + VIDTCON0, cfg);

	cfg = 0;
	cfg |= VIDTCON1_VFPDE(dat->timing.v_fpe - 1);
	cfg |= VIDTCON1_HBPD(dat->timing.h_bp - 1);
	cfg |= VIDTCON1_HFPD(dat->timing.h_fp - 1);
	cfg |= VIDTCON1_HSPW(dat->timing.h_sw - 1);
	writel(dat->regbase + VIDTCON1, cfg);

	return TRUE;
}

static bool_t exynos4412_fb_set_lcd_size(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg = 0;

	cfg |= VIDTCON2_HOZVAL(dat->width - 1);
	cfg |= VIDTCON2_LINEVAL(dat->height - 1);
	writel(dat->regbase + VIDTCON2, cfg);

	return TRUE;
}

static bool_t exynos4412_fb_set_buffer_address(struct exynos4412_fb_data_t * dat, s32_t id, void * vram)
{
	u32_t start, end;
	u32_t shw;

	start = (u32_t)(vram);
	end = (u32_t)((start + dat->width * (dat->height * dat->bytes_per_pixel)) & 0x00ffffff);

	shw = readl(dat->regbase + SHADOWCON);
	shw |= SHADOWCON_PROTECT(id);
	writel(dat->regbase + SHADOWCON, shw);

	switch(id)
	{
	case 0:
		writel(dat->regbase + VIDW00ADD0B0, start);
		writel(dat->regbase + VIDW00ADD1B0, end);
		break;

	case 1:
		writel(dat->regbase + VIDW01ADD0B0, start);
		writel(dat->regbase + VIDW01ADD1B0, end);
		break;

	case 2:
		writel(dat->regbase + VIDW02ADD0B0, start);
		writel(dat->regbase + VIDW02ADD1B0, end);
		break;

	case 3:
		writel(dat->regbase + VIDW03ADD0B0, start);
		writel(dat->regbase + VIDW03ADD1B0, end);
		break;

	case 4:
		writel(dat->regbase + VIDW04ADD0B0, start);
		writel(dat->regbase + VIDW04ADD1B0, end);
		break;

	default:
		break;
	}

	shw = readl(dat->regbase + SHADOWCON);
	shw &= ~(SHADOWCON_PROTECT(id));
	writel(dat->regbase + SHADOWCON, shw);

	return TRUE;
}

static bool_t exynos4412_fb_set_buffer_size(struct exynos4412_fb_data_t * dat, s32_t id)
{
	u32_t cfg = 0;

	cfg = VIDADDR_PAGEWIDTH(dat->width * dat->bytes_per_pixel);
	cfg |= VIDADDR_OFFSIZE(0);

	switch(id)
	{
	case 0:
		writel(dat->regbase + VIDW00ADD2, cfg);
		break;

	case 1:
		writel(dat->regbase + VIDW01ADD2, cfg);
		break;

	case 2:
		writel(dat->regbase + VIDW02ADD2, cfg);
		break;

	case 3:
		writel(dat->regbase + VIDW03ADD2, cfg);
		break;

	case 4:
		writel(dat->regbase + VIDW04ADD2, cfg);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

static bool_t exynos4412_fb_set_window_position(struct exynos4412_fb_data_t * dat, s32_t id)
{
	u32_t cfg, shw;

	shw = readl(dat->regbase + SHADOWCON);
	shw |= SHADOWCON_PROTECT(id);
	writel(dat->regbase + SHADOWCON, shw);

	switch(id)
	{
	case 0:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		writel(dat->regbase + VIDOSD0A, cfg);
		cfg = VIDOSD_RIGHT_X(dat->width - 1) | VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(dat->regbase + VIDOSD0B, cfg);
		break;

	case 1:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		writel(dat->regbase + VIDOSD1A, cfg);
		cfg = VIDOSD_RIGHT_X(dat->width - 1) | VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(dat->regbase + VIDOSD1B, cfg);
		break;

	case 2:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		writel(dat->regbase + VIDOSD2A, cfg);
		cfg = VIDOSD_RIGHT_X(dat->width - 1) | VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(dat->regbase + VIDOSD2B, cfg);
		break;

	case 3:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		writel(dat->regbase + VIDOSD3A, cfg);
		cfg = VIDOSD_RIGHT_X(dat->width - 1) | VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(dat->regbase + VIDOSD3B, cfg);
		break;

	case 4:
		cfg = VIDOSD_LEFT_X(0) | VIDOSD_TOP_Y(0);
		writel(dat->regbase + VIDOSD4A, cfg);
		cfg = VIDOSD_RIGHT_X(dat->width - 1) | VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(dat->regbase + VIDOSD4B, cfg);
		break;

	default:
		break;
	}

	shw = readl(dat->regbase + SHADOWCON);
	shw &= ~(SHADOWCON_PROTECT(id));
	writel(dat->regbase + SHADOWCON, shw);

	return TRUE;
}

static bool_t exynos4412_fb_set_window_size(struct exynos4412_fb_data_t * dat, s32_t id)
{
	u32_t cfg;

	if(id > 2)
		return FALSE;

	cfg = VIDOSD_SIZE(dat->width * dat->height);
	switch(id)
	{
	case 0:
		writel(dat->regbase + VIDOSD0C, cfg);
		break;

	case 1:
		writel(dat->regbase + VIDOSD1D, cfg);
		break;

	case 2:
		writel(dat->regbase + VIDOSD2D, cfg);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

static bool_t exynos4412_fb_window0_enable(struct exynos4412_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(dat->regbase + WINCON0);
	cfg &= ~((1 << 18) | (1 << 17) |
			(1 << 16) | (1 << 15) |
			(3 << 9) | (0xf << 2) |
			(1 << 13) | (1 << 22) |
			(1 << 1));
	cfg |= (0 << 1);
	cfg |= (0 << 13);
	cfg |= (0 << 22);
	cfg |= (1 << 0);

	if(dat->swap & EXYNOS4412_FB_SWAP_WORD)
		cfg |= (1 << 15);

	if(dat->swap & EXYNOS4412_FB_SWAP_HWORD)
		cfg |= (1 << 16);

	if(dat->swap & EXYNOS4412_FB_SWAP_BYTE)
		cfg |= (1 << 17);

	if(dat->swap & EXYNOS4412_FB_SWAP_BIT)
		cfg |= (1 << 18);

	cfg |= (dat->bpp_mode << 2);
	writel(dat->regbase + WINCON0, cfg);

	cfg = readl(dat->regbase + SHADOWCON);
	cfg |= 1 << 0;
	writel(dat->regbase + SHADOWCON, cfg);

	return TRUE;
}

static void exynos4412_fb_cfg_gpios(int base, int nr, int cfg, enum gpio_pull_t pull, enum gpio_drv_t drv)
{
	for(; nr > 0; nr--, base++)
	{
		gpio_set_cfg(base, cfg);
		gpio_set_pull(base, pull);
		gpio_set_drv(base, drv);
	}
}

static void fb_init(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct exynos4412_fb_data_t * dat = (struct exynos4412_fb_data_t *)res->data;

	/*
	 * Enable fimd clk
	 */
	clk_enable("GATE-FIMD");

	/*
	 * Initial lcd port
	 */
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF0(0), 8, 0x2, GPIO_PULL_NONE, GPIO_DRV_HIGH);
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF1(0), 8, 0x2, GPIO_PULL_NONE, GPIO_DRV_HIGH);
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF2(0), 8, 0x2, GPIO_PULL_NONE, GPIO_DRV_HIGH);
	exynos4412_fb_cfg_gpios(EXYNOS4412_GPF3(0), 4, 0x2, GPIO_PULL_NONE, GPIO_DRV_HIGH);

	/*
	 * Lcd init function
	 */
	if(dat->init)
		dat->init(dat);

	/*
	 * Display path selection
	 */
	writel(EXYNOS4412_LCDBLK_CFG, (readl(EXYNOS4412_LCDBLK_CFG) & ~(0x3<<0)) | (0x2<<0));
	writel(EXYNOS4412_LCDBLK_CFG2, (readl(EXYNOS4412_LCDBLK_CFG2) & ~(0x1<<0)) | (0x1<<0));

	/*
	 * Turn off all windows
	 */
	writel(dat->regbase + WINCON0, (readl(dat->regbase + WINCON0) & ~0x1));
	writel(dat->regbase + WINCON1, (readl(dat->regbase + WINCON1) & ~0x1));
	writel(dat->regbase + WINCON2, (readl(dat->regbase + WINCON2) & ~0x1));
	writel(dat->regbase + WINCON3, (readl(dat->regbase + WINCON3) & ~0x1));
	writel(dat->regbase + WINCON4, (readl(dat->regbase + WINCON4) & ~0x1));

	/*
	 * Turn off all windows color map
	 */
	writel(dat->regbase + WIN0MAP, (readl(dat->regbase + WIN0MAP) & ~(1<<24)));
	writel(dat->regbase + WIN1MAP, (readl(dat->regbase + WIN1MAP) & ~(1<<24)));
	writel(dat->regbase + WIN2MAP, (readl(dat->regbase + WIN2MAP) & ~(1<<24)));
	writel(dat->regbase + WIN3MAP, (readl(dat->regbase + WIN3MAP) & ~(1<<24)));
	writel(dat->regbase + WIN4MAP, (readl(dat->regbase + WIN4MAP) & ~(1<<24)));

	/*
	 * Turn off all windows color key and blending
	 */
	writel(dat->regbase + W1KEYCON0, (readl(dat->regbase + W1KEYCON0) & ~(3<<25)));
	writel(dat->regbase + W2KEYCON0, (readl(dat->regbase + W2KEYCON0) & ~(3<<25)));
	writel(dat->regbase + W3KEYCON0, (readl(dat->regbase + W3KEYCON0) & ~(3<<25)));
	writel(dat->regbase + W4KEYCON0, (readl(dat->regbase + W4KEYCON0) & ~(3<<25)));

	/*
	 * Initial lcd controller
	 */
	exynos4412_fb_set_output(dat);
	exynos4412_fb_set_display_mode(dat);
	exynos4412_fb_display_off(dat);
	exynos4412_fb_set_polarity(dat);
	exynos4412_fb_set_timing(dat);
	exynos4412_fb_set_lcd_size(dat);
	exynos4412_fb_set_clock(dat);

	/*
	 * Set lcd video buffer
	 */
	exynos4412_fb_set_buffer_size(dat, 0);
	exynos4412_fb_set_window_position(dat, 0);
	exynos4412_fb_set_window_size(dat, 0);

	/*
	 * Enable window 0 for main display area
	 */
	exynos4412_fb_window0_enable(dat);

	/*
	 * Display on
	 */
	exynos4412_fb_display_on(dat);

	/*
	 * Wait a moment
	 */
	mdelay(100);
}

static void fb_exit(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct exynos4412_fb_data_t * dat = (struct exynos4412_fb_data_t *)res->data;

	if(dat->exit)
		dat->exit(dat);

	exynos4412_fb_display_off(dat);
	clk_disable("GATE-FIMD");
}

static int fb_ioctl(struct fb_t * fb, int cmd, void * arg)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct exynos4412_fb_data_t * dat = (struct exynos4412_fb_data_t *)res->data;
	struct screen_info_t * info;
	int * brightness;

	switch(cmd)
	{
	case IOCTL_FB_GET_SCREEN_INFORMATION:
		info = (struct screen_info_t *)arg;
		info->width = dat->width;
		info->height = dat->height;
		info->xdpi = dat->xdpi;
		info->ydpi = dat->ydpi;
		info->bpp = dat->bits_per_pixel;
		return 0;

	case IOCTL_FB_SET_BACKLIGHT_BRIGHTNESS:
		brightness = (int *)arg;
		if(dat->set_backlight)
			dat->set_backlight(dat, *brightness);
		return 0;

	case IOCTL_FB_GET_BACKLIGHT_BRIGHTNESS:
		brightness = (int *)arg;
		if(dat->get_backlight)
			*brightness = dat->get_backlight(dat);
		return 0;

	default:
		break;
	}

	return -1;
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct exynos4412_fb_data_t * dat = (struct exynos4412_fb_data_t *)res->data;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = dat->width * dat->height * dat->bytes_per_pixel;
	pixels = dma_alloc_coherent(pixlen);
	if(!pixels)
		return NULL;
	memset(pixels, 0, pixlen);

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = dat->width;
	render->height = dat->height;
	render->pitch = (dat->width * dat->bytes_per_pixel + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	render->clear = sw_render_clear;
	render->snapshot = sw_render_snapshot;
	render->alloc_texture = sw_render_alloc_texture;
	render->alloc_texture_similar = sw_render_alloc_texture_similar;
	render->free_texture = sw_render_free_texture;
	render->fill_texture = sw_render_fill_texture;
	render->blit_texture = sw_render_blit_texture;
	sw_render_create_data(render);

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	if(render)
	{
		sw_render_destroy_priv_data(render);
		dma_free_coherent(render->pixels, render->pixlen);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct exynos4412_fb_data_t * dat = (struct exynos4412_fb_data_t *)res->data;
	void * pixels = render->pixels;

	if(pixels)
		exynos4412_fb_set_buffer_address(dat, 0, pixels);
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static bool_t exynos4412_register_framebuffer(struct resource_t * res)
{
	struct fb_t * fb;
	char name[64];

	fb = malloc(sizeof(struct fb_t));
	if(!fb)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb->name = strdup(name);
	fb->init = fb_init,
	fb->exit = fb_exit,
	fb->ioctl = fb_ioctl,
	fb->create = fb_create,
	fb->destroy = fb_destroy,
	fb->present = fb_present,
	fb->suspend = fb_suspend,
	fb->resume = fb_resume,
	fb->priv = res;

	if(register_framebuffer(fb))
		return TRUE;

	free(fb->name);
	free(fb);
	return FALSE;
}

static bool_t exynos4412_unregister_framebuffer(struct resource_t * res)
{
	struct fb_t * fb;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb = search_framebuffer(name);
	if(!fb)
		return FALSE;

	if(!unregister_framebuffer(fb))
		return FALSE;

	free(fb->name);
	free(fb);
	return TRUE;
}

static __init void exynos4412_fb_init(void)
{
	resource_for_each_with_name("exynos4412-fb", exynos4412_register_framebuffer);
}

static __exit void exynos4412_fb_exit(void)
{
	resource_for_each_with_name("exynos4412-fb", exynos4412_unregister_framebuffer);
}

device_initcall(exynos4412_fb_init);
device_exitcall(exynos4412_fb_exit);
