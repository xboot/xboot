/*
 * driver/s5pv210-fb.c
 *
 * s5pv210 framebuffer drivers.
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <div64.h>
#include <io.h>
#include <time/delay.h>
#include <xboot/log.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <fb/fbsoft.h>
#include <fb/fb.h>
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-others.h>
#include <s5pv210/reg-lcd.h>
#include <s5pv210-fb.h>

static x_bool s5pv210fb_set_output(struct s5pv210fb_lcd * lcd)
{
	x_u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_VIDOUT_MASK;

	if(lcd->output == OUTPUT_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_RGB;
	else if(lcd->output == OUTPUT_ITU)
		cfg |= S5PV210_VIDCON0_VIDOUT_ITU;
	else if(lcd->output == OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI0;
	else if(lcd->output == OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_I80LDI1;
	else if(lcd->output == OUTPUT_WB_RGB)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_RGB;
	else if(lcd->output == OUTPUT_WB_I80LDI0)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI0;
	else if(lcd->output == OUTPUT_WB_I80LDI1)
		cfg |= S5PV210_VIDCON0_VIDOUT_WB_I80LDI1;
	else
		return FALSE;
	writel(S5PV210_VIDCON0, cfg);


	cfg = readl(S5PV210_VIDCON2);
	cfg &= ~(S5PV210_VIDCON2_WB_MASK | S5PV210_VIDCON2_TVFORMATSEL_MASK | S5PV210_VIDCON2_TVFORMATSEL_YUV_MASK);

	if(lcd->output == OUTPUT_RGB)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == OUTPUT_ITU)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == OUTPUT_I80LDI0)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == OUTPUT_I80LDI1)
		cfg |= S5PV210_VIDCON2_WB_DISABLE;
	else if(lcd->output == OUTPUT_WB_RGB)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(lcd->output == OUTPUT_WB_I80LDI0)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else if(lcd->output == OUTPUT_WB_I80LDI1)
		cfg |= (S5PV210_VIDCON2_WB_ENABLE | S5PV210_VIDCON2_TVFORMATSEL_SW | S5PV210_VIDCON2_TVFORMATSEL_YUV444);
	else
		return FALSE;
	writel(S5PV210_VIDCON2, cfg);

	return TRUE;
}

static x_bool s5pv210fb_set_display_mode(struct s5pv210fb_lcd * lcd)
{
	x_u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_PNRMODE_MASK;
	cfg |= (lcd->rgb_mode << S5PV210_VIDCON0_PNRMODE_SHIFT);
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static x_bool s5pv210fb_display_on(struct s5pv210fb_lcd * lcd)
{
	x_u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg |= (S5PV210_VIDCON0_ENVID_ENABLE | S5PV210_VIDCON0_ENVID_F_ENABLE);
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static x_bool s5pv210fb_display_off(struct s5pv210fb_lcd * lcd)
{
	x_u32 cfg;

	cfg = readl(S5PV210_VIDCON0);
	cfg &= ~S5PV210_VIDCON0_ENVID_ENABLE;
	writel(S5PV210_VIDCON0, cfg);

	cfg &= ~S5PV210_VIDCON0_ENVID_F_ENABLE;
	writel(S5PV210_VIDCON0, cfg);

	return TRUE;
}

static x_bool s5pv210fb_set_clock(struct s5pv210fb_lcd * lcd)
{
	//XXX
	return FALSE;
}

static x_bool s5pv210fb_set_polarity(struct s5pv210fb_lcd * lcd)
{
	x_u32 cfg = 0;

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

static x_bool s5pv210fb_set_timing(struct s5pv210fb_lcd * lcd)
{
	x_u32 cfg;

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

static x_bool s5pv210fb_set_lcd_size(struct s5pv210fb_lcd * lcd)
{
	x_u32 cfg = 0;

	cfg |= S5PV210_VIDTCON2_HOZVAL(lcd->width - 1);
	cfg |= S5PV210_VIDTCON2_LINEVAL(lcd->height - 1);
	writel(S5PV210_VIDTCON2, cfg);

	return TRUE;
}

static x_bool s5pv210fb_window_on(struct s5pv210fb_lcd * lcd, x_s32 id)
{
	x_u32 cfg;

	switch(id)
	{
	case 0:
		cfg = readl(S5PV210_WINCON0);
		cfg |= S5PV210_WINCON_ENWIN_ENABLE;
		writel(S5PV210_WINCON0, cfg);
		break;

	case 1:
		cfg = readl(S5PV210_WINCON1);
		cfg |= S5PV210_WINCON_ENWIN_ENABLE;
		writel(S5PV210_WINCON1, cfg);
		break;

	case 2:
		cfg = readl(S5PV210_WINCON2);
		cfg |= S5PV210_WINCON_ENWIN_ENABLE;
		writel(S5PV210_WINCON2, cfg);
		break;

	case 3:
		cfg = readl(S5PV210_WINCON3);
		cfg |= S5PV210_WINCON_ENWIN_ENABLE;
		writel(S5PV210_WINCON3, cfg);
		break;

	case 4:
		cfg = readl(S5PV210_WINCON4);
		cfg |= S5PV210_WINCON_ENWIN_ENABLE;
		writel(S5PV210_WINCON4, cfg);
		break;

	default:
		return FALSE;
	}

	/* hardware version = 0x62 */
	cfg = readl(S5PV210_SHADOWCON);
	cfg |= 1 << id;
	writel(S5PV210_SHADOWCON, cfg);

	return TRUE;
}

