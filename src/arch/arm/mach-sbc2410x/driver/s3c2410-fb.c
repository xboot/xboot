/*
 * driver/s3c2410-fb.c
 *
 * s3c2410 on chip serial drivers.
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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
#include <xboot/log.h>
#include <xboot/io.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <s3c2410/reg-gpio.h>
#include <s3c2410/reg-lcd.h>
#include <fb/fb.h>


#define	LCD_WIDTH		(640)
#define	LCD_HEIGHT		(480)
#define	LCD_BPP			(16)

#define REGS_LCDCON1	( S3C2410_LCDCON1_TFT16BPP | S3C2410_LCDCON1_TFT | S3C2410_LCDCON1_CLKVAL(6) )
#define REGS_LCDCON2	( S3C2410_LCDCON2_VBPD(0) | S3C2410_LCDCON2_LINEVAL(LCD_HEIGHT-1) | S3C2410_LCDCON2_VFPD(10) |S3C2410_LCDCON2_VSPW(1) )
#define REGS_LCDCON3	( S3C2410_LCDCON3_HBPD(6) | S3C2410_LCDCON3_HOZVAL(LCD_WIDTH-1) | S3C2410_LCDCON3_HFPD(15) )
#define REGS_LCDCON4	( S3C2410_LCDCON4_MVAL(13) | S3C2410_LCDCON4_HSPW(28) )
#define REGS_LCDCON5	( S3C2410_LCDCON5_FRM565 | S3C2410_LCDCON5_INVVLINE | S3C2410_LCDCON5_INVVFRAME | S3C2410_LCDCON5_PWREN | S3C2410_LCDCON5_HWSWP )

#define	VRAM_ADDR		( (x_u32)(((x_u32)(&vram) + 4 - 1) & ~(4 - 1)) )
#define REGS_LCDSADDR1	( ((VRAM_ADDR>>22)<<21) | ((VRAM_ADDR>>1)&0x001FFFFF) )
#define REGS_LCDSADDR2	( ((VRAM_ADDR + (LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8))>>1)&0x001FFFFF )
#define REGS_LCDSADDR3	( (((LCD_WIDTH-LCD_WIDTH)/1)<<11)|(LCD_WIDTH/1) )


/*
 * video ram buffer for lcd.
 */
static x_u8 vram[LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8 + 4];
static x_u16 * pvram;

static struct fb_info info = {
	.name		= "fb0",
	.width		= LCD_WIDTH,
	.height		= LCD_HEIGHT,
	.format		= FORMAT_RGB_565,
	.bpp		= LCD_BPP,
	.stride		= LCD_WIDTH*LCD_BPP/8,
	.pos		= 0,
	.base		= 0,
};

static void fb_init(void)
{
	/* setting vram base address */
	info.base = (void *)VRAM_ADDR;
	pvram = (x_u16 *)VRAM_ADDR;

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

static void fb_exit(void)
{
	writel(S3C2410_LCDCON1, REGS_LCDCON1&(~S3C2410_LCDCON1_ENVID));
}

static void fb_bl(x_u8 brightness)
{
	return;
}

static void fb_set_pixel(x_u32 x, x_u32 y, x_u32 c)
{
	 *(pvram + LCD_WIDTH*y + x) = c;
}

static x_u32 fb_get_pixel(x_u32 x, x_u32 y)
{
	return *(pvram + LCD_WIDTH*y + x);
}

static void fb_hline(x_u32 x0, x_u32 y0, x_u32 x, x_u32 c)
{
	x_u16 * p = (x_u16 *)(pvram + LCD_WIDTH*y0 + x0);

	while(x--)
		*(p++) = c;
}

static void fb_vline(x_u32 x0, x_u32 y0, x_u32 y, x_u32 c)
{
	x_u16 * p = (x_u16 *)(pvram + LCD_WIDTH*y0 + x0);

	while(y--)
	{
		*p = c;
		p += LCD_WIDTH;
	}
}

static x_s32 fb_ioctl(x_u32 cmd, x_u32 arg)
{
	return -1;
}

static struct fb s3c2410_fb = {
	.info		= &info,
	.init		= fb_init,
	.exit		= fb_exit,
	.bl			= fb_bl,
	.set_pixel	= fb_set_pixel,
	.get_pixel	= fb_get_pixel,
	.hline		= fb_hline,
	.vline		= fb_vline,
	.ioctl		= fb_ioctl,
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

module_init(s3c2410_fb_init, LEVEL_DRIVER);
module_exit(s3c2410_fb_exit, LEVEL_DRIVER);
