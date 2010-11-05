/*
 * driver/s5pv210-fb.c
 *
 * s3c2410 framebuffer drivers.
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
#include <s5pv210/reg-gpio.h>
#include <s5pv210/reg-lcd.h>
#include <fb/fb.h>


/* lcd module - lte480wv */
#define	LCD_WIDTH		(800)
#define	LCD_HEIGHT		(480)
#define	LCD_BPP			(16)
#define	LCD_FORMAT		(FORMAT_RGB_565)
#if 0
#define VBPD			(7)
#define VFPD			(5)
#define VSPW			(1)
#define HBPD			(13)
#define HFPD			(8)
#define HSPW			(3)
#define FREQ			(75)
#define PIXEL_CLOCK		(FREQ * (HFPD + HSPW + HBPD + LCD_WIDTH) * (VFPD + VSPW + VBPD + LCD_HEIGHT))

#define	REGS_VIDCON0	( S3C6410_VIDCON0_PROGRESSIVE | S3C6410_VIDCON0_VIDOUT_RGBIF | S3C6410_VIDCON0_SUB_16_MODE | S3C6410_VIDCON0_MAIN_16_MODE | \
						S3C6410_VIDCON0_PNRMODE_RGB_P | S3C6410_VIDCON0_CLKVALUP_ALWAYS | S3C6410_VIDCON0_VCLKFREE_NORMAL | \
						S3C6410_VIDCON0_CLKDIR_DIVIDED | S3C6410_VIDCON0_CLKSEL_F_HCLK | S3C6410_VIDCON0_ENVID_DISABLE | S3C6410_VIDCON0_ENVID_F_DISABLE )
#define	REGS_VIDCON1	( S3C6410_VIDCON1_IHSYNC_INVERT | S3C6410_VIDCON1_IVSYNC_INVERT  | S3C6410_VIDCON1_IVDEN_NORMAL | S3C6410_VIDCON1_IVCLK_RISE_EDGE )
#define	REGS_VIDTCON0	( S3C6410_VIDTCON0_VBPDE(VBPD-1) | S3C6410_VIDTCON0_VBPD(VBPD-1) | S3C6410_VIDTCON0_VFPD(VFPD-1) | S3C6410_VIDTCON0_VSPW(VSPW-1) )
#define	REGS_VIDTCON1	( S3C6410_VIDTCON1_VFPDE(VFPD-1) | S3C6410_VIDTCON1_HBPD(HBPD-1) | S3C6410_VIDTCON1_HFPD(HFPD-1) | S3C6410_VIDTCON1_HSPW(HSPW-1) )
#define	REGS_VIDTCON2	( S3C6410_VIDTCON2_LINEVAL(LCD_HEIGHT-1) | S3C6410_VIDTCON2_HOZVAL(LCD_WIDTH-1) )
#define	REGS_DITHMODE	( (S3C6410_DITHMODE_RDITHPOS_5BIT | S3C6410_DITHMODE_GDITHPOS_6BIT | S3C6410_DITHMODE_BDITHPOS_5BIT ) & (~S3C6410_DITHMODE_DITHERING_ENABLE) )
#endif

/*
 * video ram buffer for lcd.
 */
static x_u8 vram[LCD_WIDTH * LCD_HEIGHT * LCD_BPP / 8] __attribute__((aligned(4)));
static x_u16 * pvram;

static struct fb_info info = {
	.name		= "fb",
	.width		= LCD_WIDTH,
	.height		= LCD_HEIGHT,
	.format		= LCD_FORMAT,
	.bpp		= LCD_BPP,
	.stride		= LCD_WIDTH*LCD_BPP/8,
	.pos		= 0,
	.base		= 0,
};

static void fb_init(void)
{
	x_u64 hclk;

	/* setting vram base address */
	info.base = (void *)((x_u32)(&vram));
	pvram = (x_u16 *)((x_u32)info.base);

	/* set gpd0_3 (backlight pin) output and pull up and low level */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0x3<<12)) | (0x1<<12));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<6)) | (0x2<<6));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<3)) | (0x0<<3));

	/* gph0_6 high level for reset pin */
	writel(S5PV210_GPH0CON, (readl(S5PV210_GPH0CON) & ~(0x3<<24)) | (0x1<<24));
	writel(S5PV210_GPH0PUD, (readl(S5PV210_GPH0PUD) & ~(0x3<<12)) | (0x2<<12));
	writel(S5PV210_GPH0DAT, (readl(S5PV210_GPH0DAT) & ~(0x1<<6)) | (0x1<<6));

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
	writel(S5PV210_GPF3CON, 0x00002222);
	writel(S5PV210_GPF3DRV, 0x0ff);
	writel(S5PV210_GPF3PUD, 0x0);

#define	REGS_VIDCON1	0x60
#define	REGS_VIDTCON0	0xe0e0305
#define	REGS_VIDTCON1	0x3103020
#define	REGS_VIDTCON2	0x17fd55
#define	REGS_DITHMODE	0

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
	writel(S5PV210_VIDW00ADD0B0, (x_u32)info.base);
	writel(S5PV210_VIDW00ADD0B1, (x_u32)info.base);
	writel(S5PV210_VIDW00ADD0B2, (x_u32)info.base);
	writel(S5PV210_VIDW00ADD1B0, ((x_u32)info.base + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S5PV210_VIDW00ADD1B1, ((x_u32)info.base + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
	writel(S5PV210_VIDW00ADD1B2, ((x_u32)info.base + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8)& 0x00ffffff);
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

//TODO
//test
	writel(0xf8000004, 0x60);
	writel(0xf8000010, 0xe0e0305);
	writel(0xf8000014, 0x3103020);
	writel(0xf8000170, 0x0);
	writel(0xf8000018, 0x17fd55);
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

	writel(0xe0107008,0x2); //syscon output path
//	writel(0xe0100204,0x700000); //syscon fimdclk = mpll
}

static void fb_exit(void)
{
	/* disable video output */
//	writel(S3C6410_VIDCON0, (readl(S3C6410_VIDCON0) & (~0x3)));
}

static void fb_bl(x_u8 brightness)
{
	if(brightness)
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<3)) | (0x1<<3));
	else
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<3)) | (0x0<<3));
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

static x_s32 fb_ioctl(x_u32 cmd, void * arg)
{
	return -1;
}

static struct fb s5pv210_fb = {
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

static __init void s5pv210_fb_init(void)
{
/*	if(!clk_get_rate("hclk", 0))
	{
		LOG_E("can't get the clock of \'hclk\'");
		return;
	}
*/
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