static x_bool s5pv210fb_window_off(struct s5pv210fb_lcd * lcd, x_s32 id)
{
	x_u32 cfg;

	switch(id)
	{
	case 0:
		cfg = readl(S5PV210_WINCON0);
		cfg &= ~(S5PV210_WINCON_ENWIN_ENABLE | S5PV210_WINCON_DATAPATH_MASK);
		cfg |= S5PV210_WINCON_DATAPATH_DMA;
		writel(S5PV210_WINCON0, cfg);
		break;

	case 1:
		cfg = readl(S5PV210_WINCON1);
		cfg &= ~(S5PV210_WINCON_ENWIN_ENABLE | S5PV210_WINCON_DATAPATH_MASK);
		cfg |= S5PV210_WINCON_DATAPATH_DMA;
		writel(S5PV210_WINCON1, cfg);
		break;

	case 2:
		cfg = readl(S5PV210_WINCON2);
		cfg &= ~(S5PV210_WINCON_ENWIN_ENABLE | S5PV210_WINCON_DATAPATH_MASK);
		cfg |= S5PV210_WINCON_DATAPATH_DMA;
		writel(S5PV210_WINCON2, cfg);
		break;

	case 3:
		cfg = readl(S5PV210_WINCON3);
		cfg &= ~(S5PV210_WINCON_ENWIN_ENABLE | S5PV210_WINCON_DATAPATH_MASK);
		cfg |= S5PV210_WINCON_DATAPATH_DMA;
		writel(S5PV210_WINCON3, cfg);
		break;

	case 4:
		cfg = readl(S5PV210_WINCON4);
		cfg &= ~(S5PV210_WINCON_ENWIN_ENABLE | S5PV210_WINCON_DATAPATH_MASK);
		cfg |= S5PV210_WINCON_DATAPATH_DMA;
		writel(S5PV210_WINCON4, cfg);
		break;

	default:
		return FALSE;
	}

	/* hardware version = 0x62 */
	cfg = readl(S5PV210_SHADOWCON);
	cfg &= ~(1 << id);
	writel(S5PV210_SHADOWCON, cfg);

	return TRUE;
}

