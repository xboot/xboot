/*
 * driver/s5pv210-fb.c
 *
 * s5pv210 framebuffer drivers.
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-others.h>
#include <s5pv210/reg-lcd.h>
#include <s5pv210-fb.h>

static bool_t s5pv210fb_set_output(struct s5pv210fb_lcd * lcd)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_VIDOUT_MASK;

	if(lcd->output == S5PV210FB_OUTPUT_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_RGB;
	else if(lcd->output == S5PV210FB_OUTPUT_ITU)
		cfg |= S5PV210_VIDCON0_VIDOUT_ITU;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI0;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI1;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_RGB;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI0;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI1;
	else
		return FALSE;
	writel(S5PV210_VIDCON0, cfg);


	cfg = readl(S5PV210_VIDCON2);
	cfg &= ~(S5PV210_VIDCON2_WB_MASK | S5PV210_VIDCON2_TVFORMATSEL_MASK | S5PV210_VIDCON2_TVFORMATSEL_YUV_MASK);

	if(lcd->output == S5PV210FB_OUTPUT_RGB)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_ITU)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == S5PV210FB_OUTPUT_WB_RGB)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI0)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(lcd->output == S5PV210FB_OUTPUT_WB_I80LDI1)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else
		return FALSE;
	writel(S5PV210_VIDCON2, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_display_mode(struct s5pv210fb_lcd * lcd)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_PNRMODE_MASK;
	cfg |= (lcd->rgb_mode << S5PV210_VIDCON0_PNRMODE_SHIFT);
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static bool_t s5pv210fb_display_on(struct s5pv210fb_lcd * lcd)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg |= (S5PV210_VIDCON0_ENVID_ENABLE | S5PV210_VIDCON0_ENVID_F_ENABLE);
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static bool_t s5pv210fb_display_off(struct s5pv210fb_lcd * lcd)
{
	u32_t cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_ENVID_ENABLE;
	writel(S5PV210_VIDCON0, cfg);

	cfg &= ~S5PV210_VIDCON0_ENVID_F_ENABLE;
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_clock(struct s5pv210fb_lcd * lcd)
{
	u64_t hclk, pixel_clock;
	u32_t div;
	u32_t cfg;

	/*
	 * get hclk for lcd
	 */
	if(! clk_get_rate("dsys-hclk", &hclk))
		return FALSE;

	pixel_clock = ( lcd->freq * (lcd->timing.h_fp + lcd->timing.h_bp + lcd->timing.h_sw + lcd->width) *
			(lcd->timing.v_fp + lcd->timing.v_bp + lcd->timing.v_sw + lcd->height) );

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

