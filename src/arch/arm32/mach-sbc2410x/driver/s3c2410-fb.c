/*
 * driver/s3c2410-fb.c
 *
 * s3c2410 framebuffer drivers.
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
#include <s3c2410/reg-gpio.h>
#include <s3c2410/reg-lcd.h>

#define	LCD_WIDTH		(640)
#define	LCD_HEIGHT		(480)
#define	LCD_BPP			(16)

#define REGS_LCDCON1	( S3C2410_LCDCON1_TFT16BPP | S3C2410_LCDCON1_TFT | S3C2410_LCDCON1_CLKVAL(6) )
#define REGS_LCDCON2	( S3C2410_LCDCON2_VBPD(0) | S3C2410_LCDCON2_LINEVAL(LCD_HEIGHT-1) | S3C2410_LCDCON2_VFPD(10) |S3C2410_LCDCON2_VSPW(1) )
#define REGS_LCDCON3	( S3C2410_LCDCON3_HBPD(6) | S3C2410_LCDCON3_HOZVAL(LCD_WIDTH-1) | S3C2410_LCDCON3_HFPD(15) )
#define REGS_LCDCON4	( S3C2410_LCDCON4_MVAL(13) | S3C2410_LCDCON4_HSPW(28) )
#define REGS_LCDCON5	( S3C2410_LCDCON5_FRM565 | S3C2410_LCDCON5_INVVLINE | S3C2410_LCDCON5_INVVFRAME | S3C2410_LCDCON5_PWREN | S3C2410_LCDCON5_HWSWP )

#define	VRAM_ADDR		( ((u32_t)fb->info->surface.pixels) )
#define REGS_LCDSADDR1	( ((VRAM_ADDR>>22)<<21) | ((VRAM_ADDR>>1)&0x001FFFFF) )
#define REGS_LCDSADDR2	( ((VRAM_ADDR + (LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8))>>1)&0x001FFFFF )
#define REGS_LCDSADDR3	( (((LCD_WIDTH-LCD_WIDTH)/1)<<11)|(LCD_WIDTH/1) )


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
	/* initial lcd controler */
	writel(S3C2410_LCDCON1, REGS_LCDCON1);
	writel(S3C2410_LCDCON2, REGS_LCDCON2);
	writel(S3C2410_LCDCON3, REGS_LCDCON3);
	writel(S3C2410_LCDCON4, REGS_LCDCON4);
	writel(S3C2410_LCDCON5, REGS_LCDCON5);
	writel(S3C2410_LPCSEL, 	0);

	writel(S3C2410_LCDSADDR1, REGS_LCDSADDR1);
	writel(S3C2410_LCDSADDR2, REGS_LCDSADDR2);
	writel(S3C2410_LCDSADDR3, REGS_LCDSADDR3);

	/* ensure temporary palette disabled */
	writel(S3C2410_TPAL, 0x00);

	/* enable video by setting the ENVID bit to 1 */
	writel(S3C2410_LCDCON1, REGS_LCDCON1|S3C2410_LCDCON1_ENVID);
}

static void fb_exit(struct fb * fb)
{
	return;
}

static void fb_swap(struct fb * fb)
{
	static u8_t vram_index = 0;

	vram_index = (vram_index + 1) & 0x1;
	fb->info->surface.pixels = &vram[vram_index][0];
}

static void fb_flush(struct fb * fb)
{
	writel(S3C2410_LCDSADDR1, REGS_LCDSADDR1);
	writel(S3C2410_LCDSADDR2, REGS_LCDSADDR2);
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

static struct fb s3c2410_fb = {
	.info			= &info,
	.init			= fb_init,
	.exit			= fb_exit,
	.swap			= fb_swap,
	.flush			= fb_flush,
	.ioctl			= fb_ioctl,
	.priv			= NULL,
};

static __init void s3c2410_fb_init(void)
{
	if(!register_framebuffer(&s3c2410_fb))
		LOG_E("failed to register framebuffer driver '%s'", s3c2410_fb.info->name);
}

static __exit void s3c2410_fb_exit(void)
{
	if(!unregister_framebuffer(&s3c2410_fb))
		LOG_E("failed to unregister framebuffer driver '%s'", s3c2410_fb.info->name);
}

device_initcall(s3c2410_fb_init);
device_exitcall(s3c2410_fb_exit);
