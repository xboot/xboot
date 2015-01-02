/*
 * driver/s5pv210-fb.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <s5pv210-fb.h>

static bool_t s5pv210fb_set_output(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_VIDOUT_MASK;

	if(dat->output == S5PV210FB_OUTPUT_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_RGB;
	else if(dat->output == S5PV210FB_OUTPUT_ITU)
		cfg |= S5PV210_VIDCON0_VIDOUT_ITU;
	else if(dat->output == S5PV210FB_OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI0;
	else if(dat->output == S5PV210FB_OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI1;
	else if(dat->output == S5PV210FB_OUTPUT_WB_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_RGB;
	else if(dat->output == S5PV210FB_OUTPUT_WB_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI0;
	else if(dat->output == S5PV210FB_OUTPUT_WB_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI1;
	else
		return FALSE;
	writel(S5PV210_VIDCON0, cfg);


	cfg = readl(S5PV210_VIDCON2);
	cfg &= ~(S5PV210_VIDCON2_WB_MASK | S5PV210_VIDCON2_TVFORMATSEL_MASK | S5PV210_VIDCON2_TVFORMATSEL_YUV_MASK);

	if(dat->output == S5PV210FB_OUTPUT_RGB)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(dat->output == S5PV210FB_OUTPUT_ITU)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(dat->output == S5PV210FB_OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(dat->output == S5PV210FB_OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(dat->output == S5PV210FB_OUTPUT_WB_RGB)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(dat->output == S5PV210FB_OUTPUT_WB_I80LDI0)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(dat->output == S5PV210FB_OUTPUT_WB_I80LDI1)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else
		return FALSE;
	writel(S5PV210_VIDCON2, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_display_mode(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_PNRMODE_MASK;
	cfg |= (dat->rgb_mode << S5PV210_VIDCON0_PNRMODE_SHIFT);
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static bool_t s5pv210fb_display_on(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg |= (S5PV210_VIDCON0_ENVID_ENABLE | S5PV210_VIDCON0_ENVID_F_ENABLE);
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static bool_t s5pv210fb_display_off(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_ENVID_ENABLE;
	writel(S5PV210_VIDCON0, cfg);

	cfg &= ~S5PV210_VIDCON0_ENVID_F_ENABLE;
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_clock(struct s5pv210_fb_data_t * dat)
{
	u64_t hclk, pixel_clock;
	u32_t div;
	u32_t cfg;

	hclk = clk_get_rate("dsys-hclk");
	if(!hclk)
		return FALSE;

	pixel_clock = ( dat->freq * (dat->timing.h_fp + dat->timing.h_bp + dat->timing.h_sw + dat->width) *
			(dat->timing.v_fp + dat->timing.v_bp + dat->timing.v_sw + dat->height) );

	div = (u32_t)(hclk / pixel_clock);
	if((hclk % pixel_clock) > 0)
		div++;

	/*
	 * fixed clock source: hclk
	 */
	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~(S5PV210_VIDCON0_CLKSEL_MASK | S5PV210_VIDCON0_CLKVALUP_MASK | S5PV210_VIDCON0_VCLKEN_MASK | S5PV210_VIDCON0_CLKDIR_MASK);
	cfg |= (S5PV210_VIDCON0_CLKSEL_HCLK | S5PV210_VIDCON0_CLKVALUP_ALWAYS | S5PV210_VIDCON0_VCLKEN_NORMAL | S5PV210_VIDCON0_CLKDIR_DIVIDED);
	cfg |= S5PV210_VIDCON0_CLKVAL_F(div - 1);

	writel(S5PV210_VIDCON0, cfg);
	return TRUE;
}

