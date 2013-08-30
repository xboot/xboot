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
#include <s5pv210-fb.h>

static void lcd_init(struct s5pv210_fb_data_t * dat)
{
	/*
	 * set gpd0_0 (backlight pwm pin) output and pull up and high level for disabled
	 */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<0)) | (0x1<<0));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));

	/*
	 * set gpf3_5 (backlight enable pin) output and pull up and low level for disabled
	 */
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0xf<<20)) | (0x1<<20));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0x3<<10)) | (0x2<<10));
	writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<5)) | (0x0<<5));

	/*
	 * wait a moment
	 */
	mdelay(10);
}

static void lcd_exit(struct s5pv210_fb_data_t * dat)
{
	/*
	 * set gpd0_0 (backlight pwm pin) output and pull up and high level for disabled
	 */
	writel(S5PV210_GPD0CON, (readl(S5PV210_GPD0CON) & ~(0xf<<0)) | (0x1<<0));
	writel(S5PV210_GPD0PUD, (readl(S5PV210_GPD0PUD) & ~(0x3<<0)) | (0x2<<0));
	writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));

	/*
	 * set gpf3_5 (backlight enable pin) output and pull up and low level for disabled
	 */
	writel(S5PV210_GPF3CON, (readl(S5PV210_GPF3CON) & ~(0xf<<20)) | (0x1<<20));
	writel(S5PV210_GPF3PUD, (readl(S5PV210_GPF3PUD) & ~(0x3<<10)) | (0x2<<10));
	writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<5)) | (0x0<<5));
}

static void lcd_set_backlight(struct s5pv210_fb_data_t * dat, int brightness)
{
	if(brightness < 0)
		brightness = 0;
	if(brightness > 255)
		brightness = 255;
	dat->brightness = brightness;

	if(dat->brightness)
	{
		writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<5)) | (0x1<<5));
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x0<<0));
	}
	else
	{
		writel(S5PV210_GPF3DAT, (readl(S5PV210_GPF3DAT) & ~(0x1<<5)) | (0x0<<5));
		writel(S5PV210_GPD0DAT, (readl(S5PV210_GPD0DAT) & ~(0x1<<0)) | (0x1<<0));
	}
}

static int lcd_get_backlight(struct s5pv210_fb_data_t * dat)
{
	return dat->brightness;
}

/*
 * lcd module - EK070TN93
 */
static struct s5pv210_fb_data_t s5pv210_fb_data = {
	.width				= 800,
	.height				= 480,
	.bits_per_pixel		= 32,
	.bytes_per_pixel	= 4,
	.freq				= 60,

	.output				= S5PV210FB_OUTPUT_RGB,
	.rgb_mode			= S5PV210FB_MODE_RGB_P,
	.bpp_mode			= S5PV210FB_BPP_MODE_32BPP,
	.swap				= S5PV210FB_SWAP_WORD,

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

	.init				= lcd_init,
	.exit				= lcd_exit,

	.brightness			= 0,
	.set_backlight		= lcd_set_backlight,
	.get_backlight		= lcd_get_backlight,
};

static struct resource_t res_fb = {
	.name		= "fb-s5pv210",
	.id			= -1,
	.data		= &s5pv210_fb_data,
};

static __init void resource_fb_init(void)
{
	if(register_resource(&res_fb))
		LOG("Register resource '%s.%d'", res_fb.name, res_fb.id);
	else
		LOG("Failed to register resource '%s.%d'", res_fb.name, res_fb.id);
}

static __exit void resource_fb_exit(void)
{
	if(unregister_resource(&res_fb))
		LOG("Unregister resource '%s.%d'", res_fb.name, res_fb.id);
	else
		LOG("Failed to unregister resource '%s.%d'", res_fb.name, res_fb.id);
}

resource_initcall(resource_fb_init);
resource_exitcall(resource_fb_exit);