static x_bool s5pv210fb_win_map_on(struct s5pv210fb_lcd * lcd, x_s32 id, x_s32 color)
{
	x_u32 cfg = 0;

	cfg |= S5PV210_WINMAP_ENABLE;
	cfg |= S5PV210_WINMAP_COLOR(color);

	switch(id)
	{
	case 0:
		writel(S5PV210_WIN0MAP, cfg);
		break;

	case 1:
		writel(S5PV210_WIN1MAP, cfg);
		break;

	case 2:
		writel(S5PV210_WIN2MAP, cfg);
		break;

	case 3:
		writel(S5PV210_WIN3MAP, cfg);
		break;

	case 4:
		writel(S5PV210_WIN4MAP, cfg);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

static x_bool s5pv210fb_win_map_off(struct s5pv210fb_lcd * lcd, x_s32 id)
{
	switch(id)
	{
	case 0:
		writel(S5PV210_WIN0MAP, 0);
		break;

	case 1:
		writel(S5PV210_WIN1MAP, 0);
		break;

	case 2:
		writel(S5PV210_WIN2MAP, 0);
		break;

	case 3:
		writel(S5PV210_WIN3MAP, 0);
		break;

	case 4:
		writel(S5PV210_WIN4MAP, 0);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

static x_bool s5pv210fb_set_buffer_address(struct s5pv210fb_lcd * lcd, x_s32 id)
{
	x_u32 start, end;
	x_u32 shw;

	start = (x_u32)(lcd->vram);
	end = (x_u32)((start + lcd->width * (lcd->height * lcd->bpp / 8)) & 0x00ffffff);

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

static x_bool s5pv210fb_set_buffer_size(struct s5pv210fb_lcd * lcd, x_s32 id)
{
	x_u32 cfg = 0;

	cfg = S5PV210_VIDADDR_PAGEWIDTH(lcd->width * lcd->bpp / 8);
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

static x_bool s5pv210fb_set_window_position(struct s5pv210fb_lcd * lcd, x_s32 id)
{
	x_u32 cfg, shw;

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

static x_bool s5pv210fb_set_window_size(struct s5pv210fb_lcd * lcd, x_s32 id)
{
	x_u32 cfg;

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

static void fb_init(struct fb * fb)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);

	if(lcd->init)
		lcd->init();

	/* lcd port config */
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

#if 0
	/* gpf0_2 low level for enable display */
	writel(S5PV210_GPF0CON, (readl(S5PV210_GPF0CON) & ~(0x3<<8)) | (0x1<<8));
	writel(S5PV210_GPF0PUD, (readl(S5PV210_GPF0PUD) & ~(0x3<<4)) | (0x2<<4));
	writel(S5PV210_GPF0DAT, (readl(S5PV210_GPF0DAT) & ~(0x1<<2)) | (0x0<<2));
#endif

	/* display path selection */
	writel(S5PV210_DISPLAY_CONTROL, (readl(S5PV210_DISPLAY_CONTROL) & ~(0x3<<0)) | (0x2<<0));

	/* turn all windows off */
	writel(S5PV210_WINCON0, (readl(S5PV210_WINCON0) & ~0x1));
	writel(S5PV210_WINCON1, (readl(S5PV210_WINCON1) & ~0x1));
	writel(S5PV210_WINCON2, (readl(S5PV210_WINCON2) & ~0x1));
	writel(S5PV210_WINCON3, (readl(S5PV210_WINCON3) & ~0x1));
	writel(S5PV210_WINCON4, (readl(S5PV210_WINCON4) & ~0x1));

	/* turn all windows color map off */
	writel(S5PV210_WIN0MAP, (readl(S5PV210_WIN0MAP) & ~(1<<24)));
	writel(S5PV210_WIN1MAP, (readl(S5PV210_WIN1MAP) & ~(1<<24)));
	writel(S5PV210_WIN2MAP, (readl(S5PV210_WIN2MAP) & ~(1<<24)));
	writel(S5PV210_WIN3MAP, (readl(S5PV210_WIN3MAP) & ~(1<<24)));
	writel(S5PV210_WIN4MAP, (readl(S5PV210_WIN4MAP) & ~(1<<24)));

	/* turn all windows color key off */
	writel(S5PV210_W1KEYCON0, (readl(S5PV210_W1KEYCON0) & ~(3<<25)));
	writel(S5PV210_W2KEYCON0, (readl(S5PV210_W2KEYCON0) & ~(3<<25)));
	writel(S5PV210_W3KEYCON0, (readl(S5PV210_W3KEYCON0) & ~(3<<25)));
	writel(S5PV210_W4KEYCON0, (readl(S5PV210_W4KEYCON0) & ~(3<<25)));

	s5pv210fb_set_output(lcd);
	s5pv210fb_set_display_mode(lcd);
	s5pv210fb_display_off(lcd);
	s5pv210fb_set_polarity(lcd);
	s5pv210fb_set_timing(lcd);
	s5pv210fb_set_lcd_size(lcd);

	s5pv210fb_set_clock(lcd);

/*	s5pv210fb_window_off(lcd, 0);
	s5pv210fb_window_off(lcd, 1);
	s5pv210fb_window_off(lcd, 2);
	s5pv210fb_window_off(lcd, 3);
	s5pv210fb_window_off(lcd, 4);

	s5pv210fb_win_map_off(lcd, 0);
	s5pv210fb_win_map_off(lcd, 1);
	s5pv210fb_win_map_off(lcd, 2);
	s5pv210fb_win_map_off(lcd, 3);
	s5pv210fb_win_map_off(lcd, 4);*/

	s5pv210fb_set_buffer_address(lcd, 0);
	s5pv210fb_set_buffer_size(lcd, 0);
	s5pv210fb_set_window_position(lcd, 0);
	s5pv210fb_set_window_size(lcd, 0);

	writel(S5PV210_WINCON0, (readl(S5PV210_WINCON0) & ~(0x1<<22 | 0x1<<16 | 0x3<<9 | 0xf<<2 | 0x1<<0)) | (0x5<<2 | 0x1<<16));

	s5pv210fb_window_on(lcd, 0);
	s5pv210fb_display_on(lcd);

	/* delay for avoid flash screen */
//	mdelay(50);

#if 0
	x_u64 hclk;

	/* set gpd0_0 (backlight pin) output and pull up and high level */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0x3<<0)) | (0x1<<0));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));

	/* gpf3_5 high level for enable lcd power */
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0x3<<20)) | (0x1<<20));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0x3<<10)) | (0x2<<10));
	writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<5)) | (0x1<<5));

	/* gpf3_4 high level for reset pin */
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0x3<<16)) | (0x1<<16));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0x3<<8)) | (0x2<<8));
	writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<4)) | (0x1<<4));

	/* display path selection */
	writel(S5PV210_DISPLAY_CONTROL, (readl(S5PV210_DISPLAY_CONTROL) & ~(0x3<<0)) | (0x2<<0));

	/* lcd port config */
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

	/* gpf0_2 low level for enable display */
	writel(S5PV210_GPF0CON, (readl(S5PV210_GPF0CON) & ~(0x3<<8)) | (0x1<<8));
	writel(S5PV210_GPF0PUD, (readl(S5PV210_GPF0PUD) & ~(0x3<<4)) | (0x2<<4));
	writel(S5PV210_GPF0DAT, (readl(S5PV210_GPF0DAT) & ~(0x1<<2)) | (0x0<<2));

	/* initial lcd controler */
	writel(S5PV210_VIDCON1, REGS_VIDCON1);
	writel(S5PV210_VIDTCON0, REGS_VIDTCON0);
	writel(S5PV210_VIDTCON1, REGS_VIDTCON1);
	writel(S5PV210_VIDTCON2, REGS_VIDTCON2);
	writel(S5PV210_DITHMODE, REGS_DITHMODE);

	/* get hclk for lcd */
