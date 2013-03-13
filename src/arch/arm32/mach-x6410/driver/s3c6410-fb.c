/*
 * driver/s3c6410-fb.c
 *
 * s3c6410 framebuffer drivers.
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
#include <xboot/log.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <fb/fb.h>
#include <s3c6410/reg-gpio.h>
#include <s3c6410/reg-lcd.h>

/* the lcd module - lw500 */
#define	LCD_WIDTH		(480)
#define	LCD_HEIGHT		(272)
#define	LCD_BPP			(16)

#define VBPD			(2)
#define VFPD			(2)
#define VSPW			(10)
#define HBPD			(2)
#define HFPD			(2)
#define HSPW			(41)
#define FREQ			(50)
#define PIXEL_CLOCK		(FREQ * (HFPD + HSPW + HBPD + LCD_WIDTH) * (VFPD + VSPW + VBPD + LCD_HEIGHT))

#define	REGS_VIDCON0	( S3C6410_VIDCON0_PROGRESSIVE | S3C6410_VIDCON0_VIDOUT_RGBIF | S3C6410_VIDCON0_SUB_16_MODE | S3C6410_VIDCON0_MAIN_16_MODE | \
						S3C6410_VIDCON0_PNRMODE_RGB_P | S3C6410_VIDCON0_CLKVALUP_ALWAYS | S3C6410_VIDCON0_VCLKFREE_NORMAL | \
						S3C6410_VIDCON0_CLKDIR_DIVIDED | S3C6410_VIDCON0_CLKSEL_F_HCLK | S3C6410_VIDCON0_ENVID_DISABLE | S3C6410_VIDCON0_ENVID_F_DISABLE )
#define	REGS_VIDCON1	( S3C6410_VIDCON1_IHSYNC_INVERT | S3C6410_VIDCON1_IVSYNC_INVERT  | S3C6410_VIDCON1_IVDEN_NORMAL | S3C6410_VIDCON1_IVCLK_RISE_EDGE )
#define	REGS_VIDTCON0	( S3C6410_VIDTCON0_VBPDE(VBPD-1) | S3C6410_VIDTCON0_VBPD(VBPD-1) | S3C6410_VIDTCON0_VFPD(VFPD-1) | S3C6410_VIDTCON0_VSPW(VSPW-1) )
#define	REGS_VIDTCON1	( S3C6410_VIDTCON1_VFPDE(VFPD-1) | S3C6410_VIDTCON1_HBPD(HBPD-1) | S3C6410_VIDTCON1_HFPD(HFPD-1) | S3C6410_VIDTCON1_HSPW(HSPW-1) )
#define	REGS_VIDTCON2	( S3C6410_VIDTCON2_LINEVAL(LCD_HEIGHT-1) | S3C6410_VIDTCON2_HOZVAL(LCD_WIDTH-1) )
#define	REGS_DITHMODE	( (S3C6410_DITHMODE_RDITHPOS_5BIT | S3C6410_DITHMODE_GDITHPOS_6BIT | S3C6410_DITHMODE_BDITHPOS_5BIT ) & (~S3C6410_DITHMODE_DITHERING_ENABLE) )

/*
 * video ram double buffer for lcd.
 */
static u8_t vram[2][LCD_WIDTH * LCD_HEIGHT * LCD_BPP / 8] __attribute__((aligned(4)));

static struct fb_info info = {
	.name						= "fb",

	.surface = {
		.info = {
			.bits_per_pixel		= LCD_BPP,
			.bytes_per_pixel	= LCD_BPP / 8,

			.red_mask_size		= 5,
			.red_field_pos		= 0,
			.green_mask_size	= 6,
			.green_field_pos	= 5,
			.blue_mask_size		= 5,
			.blue_field_pos		= 11,
			.alpha_mask_size	= 0,
			.alpha_field_pos	= 0,

			.fmt				= PIXEL_FORMAT_BGR_565,
		},

		.w						= LCD_WIDTH,
		.h						= LCD_HEIGHT,
		.pitch					= LCD_WIDTH * LCD_BPP / 8,
		.flag					= SURFACE_PIXELS_DONTFREE,
		.pixels					= &vram[0][0],

		.clip = {
			.x					= 0,
			.y					= 0,
			.w					= LCD_WIDTH,
			.h					= LCD_HEIGHT,
		},

		.maps = {
			.point				= map_software_point,
			.hline				= map_software_hline,
			.vline				= map_software_vline,
			.fill				= map_software_fill,
			.blit				= map_software_blit,
			.scale				= map_software_scale,
			.rotate				= map_software_rotate,
			.transform			= map_software_transform,
		},
	},
};

