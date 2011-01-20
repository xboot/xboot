/*
 * resource/res-fb.c
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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
#include <io.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <s5pv210/reg-gpio.h>
#include <s5pv210-fb.h>

static void lcd_init(void)
{
	/*
	 * set gpd0_0 (backlight pin) output and pull up and high level for disabled
	 */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0x3<<0)) | (0x1<<0));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));

	/*
	 * gpf3_5 high level for enable lcd power
	 */
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0x3<<20)) | (0x1<<20));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0x3<<10)) | (0x2<<10));
	writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<5)) | (0x1<<5));

	/*
	 * gpf3_4 high level for reset pin
	 */
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0x3<<16)) | (0x1<<16));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0x3<<8)) | (0x2<<8));
	writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<4)) | (0x1<<4));

	/*
	 * wait a moment
	 */
	mdelay(10);
}

static void lcd_exit(void)
{
	/*
	 * set gpd0_0 (backlight pin) output and pull up and high level for disabled
	 */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0x3<<0)) | (0x1<<0));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));

	/*
	 * gpf3_5 low level for disable lcd power
	 */
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0x3<<20)) | (0x1<<20));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0x3<<10)) | (0x2<<10));
	writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<5)) | (0x1<<5));
}

static void lcd_backlight(x_u8 brightness)
{
	if(brightness)
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x0<<0));
	else
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));
}

/*
 * lcd module - EK070TN93
 */
static x_u8 vram[800 * 480 * 32 / 8] __attribute__((aligned(4)));

static struct s5pv210fb_lcd lcd = {
	.width			= 800,
	.height			= 480,
	.bpp			= 32,
	.freq			= 60,

	.output			= S5PV210FB_OUTPUT_RGB,
	.rgb_mode		= S5PV210FB_MODE_BGR_P,
	.bpp_mode		= S5PV210FB_BPP_MODE_32BPP,
	.swap			= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask		= 8,
		.r_field	= 0,
		.g_mask		= 8,
		.g_field	= 8,
		.b_mask		= 8,
		.b_field	= 16,
		.a_mask		= 8,
		.a_field	= 24,
	},

	.timing = {
		.h_fp		= 210,
		.h_bp		= 46,
		.h_sw		= 10,
		.v_fp		= 22,
		.v_fpe		= 1,
		.v_bp		= 23,
		.v_bpe		= 1,
		.v_sw		= 7,
	},

	.polarity = {
		.rise_vclk	= 0,
		.inv_hsync	= 1,
		.inv_vsync	= 1,
		.inv_vden	= 0,
	},

	.vram			= &vram[0],

	.init			= lcd_init,
	.exit			= lcd_exit,
	.backlight		= lcd_backlight,
};

static struct resource fb_res = {
	.name		= "fb",
	.data		= &lcd,
};

static __init void res_fb_init(void)
{
	if(!register_resource(&fb_res))
		LOG_E("failed to register resource '%s'", fb_res.name);
}

static __exit void res_fb_exit(void)
{
	if(!unregister_resource(&fb_res))
		LOG_E("failed to unregister resource '%s'", fb_res.name);
}

module_init(res_fb_init, LEVEL_MACH_RES);
module_exit(res_fb_exit, LEVEL_MACH_RES);