static bool_t s5pv210fb_set_polarity(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg = 0;

	if(dat->polarity.rise_vclk)
		cfg |= S5PV210_VIDCON1_IVCLK_RISING_EDGE;

	if(dat->polarity.inv_hsync)
		cfg |= S5PV210_VIDCON1_IHSYNC_INVERT;

	if(dat->polarity.inv_vsync)
		cfg |= S5PV210_VIDCON1_IVSYNC_INVERT;

	if(dat->polarity.inv_vden)
		cfg |= S5PV210_VIDCON1_IVDEN_INVERT;

	writel(S5PV210_VIDCON1, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_timing(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg;

	cfg = 0;
	cfg |= S5PV210_VIDTCON0_VBPDE(dat->timing.v_bpe - 1);
	cfg |= S5PV210_VIDTCON0_VBPD(dat->timing.v_bp - 1);
	cfg |= S5PV210_VIDTCON0_VFPD(dat->timing.v_fp - 1);
	cfg |= S5PV210_VIDTCON0_VSPW(dat->timing.v_sw - 1);
	writel(S5PV210_VIDTCON0, cfg);

	cfg = 0;
	cfg |= S5PV210_VIDTCON1_VFPDE(dat->timing.v_fpe - 1);
	cfg |= S5PV210_VIDTCON1_HBPD(dat->timing.h_bp - 1);
	cfg |= S5PV210_VIDTCON1_HFPD(dat->timing.h_fp - 1);
	cfg |= S5PV210_VIDTCON1_HSPW(dat->timing.h_sw - 1);
	writel(S5PV210_VIDTCON1, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_lcd_size(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg = 0;

	cfg |= S5PV210_VIDTCON2_HOZVAL(dat->width - 1);
	cfg |= S5PV210_VIDTCON2_LINEVAL(dat->height - 1);
	writel(S5PV210_VIDTCON2, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_buffer_address(struct s5pv210_fb_data_t * dat, s32_t id, void * vram)
{
	u32_t start, end;
	u32_t shw;

	start = (u32_t)(vram);
	end = (u32_t)((start + dat->width * (dat->height * dat->bytes_per_pixel)) & 0x00ffffff);

	shw = readl(S5PV210_SHADOWCON);
	shw |= S5PV210_SHADOWCON_PROTECT(id);
	writel(S5PV210_SHADOWCON, shw);

	switch(id)
	{
	case 0:
		writel(S5PV210_VIDW00ADD0B0, start);
		writel(S5PV210_VIDW00ADD1B0, end);
		break;

	case 1:
		writel(S5PV210_VIDW01ADD0B0, start);
		writel(S5PV210_VIDW01ADD1B0, end);
		break;

	case 2:
		writel(S5PV210_VIDW02ADD0B0, start);
		writel(S5PV210_VIDW02ADD1B0, end);
		break;

	case 3:
		writel(S5PV210_VIDW03ADD0B0, start);
		writel(S5PV210_VIDW03ADD1B0, end);
		break;

	case 4:
		writel(S5PV210_VIDW04ADD0B0, start);
		writel(S5PV210_VIDW04ADD1B0, end);
		break;

	default:
		break;
	}

	shw = readl(S5PV210_SHADOWCON);
	shw &= ~(S5PV210_SHADOWCON_PROTECT(id));
	writel(S5PV210_SHADOWCON, shw);

	return TRUE;
}

static bool_t s5pv210fb_set_buffer_size(struct s5pv210_fb_data_t * dat, s32_t id)
{
	u32_t cfg = 0;

	cfg = S5PV210_VIDADDR_PAGEWIDTH(dat->width * dat->bytes_per_pixel);
	cfg |= S5PV210_VIDADDR_OFFSIZE(0);

	switch(id)
	{
	case 0:
		writel(S5PV210_VIDW00ADD2, cfg);
		break;

	case 1:
		writel(S5PV210_VIDW01ADD2, cfg);
		break;

	case 2:
		writel(S5PV210_VIDW02ADD2, cfg);
		break;

	case 3:
		writel(S5PV210_VIDW03ADD2, cfg);
		break;

	case 4:
		writel(S5PV210_VIDW04ADD2, cfg);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

static bool_t s5pv210fb_set_window_position(struct s5pv210_fb_data_t * dat, s32_t id)
{
	u32_t cfg, shw;

	shw = readl(S5PV210_SHADOWCON);
	shw |= S5PV210_SHADOWCON_PROTECT(id);
	writel(S5PV210_SHADOWCON, shw);

	switch(id)
	{
	case 0:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD0A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(dat->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(S5PV210_VIDOSD0B, cfg);
		break;

	case 1:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD1A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(dat->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(S5PV210_VIDOSD1B, cfg);
		break;

	case 2:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD2A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(dat->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(S5PV210_VIDOSD2B, cfg);
		break;

	case 3:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD3A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(dat->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(S5PV210_VIDOSD3B, cfg);
		break;

	case 4:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD4A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(dat->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(dat->height - 1);
		writel(S5PV210_VIDOSD4B, cfg);
		break;

	default:
		break;
	}

	shw = readl(S5PV210_SHADOWCON);
	shw &= ~(S5PV210_SHADOWCON_PROTECT(id));
	writel(S5PV210_SHADOWCON, shw);

	return TRUE;
}

static bool_t s5pv210fb_set_window_size(struct s5pv210_fb_data_t * dat, s32_t id)
{
	u32_t cfg;

	if(id > 2)
		return FALSE;

	cfg = S5PV210_VIDOSD_SIZE(dat->width * dat->height);

	switch(id)
	{
	case 0:
		writel(S5PV210_VIDOSD0C, cfg);
		break;

	case 1:
		writel(S5PV210_VIDOSD1D, cfg);
		break;

	case 2:
		writel(S5PV210_VIDOSD2D, cfg);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

static bool_t s5pv210fb_window2_enable(struct s5pv210_fb_data_t * dat)
{
	u32_t cfg;

	/*
	 * window control
	 */
	cfg = readl(S5PV210_WINCON2);

	cfg &= ~(S5PV210_WINCON_BITSWP_ENABLE | S5PV210_WINCON_BYTESWP_ENABLE |
				S5PV210_WINCON_HAWSWP_ENABLE | S5PV210_WINCON_WSWP_ENABLE |
				S5PV210_WINCON_BURSTLEN_MASK | S5PV210_WINCON_BPPMODE_MASK |
				S5PV210_WINCON_INRGB_MASK | S5PV210_WINCON_DATAPATH_MASK |
				S5PV210_WINCON_ALPHA_SEL_MASK);

	cfg |= S5PV210_WINCON_ALPHA0_SEL;
	cfg |= S5PV210_WINCON_INRGB_RGB;
	cfg |= S5PV210_WINCON_DATAPATH_DMA;
	cfg |= S5PV210_WINCON_ENWIN_ENABLE;

	if(dat->swap & S5PV210FB_SWAP_WORD)
		cfg |= S5PV210_WINCON_WSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_WSWP_DISABLE;

	if(dat->swap & S5PV210FB_SWAP_HWORD)
		cfg |= S5PV210_WINCON_HAWSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_HAWSWP_DISABLE;

	if(dat->swap & S5PV210FB_SWAP_BYTE)
		cfg |= S5PV210_WINCON_BYTESWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_BYTESWP_DISABLE;

	if(dat->swap & S5PV210FB_SWAP_BIT)
		cfg |= S5PV210_WINCON_BITSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_BITSWP_DISABLE;

	cfg |= (dat->bpp_mode << S5PV210_WINCON_BPPMODE_SHIFT);

	writel(S5PV210_WINCON2, cfg);

	/*
	 * hardware version = 0x62
	 */
	cfg = readl(S5PV210_SHADOWCON);
	cfg |= 1 << 2;
	writel(S5PV210_SHADOWCON, cfg);

	return TRUE;
}

static void fb_init(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5pv210_fb_data_t * dat = (struct s5pv210_fb_data_t *)res->data;

	/*
	 * Enable fimd clk
	 */
	clk_enable("dsys-hclk");

	/*
	 * Initial lcd port
	 */
	writel(S5PV210_GPF0_BASE + S5PV210_GPIO_CON, 0x22222222);
	writel(S5PV210_GPF0_BASE + S5PV210_GPIO_DRV, 0xffffffff);
	writel(S5PV210_GPF0_BASE + S5PV210_GPIO_PUD, 0x0);
	writel(S5PV210_GPF1_BASE + S5PV210_GPIO_CON, 0x22222222);
	writel(S5PV210_GPF1_BASE + S5PV210_GPIO_DRV, 0xffffffff);
	writel(S5PV210_GPF1_BASE + S5PV210_GPIO_PUD, 0x0);
	writel(S5PV210_GPF2_BASE + S5PV210_GPIO_CON, 0x22222222);
	writel(S5PV210_GPF2_BASE + S5PV210_GPIO_DRV, 0xffffffff);
	writel(S5PV210_GPF2_BASE + S5PV210_GPIO_PUD, 0x0);
	writel(S5PV210_GPF3_BASE + S5PV210_GPIO_CON, (readl(S5PV210_GPF3_BASE + S5PV210_GPIO_CON) & ~(0xffff<<0)) | (0x2222<<0));
	writel(S5PV210_GPF3_BASE + S5PV210_GPIO_DRV, (readl(S5PV210_GPF3_BASE + S5PV210_GPIO_DRV) & ~(0xff<<0)) | (0xff<<0));
	writel(S5PV210_GPF3_BASE + S5PV210_GPIO_PUD, (readl(S5PV210_GPF3_BASE + S5PV210_GPIO_PUD) & ~(0xff<<0)) | (0x00<<0));

	/*
	 * Lcd init function
	 */
	if(dat->init)
		dat->init(dat);

	/*
	 * Display path selection
	 */
	writel(S5PV210_DISPLAY_CONTROL, (readl(S5PV210_DISPLAY_CONTROL) & ~(0x3<<0)) | (0x2<<0));

	/*
	 * Turn off all windows
	 */
	writel(S5PV210_WINCON0, (readl(S5PV210_WINCON0) & ~0x1));
	writel(S5PV210_WINCON1, (readl(S5PV210_WINCON1) & ~0x1));
	writel(S5PV210_WINCON2, (readl(S5PV210_WINCON2) & ~0x1));
	writel(S5PV210_WINCON3, (readl(S5PV210_WINCON3) & ~0x1));
	writel(S5PV210_WINCON4, (readl(S5PV210_WINCON4) & ~0x1));

	/*
	 * Turn off all windows color map
	 */
	writel(S5PV210_WIN0MAP, (readl(S5PV210_WIN0MAP) & ~(1<<24)));
	writel(S5PV210_WIN1MAP, (readl(S5PV210_WIN1MAP) & ~(1<<24)));
	writel(S5PV210_WIN2MAP, (readl(S5PV210_WIN2MAP) & ~(1<<24)));
	writel(S5PV210_WIN3MAP, (readl(S5PV210_WIN3MAP) & ~(1<<24)));
	writel(S5PV210_WIN4MAP, (readl(S5PV210_WIN4MAP) & ~(1<<24)));

	/*
	 * Turn off all windows color key
	 */
	writel(S5PV210_W1KEYCON0, (readl(S5PV210_W1KEYCON0) & ~(3<<25)));
	writel(S5PV210_W2KEYCON0, (readl(S5PV210_W2KEYCON0) & ~(3<<25)));
	writel(S5PV210_W3KEYCON0, (readl(S5PV210_W3KEYCON0) & ~(3<<25)));
	writel(S5PV210_W4KEYCON0, (readl(S5PV210_W4KEYCON0) & ~(3<<25)));

	/*
	 * Initial lcd controller
	 */
	s5pv210fb_set_output(dat);
	s5pv210fb_set_display_mode(dat);
	s5pv210fb_display_off(dat);
	s5pv210fb_set_polarity(dat);
	s5pv210fb_set_timing(dat);
	s5pv210fb_set_lcd_size(dat);
	s5pv210fb_set_clock(dat);

	/*
	 * Set lcd video buffer
	 */
	s5pv210fb_set_buffer_size(dat, 2);
	s5pv210fb_set_window_position(dat, 2);
	s5pv210fb_set_window_size(dat, 2);

	/*
	 * Enable window 2 for main display area
	 */
	s5pv210fb_window2_enable(dat);

	/*
	 * Display on
	 */
	s5pv210fb_display_on(dat);

	/*
	 * Wait a moment
	 */
	mdelay(100);
}

static void fb_exit(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5pv210_fb_data_t * dat = (struct s5pv210_fb_data_t *)res->data;

	if(dat->exit)
		dat->exit(dat);

	s5pv210fb_display_off(dat);
	clk_disable("dsys-hclk");
}

static int fb_ioctl(struct fb_t * fb, int cmd, void * arg)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5pv210_fb_data_t * dat = (struct s5pv210_fb_data_t *)res->data;
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
	struct s5pv210_fb_data_t * dat = (struct s5pv210_fb_data_t *)res->data;
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
		sw_render_destroy_data(render);
		dma_free_coherent(render->pixels, render->pixlen);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5pv210_fb_data_t * dat = (struct s5pv210_fb_data_t *)res->data;
	void * pixels = render->pixels;

	if(pixels)
		s5pv210fb_set_buffer_address(dat, 2, pixels);
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static bool_t s5pv210_register_framebuffer(struct resource_t * res)
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

static bool_t s5pv210_unregister_framebuffer(struct resource_t * res)
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

static __init void s5pv210_fb_init(void)
{
	resource_for_each_with_name("s5pv210-fb", s5pv210_register_framebuffer);
}

static __exit void s5pv210_fb_exit(void)
{
	resource_for_each_with_name("s5pv210-fb", s5pv210_unregister_framebuffer);
}

device_initcall(s5pv210_fb_init);
device_exitcall(s5pv210_fb_exit);