static void fb_init(struct fb * fb)
{
	u64_t hclk;

	/* set gpf15 (backlight pin) output and pull up and low level */
	writel(S3C6410_GPFCON, (readl(S3C6410_GPFCON) & ~(0x3<<30)) | (0x1<<30));
	writel(S3C6410_GPFPUD, (readl(S3C6410_GPFPUD) & ~(0x3<<30)) | (0x2<<30));
	writel(S3C6410_GPFDAT, (readl(S3C6410_GPFDAT) & ~(0x1<<15)) | (0x0<<15));

	/* must be '0' for normal-path instead of by-pass */
	writel(S3C6410_MIFPCON, 0);

	/* select tft lcd type (rgb i/f) */
	writel(S3C6410_SPCON, (readl(S3C6410_SPCON) & ~(0x3<<0)) | (0x1<<0));

	/* lcd port config */
	writel(S3C6410_GPICON, 0xaaaaaaaa);
	writel(S3C6410_GPJCON, 0xaaaaaaaa);

	/* initial lcd controler */
	writel(S3C6410_VIDCON1, REGS_VIDCON1);
	writel(S3C6410_VIDTCON0, REGS_VIDTCON0);
	writel(S3C6410_VIDTCON1, REGS_VIDTCON1);
	writel(S3C6410_VIDTCON2, REGS_VIDTCON2);
	writel(S3C6410_DITHMODE, REGS_DITHMODE);

	/* get hclk for lcd */
	clk_get_rate("hclk", &hclk);
	writel(S3C6410_VIDCON0, (REGS_VIDCON0 | S3C6410_VIDCON0_CLKVAL_F((u32_t)(div64(hclk, PIXEL_CLOCK) - 1)) ) );

	/* turn all windows off */
	writel(S3C6410_WINCON0, (readl(S3C6410_WINCON0) & ~0x1));
	writel(S3C6410_WINCON1, (readl(S3C6410_WINCON1) & ~0x1));
	writel(S3C6410_WINCON2, (readl(S3C6410_WINCON2) & ~0x1));
	writel(S3C6410_WINCON3, (readl(S3C6410_WINCON3) & ~0x1));
	writel(S3C6410_WINCON4, (readl(S3C6410_WINCON4) & ~0x1));

	/* turn all windows color map off */
	writel(S3C6410_WIN0MAP, (readl(S3C6410_WIN0MAP) & ~(1<<24)));
	writel(S3C6410_WIN1MAP, (readl(S3C6410_WIN1MAP) & ~(1<<24)));
	writel(S3C6410_WIN2MAP, (readl(S3C6410_WIN2MAP) & ~(1<<24)));
	writel(S3C6410_WIN3MAP, (readl(S3C6410_WIN3MAP) & ~(1<<24)));
	writel(S3C6410_WIN4MAP, (readl(S3C6410_WIN4MAP) & ~(1<<24)));

	/* turn all windows color key off */
	writel(S3C6410_W1KEYCON0, (readl(S3C6410_W1KEYCON0) & ~(3<<25)));
	writel(S3C6410_W2KEYCON0, (readl(S3C6410_W2KEYCON0) & ~(3<<25)));
	writel(S3C6410_W3KEYCON0, (readl(S3C6410_W3KEYCON0) & ~(3<<25)));
	writel(S3C6410_W4KEYCON0, (readl(S3C6410_W4KEYCON0) & ~(3<<25)));

	/* config window 0 */
	writel(S3C6410_WINCON0, (readl(S3C6410_WINCON0) & ~(0x1<<22 | 0x1<<16 | 0x3<<9 | 0xf<<2 | 0x1<<0)) | (0x5<<2 | 0x1<<16));

	/* window 0 frambuffer addresss */
	writel(S3C6410_VIDW00ADD0B0, ((u32_t)fb->info->surface.pixels));
	writel(S3C6410_VIDW00ADD0B1, ((u32_t)fb->info->surface.pixels));
	writel(S3C6410_VIDW00ADD1B0, (((u32_t)fb->info->surface.pixels) + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S3C6410_VIDW00ADD1B1, (((u32_t)fb->info->surface.pixels) + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S3C6410_VIDW00ADD2, (LCD_WIDTH*LCD_BPP/8) & 0x00001fff);

	/* config view port */
	writel(S3C6410_VIDOSD0A, OSD_LTX(0) | OSD_LTY(0));
	writel(S3C6410_VIDOSD0B, OSD_RBX(LCD_WIDTH) | OSD_RBY(LCD_HEIGHT));
	writel(S3C6410_VIDOSD0C, OSDSIZE(LCD_WIDTH * LCD_HEIGHT));

	/* enable window 0 */
	writel(S3C6410_WINCON0, (readl(S3C6410_WINCON0) | 0x1));

	/* enable video controller output */
	writel(S3C6410_VIDCON0, (readl(S3C6410_VIDCON0) | 0x3));

	/* delay for avoid flash screen */
	mdelay(50);
}

static void fb_exit(struct fb * fb)
{
	/* disable video output */
	writel(S3C6410_VIDCON0, (readl(S3C6410_VIDCON0) & (~0x3)));
}

static void fb_swap(struct fb * fb)
{
	static u8_t vram_index = 0;

	vram_index = (vram_index + 1) & 0x1;
	fb->info->surface.pixels = &vram[vram_index][0];
}

static void fb_flush(struct fb * fb)
{
	writel(S3C6410_VIDW00ADD0B0, ((u32_t)fb->info->surface.pixels));
	writel(S3C6410_VIDW00ADD0B1, ((u32_t)fb->info->surface.pixels));
	writel(S3C6410_VIDW00ADD1B0, (((u32_t)fb->info->surface.pixels) + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S3C6410_VIDW00ADD1B1, (((u32_t)fb->info->surface.pixels) + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
}

static int fb_ioctl(struct fb * fb, int cmd, void * arg)
{
	static u8_t brightness = 0;
	u8_t * p;

	switch(cmd)
	{
	case IOCTL_SET_FB_BACKLIGHT:
		p = (u8_t *)arg;
		brightness = (*p) & 0xff;

		if(brightness)
			writel(S3C6410_GPFDAT, (readl(S3C6410_GPFDAT) & ~(0x1<<15)) | (0x1<<15));
		else
			writel(S3C6410_GPFDAT, (readl(S3C6410_GPFDAT) & ~(0x1<<15)) | (0x0<<15));

		return 0;

	case IOCTL_GET_FB_BACKLIGHT:
		p = (u8_t *)arg;
		*p = brightness;
		return 0;

	default:
		break;
	}

	return -1;
}

static struct fb s3c6410_fb = {
	.info			= &info,
	.init			= fb_init,
	.exit			= fb_exit,
	.swap			= fb_swap,
	.flush			= fb_flush,
	.ioctl			= fb_ioctl,
	.priv			= NULL,
};

static __init void s3c6410_fb_init(void)
{
	if(!clk_get_rate("hclk", 0))
	{
		LOG_E("can't get the clock of \'hclk\'");
		return;
	}

	if(!register_framebuffer(&s3c6410_fb))
		LOG_E("failed to register framebuffer driver '%s'", s3c6410_fb.info->name);
}

static __exit void s3c6410_fb_exit(void)
{
	if(!unregister_framebuffer(&s3c6410_fb))
		LOG_E("failed to unregister framebuffer driver '%s'", s3c6410_fb.info->name);
}

device_initcall(s3c6410_fb_init);
device_exitcall(s3c6410_fb_exit);