//	clk_get_rate("hclk", &hclk);
//	writel(S3C6410_VIDCON0, (REGS_VIDCON0 | S3C6410_VIDCON0_CLKVAL_F((x_u32)(div64(hclk, PIXEL_CLOCK) - 1)) ) );

	/* turn all windows off */
	writel(S5PV210_WINCON0, (readl(S5PV210_WINCON0) & ~0x1));
	writel(S5PV210_WINCON1, (readl(S5PV210_WINCON1) & ~0x1));
	writel(S5PV210_WINCON2, (readl(S5PV210_WINCON2) & ~0x1));
	writel(S5PV210_WINCON3, (readl(S5PV210_WINCON3) & ~0x1));
	writel(S5PV210_WINCON4, (readl(S5PV210_WINCON4) & ~0x1));

	/* turn all windows color map off */
	writel(S5PV210_WIN0MAP, (readl(S5PV210_WIN0MAP) & ~(1<<24)));
	writel(S5PV210_WIN1MAP, (readl(S5PV210_WIN1MAP) & ~(1<<24)));
	writel(S5PV210_WIN2MAP, (readl(S5PV210_WIN2MAP) & ~(1<<24)));
	writel(S5PV210_WIN3MAP, (readl(S5PV210_WIN3MAP) & ~(1<<24)));
	writel(S5PV210_WIN4MAP, (readl(S5PV210_WIN4MAP) & ~(1<<24)));

	/* turn all windows color key off */
	writel(S5PV210_W1KEYCON0, (readl(S5PV210_W1KEYCON0) & ~(3<<25)));
	writel(S5PV210_W2KEYCON0, (readl(S5PV210_W2KEYCON0) & ~(3<<25)));
	writel(S5PV210_W3KEYCON0, (readl(S5PV210_W3KEYCON0) & ~(3<<25)));
	writel(S5PV210_W4KEYCON0, (readl(S5PV210_W4KEYCON0) & ~(3<<25)));

	/* config window 0 */
	writel(S5PV210_WINCON0, (readl(S5PV210_WINCON0) & ~(0x1<<22 | 0x1<<16 | 0x3<<9 | 0xf<<2 | 0x1<<0)) | (0x5<<2 | 0x1<<16));

	/* window 0 frambuffer addresss */
	writel(S5PV210_VIDW00ADD0B0, (x_u32)info.bitmap.data);
	writel(S5PV210_VIDW00ADD0B1, (x_u32)info.bitmap.data);
	writel(S5PV210_VIDW00ADD0B2, (x_u32)info.bitmap.data);
	writel(S5PV210_VIDW00ADD1B0, ((x_u32)info.bitmap.data + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S5PV210_VIDW00ADD1B1, ((x_u32)info.bitmap.data + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S5PV210_VIDW00ADD1B2, ((x_u32)info.bitmap.data + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S5PV210_VIDW00ADD2, (LCD_WIDTH*LCD_BPP/8) & 0x00001fff);

	/*
	 * window position control register
	 */
	#define OSD_LTX(n)							( ( (n) & 0x7ff ) << 11 )
	#define OSD_LTY(n)							( ( (n) & 0x7ff ) << 0 )
	#define OSD_RBX(n)							( ( (n) & 0x7ff ) << 11 )
	#define OSD_RBY(n)							( ( (n) & 0x7ff ) << 0 )
	#define OSDSIZE(n)							( ( (n) & 0xffffff ) << 0 )

	/* config view port */
	writel(S5PV210_VIDOSD0A, OSD_LTX(0) | OSD_LTY(0));
	writel(S5PV210_VIDOSD0B, OSD_RBX(LCD_WIDTH) | OSD_RBY(LCD_HEIGHT));
	writel(S5PV210_VIDOSD0C, OSDSIZE(LCD_WIDTH * LCD_HEIGHT));

	/* enable window 0 */
	writel(S5PV210_WINCON0, (readl(S5PV210_WINCON0) | 0x1));

	/* enable video controller output */
	writel(S5PV210_VIDCON0, (readl(S5PV210_VIDCON0) | 0x3));

	/* delay for avoid flash screen */
	mdelay(50);


//test
	/* initial lcd controler */
/*	writel(0xf8000004, 0x60);
	writel(0xf8000010, 0xe0e0305);
	writel(0xf8000014, 0x3103020);
	writel(0xf8000018, 0x17fd55);
	writel(0xf8000170, 0x0);*/



	writel(0xf8000000, 0x0);
	writel(0xf8000000, 0x254);
	writel(0xf8000130, 0x20);
	writel(0xf8000020, 0x0);
	writel(0xf8000024, 0x0);
	writel(0xf8000028, 0x0);
	writel(0xf800002c, 0x0);
	writel(0xf8000030, 0x0);
	writel(0xf8000034, 0x0);
	writel(0xf8000180, 0x0);
	writel(0xf8000184, 0x0);
	writel(0xf8000188, 0x0);
	writel(0xf800018c, 0x0);
	writel(0xf8000190, 0x0);
	writel(0xf8000140, 0x0);
	writel(0xf8000148, 0x0);
	writel(0xf8000150, 0x0);
	writel(0xf8000158, 0x0);
	writel(0xf8000058, 0x0);
	writel(0xf8000208, 0x0);
	writel(0xf800020c, 0x0);
	writel(0xf8000068, 0x0);
	writel(0xf8000210, 0x0);
	writel(0xf8000214, 0x0);
	writel(0xf8000078, 0x0);
	writel(0xf8000218, 0x0);
	writel(0xf800021c, 0x0);
	writel(0xf8000088, 0x0);
	writel(0xf8000220, 0x0);
	writel(0xf8000224, 0x0);
	writel(0xf8000260, 0x1);
	writel(0xf8000048, 0x100200);
	writel(0xf8000200, 0xffffff);
	writel(0xf8000204, 0xffffff);
	writel(0xf8000034, 0x0);
	writel(0xf8000020, 0x802c);
/*
	writel(0xf80000a0, uFbAddr + 0x00000000);
	writel(0xf80000d0, uFbAddr + 0x00400800);
	writel(0xf80000a4, uFbAddr + 0x00000000);
	writel(0xf80000d4, uFbAddr + 0x00400800);
	writel(0xf80020a0, uFbAddr + 0x00000000);
	writel(0xf80020d0, uFbAddr + 0x00400800);
	*/

	writel(0xf8000100, 0x1558);
	writel(0xf8000040, 0x0);
	writel(0xf8000044, 0x2aaaff);
	writel(0xf8000020, 0x802d);
	writel(0xf8000034, 0x1);
	writel(0xf8000020, 0x802d);
	writel(0xf8000034, 0x1);
	writel(0xf8000000, 0x257);
	writel(0xf8000000, 0x57); //===> MPLL should be 667 !!!!
	writel(0xf8000000, 0x53);
	writel(0xf8000010, 0x60400);
	writel(0xf80001a4, 0x3);

//	writel(0xe0107008,0x2); //syscon output path
//	writel(0xe0100204,0x700000); //syscon fimdclk = mpll
#endif
}

static void fb_exit(struct fb * fb)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);

	if(lcd->exit)
		lcd->exit();
}