static bool_t s5pv210fb_set_polarity(struct s5pv210fb_lcd * lcd)
{
	u32_t cfg = 0;

	if(lcd->polarity.rise_vclk)
		cfg |= S5PV210_VIDCON1_IVCLK_RISING_EDGE;

	if(lcd->polarity.inv_hsync)
		cfg |= S5PV210_VIDCON1_IHSYNC_INVERT;

	if(lcd->polarity.inv_vsync)
		cfg |= S5PV210_VIDCON1_IVSYNC_INVERT;

	if(lcd->polarity.inv_vden)
		cfg |= S5PV210_VIDCON1_IVDEN_INVERT;

	writel(S5PV210_VIDCON1, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_timing(struct s5pv210fb_lcd * lcd)
{
	u32_t cfg;

	cfg = 0;
	cfg |= S5PV210_VIDTCON0_VBPDE(lcd->timing.v_bpe - 1);
	cfg |= S5PV210_VIDTCON0_VBPD(lcd->timing.v_bp - 1);
	cfg |= S5PV210_VIDTCON0_VFPD(lcd->timing.v_fp - 1);
	cfg |= S5PV210_VIDTCON0_VSPW(lcd->timing.v_sw - 1);
	writel(S5PV210_VIDTCON0, cfg);

	cfg = 0;
	cfg |= S5PV210_VIDTCON1_VFPDE(lcd->timing.v_fpe - 1);
	cfg |= S5PV210_VIDTCON1_HBPD(lcd->timing.h_bp - 1);
	cfg |= S5PV210_VIDTCON1_HFPD(lcd->timing.h_fp - 1);
	cfg |= S5PV210_VIDTCON1_HSPW(lcd->timing.h_sw - 1);
	writel(S5PV210_VIDTCON1, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_lcd_size(struct s5pv210fb_lcd * lcd)
{
	u32_t cfg = 0;

	cfg |= S5PV210_VIDTCON2_HOZVAL(lcd->width - 1);
	cfg |= S5PV210_VIDTCON2_LINEVAL(lcd->height - 1);
	writel(S5PV210_VIDTCON2, cfg);

	return TRUE;
}

static bool_t s5pv210fb_set_buffer_address(struct s5pv210fb_lcd * lcd, s32_t id, void * vram)
{
	u32_t start, end;
	u32_t shw;

	start = (u32_t)(vram);
	end = (u32_t)((start + lcd->width * (lcd->height * lcd->bytes_per_pixel)) & 0x00ffffff);

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

static bool_t s5pv210fb_set_buffer_size(struct s5pv210fb_lcd * lcd, s32_t id)
{
	u32_t cfg = 0;

	cfg = S5PV210_VIDADDR_PAGEWIDTH(lcd->width * lcd->bytes_per_pixel);
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

static bool_t s5pv210fb_set_window_position(struct s5pv210fb_lcd * lcd, s32_t id)
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
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(S5PV210_VIDOSD0B, cfg);
		break;

	case 1:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD1A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(S5PV210_VIDOSD1B, cfg);
		break;

	case 2:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD2A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(S5PV210_VIDOSD2B, cfg);
		break;

	case 3:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD3A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
		writel(S5PV210_VIDOSD3B, cfg);
		break;

	case 4:
		cfg = S5PV210_VIDOSD_LEFT_X(0) | S5PV210_VIDOSD_TOP_Y(0);
		writel(S5PV210_VIDOSD4A, cfg);
		cfg = S5PV210_VIDOSD_RIGHT_X(lcd->width - 1) | S5PV210_VIDOSD_BOTTOM_Y(lcd->height - 1);
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

static bool_t s5pv210fb_set_window_size(struct s5pv210fb_lcd * lcd, s32_t id)
{
	u32_t cfg;

	if(id > 2)
		return FALSE;

	cfg = S5PV210_VIDOSD_SIZE(lcd->width * lcd->height);

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

static bool_t s5pv210fb_window2_enable(struct s5pv210fb_lcd * lcd)
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

	if(lcd->swap & S5PV210FB_SWAP_WORD)
		cfg |= S5PV210_WINCON_WSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_WSWP_DISABLE;

	if(lcd->swap & S5PV210FB_SWAP_HWORD)
		cfg |= S5PV210_WINCON_HAWSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_HAWSWP_DISABLE;

	if(lcd->swap & S5PV210FB_SWAP_BYTE)
		cfg |= S5PV210_WINCON_BYTESWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_BYTESWP_DISABLE;

	if(lcd->swap & S5PV210FB_SWAP_BIT)
		cfg |= S5PV210_WINCON_BITSWP_ENABLE;
	else
		cfg |= S5PV210_WINCON_BITSWP_DISABLE;

	cfg |= (lcd->bpp_mode << S5PV210_WINCON_BPPMODE_SHIFT);

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
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);

	/*
	 * initial lcd port
	 */
	writel(S5PV210_GPF0CON, 0x22222222);
	writel(S5PV210_GPF0DRV, 0xffffffff);
	writel(S5PV210_GPF0PUD, 0x0);
	writel(S5PV210_GPF1CON, 0x22222222);
	writel(S5PV210_GPF1DRV, 0xffffffff);
	writel(S5PV210_GPF1PUD, 0x0);
	writel(S5PV210_GPF2CON, 0x22222222);
	writel(S5PV210_GPF2DRV, 0xffffffff);
	writel(S5PV210_GPF2PUD, 0x0);
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0xffff<<0)) | (0x2222<<0));
	writel(S5PV210_GPF3DRV, (readl(S5PV210_GPF3DRV) & ~(0xff<<0)) | (0xff<<0));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0xff<<0)) | (0x00<<0));

	/*
	 * lcd init function
	 */
	if(lcd->init)
		lcd->init();

	/*
	 * display path selection
	 */
	writel(S5PV210_DISPLAY_CONTROL, (readl(S5PV210_DISPLAY_CONTROL) & ~(0x3<<0)) | (0x2<<0));

	/*
	 * turn all windows off
	 */
	writel(S5PV210_WINCON0, (readl(S5PV210_WINCON0) & ~0x1));
	writel(S5PV210_WINCON1, (readl(S5PV210_WINCON1) & ~0x1));
	writel(S5PV210_WINCON2, (readl(S5PV210_WINCON2) & ~0x1));
	writel(S5PV210_WINCON3, (readl(S5PV210_WINCON3) & ~0x1));
	writel(S5PV210_WINCON4, (readl(S5PV210_WINCON4) & ~0x1));

	/*
	 * turn all windows color map off
	 */
	writel(S5PV210_WIN0MAP, (readl(S5PV210_WIN0MAP) & ~(1<<24)));
	writel(S5PV210_WIN1MAP, (readl(S5PV210_WIN1MAP) & ~(1<<24)));
	writel(S5PV210_WIN2MAP, (readl(S5PV210_WIN2MAP) & ~(1<<24)));
	writel(S5PV210_WIN3MAP, (readl(S5PV210_WIN3MAP) & ~(1<<24)));
	writel(S5PV210_WIN4MAP, (readl(S5PV210_WIN4MAP) & ~(1<<24)));

	/*
	 * turn all windows color key off
	 */
	writel(S5PV210_W1KEYCON0, (readl(S5PV210_W1KEYCON0) & ~(3<<25)));
	writel(S5PV210_W2KEYCON0, (readl(S5PV210_W2KEYCON0) & ~(3<<25)));
	writel(S5PV210_W3KEYCON0, (readl(S5PV210_W3KEYCON0) & ~(3<<25)));
	writel(S5PV210_W4KEYCON0, (readl(S5PV210_W4KEYCON0) & ~(3<<25)));

	/*
	 * initial lcd controller
	 */
	s5pv210fb_set_output(lcd);
	s5pv210fb_set_display_mode(lcd);
	s5pv210fb_display_off(lcd);
	s5pv210fb_set_polarity(lcd);
	s5pv210fb_set_timing(lcd);
	s5pv210fb_set_lcd_size(lcd);
	s5pv210fb_set_clock(lcd);

	/*
	 * set lcd video buffer
	 */
	s5pv210fb_set_buffer_size(lcd, 2);
	s5pv210fb_set_window_position(lcd, 2);
	s5pv210fb_set_window_size(lcd, 2);

	/*
	 * enable window 2 for main display area
	 */
	s5pv210fb_window2_enable(lcd);

	/*
	 * display on
	 */
	s5pv210fb_display_on(lcd);

	/*
	 * wait a moment
	 */
	mdelay(100);
}

