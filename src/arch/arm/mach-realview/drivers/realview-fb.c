/*
 * drivers/realview-fb.c
 *
 * realview framebuffer drivers. prime cell lcd controller (pl110)
 *
 * Copyright (c) 2007-2009  jianjun jiang <jerryjianjun@gmail.com>
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
#include <debug.h>
#include <macros.h>
#include <malloc.h>
#include <time/delay.h>
#include <xboot/io.h>
#include <xboot/ioctl.h>
#include <xboot/clk.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/platform_device.h>
#include <realview/reg-lcd.h>
#include <fb/fb.h>


#define	LCD_WIDTH		(800)
#define	LCD_HEIGHT		(480)
#define	LCD_BPP			(16)
#define	LCD_FORMAT		(FORMAT_RGB_565)

#define HBP				(2)
#define HFP				(2)
#define HSW				(2)
#define VBP				(2)
#define VFP				(2)
#define VSW				(2)
#define	REGS_TIM0		( (HBP<<24) | (HFP<<16) | (HSW<<8) | ((LCD_WIDTH/16-1)<<2) )
#define	REGS_TIM1		( (VBP<<24) | (VFP<<16) | (VSW<<10) | ((LCD_HEIGHT-1)<<0) )
#define	REGS_TIM2		( (1<<26) | ((LCD_WIDTH/16-1)<<16) | (1<<5) | (1<<0) )
#define	REGS_TIM3		( (0<<0) )

/*
 * video ram buffer for lcd.
 */
static x_u8 vram[LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8 + MEM_ALIGNMENT];
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
	/* setting vram base address */
	info.base = (void *)MEM_ALIGN(&vram);
	pvram = (x_u16 *)((x_u32)info.base);

	/* initial lcd controller */
	writel(REALVIEW_CLCD_TIM0, REGS_TIM0);
	writel(REALVIEW_CLCD_TIM1, REGS_TIM1);
	writel(REALVIEW_CLCD_TIM2, REGS_TIM2);
	writel(REALVIEW_CLCD_TIM3, REGS_TIM3);

	/* framebuffer base address */
	writel(REALVIEW_CLCD_UBAS, ((x_u32)info.base));
	writel(REALVIEW_CLCD_LBAS, ((x_u32)info.base + LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8));

	/* disable all lcd interrupts */
	writel(REALVIEW_CLCD_IMSC, 0x0);

	/* set lcd controller register */
	writel(REALVIEW_CLCD_CNTL, REALVIEW_CNTL_BGR | REALVIEW_CNTL_LCDTFT | REALVIEW_CNTL_LCDBPP16);

	/* enable lcd output */
	writel(REALVIEW_CLCD_CNTL, (readl(REALVIEW_CLCD_CNTL) | REALVIEW_CNTL_LCDEN | REALVIEW_CNTL_LCDPWR));
}

static void fb_exit(void)
{
	return;
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

static struct fb realview_fb = {
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

static __init void realview_fb_init(void)
{
	if(!register_framebuffer(&realview_fb))
		DEBUG_E("failed to register framebuffer driver '%s'", realview_fb.info->name);
}

static __exit void realview_fb_exit(void)
{
	if(!unregister_framebuffer(&realview_fb))
		DEBUG_E("failed to unregister framebuffer driver '%s'", realview_fb.info->name);
}

module_init(realview_fb_init, LEVEL_DRIVER);
module_exit(realview_fb_exit, LEVEL_DRIVER);
