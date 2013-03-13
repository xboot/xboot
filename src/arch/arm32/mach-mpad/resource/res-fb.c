/*
 * resource/res-fb.c
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
#include <types.h>
#include <io.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <time/delay.h>
#include <s5pv210/reg-gpio.h>
#include <s5pv210-fb.h>

static void lcd_init(void)
{
	/*
	 * set gpd0_0 (backlight pwm pin) output and pull up and high level for disabled
	 */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<0)) | (0x1<<0));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));

	/*
	 * set gph2_4 (backlight enable pin) output and pull up and low level for disabled
	 */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<16)) | (0x1<<16));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<8)) | (0x2<<8));
	writel(S5PV210_GPH2DAT, (readl(S5PV210_GPH2DAT) & ~(0x1<<4)) | (0x0<<4));

	/*
	 * set gph1_6 (LCD_5V) output and pull up and high level for enabled
	 */
	writel(S5PV210_GPH1CON, (readl(S5PV210_GPH1CON) & ~(0xf<<24)) | (0x1<<24));
	writel(S5PV210_GPH1PUD, (readl(S5PV210_GPH1PUD) & ~(0x3<<12)) | (0x2<<12));
	writel(S5PV210_GPH1DAT, (readl(S5PV210_GPH1DAT) & ~(0x1<<6)) | (0x1<<6));

	/*
	 * set gph1_7 (LCD_33) output and pull up and low level for enabled
	 */
	writel(S5PV210_GPH1CON, (readl(S5PV210_GPH1CON) & ~(0xf<<28)) | (0x1<<28));
	writel(S5PV210_GPH1PUD, (readl(S5PV210_GPH1PUD) & ~(0x3<<14)) | (0x2<<14));
	writel(S5PV210_GPH1DAT, (readl(S5PV210_GPH1DAT) & ~(0x1<<7)) | (0x0<<7));

	/*
	 * wait a moment
	 */
	mdelay(100);
}

static void lcd_exit(void)
{
	/*
	 * set gph2_4 (backlight enable pin) output and pull up and low level for disabled
	 */
	writel(S5PV210_GPH2CON, (readl(S5PV210_GPH2CON) & ~(0xf<<16)) | (0x1<<16));
	writel(S5PV210_GPH2PUD, (readl(S5PV210_GPH2PUD) & ~(0x3<<2)) | (0x2<<8));
	writel(S5PV210_GPH2DAT, (readl(S5PV210_GPH2DAT) & ~(0x1<<1)) | (0x0<<1));

	/*
	 * set gph1_6 (LCD_5V) output and pull up and low level for disabled
	 */
	writel(S5PV210_GPH1CON, (readl(S5PV210_GPH1CON) & ~(0xf<<24)) | (0x1<<24));
	writel(S5PV210_GPH1PUD, (readl(S5PV210_GPH1PUD) & ~(0x3<<12)) | (0x2<<12));
	writel(S5PV210_GPH1DAT, (readl(S5PV210_GPH1DAT) & ~(0x1<<6)) | (0x0<<6));

	/*
	 * set gph1_7 (LCD_33) output and pull up and low level for disabled
	 */
	writel(S5PV210_GPH1CON, (readl(S5PV210_GPH1CON) & ~(0xf<<28)) | (0x1<<28));
	writel(S5PV210_GPH1PUD, (readl(S5PV210_GPH1PUD) & ~(0x3<<14)) | (0x2<<14));
	writel(S5PV210_GPH1DAT, (readl(S5PV210_GPH1DAT) & ~(0x1<<7)) | (0x0<<7));
}

static void lcd_backlight(u8_t brightness)
{
	if(brightness)
	{
		writel(S5PV210_GPH2DAT, (readl(S5PV210_GPH2DAT) & ~(0x1<<4)) | (0x1<<4));
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x0<<0));
	}
	else
	{
		writel(S5PV210_GPH2DAT, (readl(S5PV210_GPH2DAT) & ~(0x1<<4)) | (0x0<<4));
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));
	}
}

/*
 * lcd module - EK070TN93
 */
static u8_t vram[2][800 * 480 * 32 / 8] __attribute__((aligned(4)));

static struct s5pv210fb_lcd lcd = {
	.width				= 800,
	.height				= 480,
	.bits_per_pixel		= 32,
	.bytes_per_pixel	= 4,
	.freq				= 60,

	.output				= S5PV210FB_OUTPUT_RGB,
	.rgb_mode			= S5PV210FB_MODE_BGR_P,
	.bpp_mode			= S5PV210FB_BPP_MODE_32BPP,
	.swap				= S5PV210FB_SWAP_WORD,

	.rgba = {
		.r_mask			= 8,
		.r_field		= 0,
		.g_mask			= 8,
		.g_field		= 8,
		.b_mask			= 8,
		.b_field		= 16,
		.a_mask			= 8,
		.a_field		= 24,
	},

	.timing = {
		.h_fp			= 210,
		.h_bp			= 38,
		.h_sw			= 10,
		.v_fp			= 22,
		.v_fpe			= 1,
		.v_bp			= 18,
		.v_bpe			= 1,
		.v_sw			= 7,
	},

	.polarity = {
		.rise_vclk		= 0,
		.inv_hsync		= 1,
		.inv_vsync		= 1,
		.inv_vden		= 0,
	},

	.vram_front			= &vram[0][0],
	.vram_back			= &vram[1][0],

	.init				= lcd_init,
	.exit				= lcd_exit,
	.backlight			= lcd_backlight,
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

core_initcall(res_fb_init);
core_exitcall(res_fb_exit);