static void fb_exit(struct fb_t * fb)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);

	if(lcd->exit)
		lcd->exit();

	s5pv210fb_display_off(lcd);
}

static int fb_xcursor(struct fb_t * fb, int ox)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);

	if(ox == 0)
		return lcd->xpos;

	lcd->xpos = lcd->xpos + ox;
	if(lcd->xpos < 0)
		lcd->xpos = 0;
	if(lcd->xpos > lcd->width - 1)
		lcd->xpos = lcd->width - 1;

	return lcd->xpos;
}

static int fb_ycursor(struct fb_t * fb, int oy)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);

	if(oy == 0)
		return lcd->ypos;

	lcd->ypos = lcd->ypos + oy;
	if(lcd->ypos < 0)
		lcd->ypos = 0;
	if(lcd->ypos > lcd->height - 1)
		lcd->ypos = lcd->height - 1;

	return lcd->ypos;
}

static int fb_backlight(struct fb_t * fb, int brightness)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);

	if(lcd->backlight)
		return lcd->backlight(brightness);
	return 0;
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);
	struct render_t * render;
	void * pixels;
	size_t size;

	size = lcd->width * lcd->height * lcd->bytes_per_pixel;
	pixels = memalign(4, size);
	if(!pixels)
		return NULL;
	memset(pixels, 0, size);

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = lcd->width;
	render->height = lcd->height;
	render->pitch = (lcd->width * lcd->bytes_per_pixel + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;

	render->clear = sw_render_clear;
	render->snapshot = sw_render_snapshot;
	render->alloc_texture = sw_render_alloc_texture;
	render->alloc_texture_similar = sw_render_alloc_texture_similar;
	render->free_texture = sw_render_free_texture;
	render->fill_texture = sw_render_fill_texture;
	render->blit_texture = sw_render_blit_texture;
	sw_render_create_priv_data(render);

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	if(render)
	{
		sw_render_destroy_priv_data(render);
		free(render->pixels);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);
	void * pixels = render->pixels;

	if(pixels)
		s5pv210fb_set_buffer_address(lcd, 2, pixels);
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static struct fb_t s5pv210_fb = {
	.name		= "fb0",
	.init		= fb_init,
	.exit		= fb_exit,
	.xcursor	= fb_xcursor,
	.ycursor	= fb_ycursor,
	.backlight	= fb_backlight,
	.create		= fb_create,
	.destroy	= fb_destroy,
	.present	= fb_present,
	.suspend	= fb_suspend,
	.resume		= fb_resume,
};

static __init void s5pv210_fb_init(void)
{
	struct s5pv210fb_lcd * lcd;

	s5pv210_fb.priv = resource_get_data(s5pv210_fb.name);
	lcd = (struct s5pv210fb_lcd *)(s5pv210_fb.priv);

	if(! s5pv210_fb.priv)
	{
		LOG("can't get the resource of \'%s\'", s5pv210_fb.name);
		return;
	}

	if(! clk_get_rate("dsys-hclk", 0))
	{
		LOG("can't get the clock of \'dsys-hclk\'");
		return;
	}

	if( (lcd->bits_per_pixel != 16) && (lcd->bits_per_pixel != 24) && (lcd->bits_per_pixel != 32) )
		return;

	if(! register_framebuffer(&s5pv210_fb))
		LOG("failed to register framebuffer driver '%s'", s5pv210_fb.name);
}

static __exit void s5pv210_fb_exit(void)
{
	if(!unregister_framebuffer(&s5pv210_fb))
		LOG("failed to unregister framebuffer driver '%s'", s5pv210_fb.name);
}

device_initcall(s5pv210_fb_init);
device_exitcall(s5pv210_fb_exit);