static x_s32 fb_ioctl(struct fb * fb, x_u32 cmd, void * arg)
{
	struct s5pv210fb_lcd * lcd = (struct s5pv210fb_lcd *)(fb->priv);
	static x_u8 brightness = 0;
	x_u8 * p;

	switch(cmd)
	{
	case IOCTL_SET_FB_BACKLIGHT:
		p = (x_u8 *)arg;
		brightness = (*p) & 0xff;
		if(lcd->backlight)
			lcd->backlight(brightness);
		return 0;

	case IOCTL_GET_FB_BACKLIGHT:
		p = (x_u8 *)arg;
		*p = brightness;
		return 0;

	default:
		break;
	}

	return -1;
}

static struct fb_info info = {
	.name			= "fb",
};

static struct fb s5pv210_fb = {
	.info			= &info,
	.init			= fb_init,
	.exit			= fb_exit,
	.map_color		= fb_soft_map_color,
	.unmap_color	= fb_soft_unmap_color,
	.fill_rect		= fb_soft_fill_rect,
	.blit_bitmap	= fb_soft_blit_bitmap,
	.ioctl			= fb_ioctl,
	.priv			= NULL,
};

static __init void s5pv210_fb_init(void)
{
	struct s5pv210fb_lcd * lcd;

	s5pv210_fb.priv = resource_get_data(s5pv210_fb.info->name);
	lcd = (struct s5pv210fb_lcd *)(s5pv210_fb.priv);

	if(! s5pv210_fb.priv)
	{
		LOG_W("can't get the resource of \'%s\'", s5pv210_fb.info->name);
		return;
	}

	if( (lcd->bpp != 16) && (lcd->bpp != 24) && (lcd->bpp != 32) )
		return;

	info.bitmap.info.width = lcd->width;
	info.bitmap.info.height = lcd->height;
	info.bitmap.info.bpp = lcd->bpp;
	info.bitmap.info.bytes_per_pixel = lcd->bpp / 8;
	info.bitmap.info.pitch = lcd->width * lcd->bpp / 8;

	info.bitmap.info.red_mask_size = lcd->rgba.r_mask;
	info.bitmap.info.red_field_pos = lcd->rgba.r_field;
	info.bitmap.info.green_mask_size = lcd->rgba.g_mask;
	info.bitmap.info.green_field_pos = lcd->rgba.g_field;
	info.bitmap.info.blue_mask_size = lcd->rgba.b_mask;
	info.bitmap.info.blue_field_pos = lcd->rgba.b_field;
	info.bitmap.info.alpha_mask_size = lcd->rgba.a_mask;
	info.bitmap.info.alpha_field_pos = lcd->rgba.a_field;

	info.bitmap.info.fmt = get_bitmap_format(&(info.bitmap.info));

	info.bitmap.info.fg_r = 0xff;
	info.bitmap.info.fg_g = 0xff;
	info.bitmap.info.fg_b = 0xff;
	info.bitmap.info.fg_a = 0xff;

	info.bitmap.info.bg_r = 0x00;
	info.bitmap.info.bg_g = 0x00;
	info.bitmap.info.bg_b = 0x00;
	info.bitmap.info.bg_a = 0x00;

	info.bitmap.viewport.left = 0;
	info.bitmap.viewport.top = 0;
	info.bitmap.viewport.right = lcd->width;
	info.bitmap.viewport.bottom = lcd->height;

	info.bitmap.allocated = FALSE;
	info.bitmap.data = lcd->vram;

	if(!register_framebuffer(&s5pv210_fb))
		LOG_E("failed to register framebuffer driver '%s'", s5pv210_fb.info->name);
}

static __exit void s5pv210_fb_exit(void)
{
	if(!unregister_framebuffer(&s5pv210_fb))
		LOG_E("failed to unregister framebuffer driver '%s'", s5pv210_fb.info->name);
}

module_init(s5pv210_fb_init, LEVEL_DRIVER);
module_exit(s5pv210_fb_exit, LEVEL_DRIVER);
