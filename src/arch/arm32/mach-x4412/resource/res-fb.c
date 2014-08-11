/*
 * resource/res-fb.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <exynos4412-fb.h>

static void lcd_init(struct exynos4412_fb_data_t * dat)
{
	/*
	 * Set GPD0_0 (backlight pwm pin) output and pull up and high level for disabled
	 */
	gpio_set_pull(EXYNOS4412_GPD0(0), GPIO_PULL_UP);
	gpio_direction_output(EXYNOS4412_GPD0(0), 1);

	/*
	 * Set GPX3_5 (backlight enable pin) output and pull up and low level for disabled
	 */
	gpio_set_pull(EXYNOS4412_GPX3(5), GPIO_PULL_UP);
	gpio_direction_output(EXYNOS4412_GPX3(5), 0);

	/*
	 * Wait a moment
	 */
	mdelay(10);
}

static void lcd_exit(struct exynos4412_fb_data_t * dat)
{
	/*
	 * Set GPD0_0 (backlight pwm pin) output and pull up and high level for disabled
	 */
	gpio_set_pull(EXYNOS4412_GPD0(0), GPIO_PULL_UP);
	gpio_direction_output(EXYNOS4412_GPD0(0), 1);

	/*
	 * Set GPX3_5 (backlight enable pin) output and pull up and low level for disabled
	 */
	gpio_set_pull(EXYNOS4412_GPX3(5), GPIO_PULL_UP);
	gpio_direction_output(EXYNOS4412_GPX3(5), 0);
}

static void lcd_set_backlight(struct exynos4412_fb_data_t * dat, int brightness)
{
	dat->brightness = brightness;
	if(dat->brightness)
	{
		gpio_direction_output(EXYNOS4412_GPX3(5), 1);
		gpio_direction_output(EXYNOS4412_GPD0(0), 0);
	}
	else
	{
		gpio_direction_output(EXYNOS4412_GPX3(5), 0);
		gpio_direction_output(EXYNOS4412_GPD0(0), 1);
	}
}

static int lcd_get_backlight(struct exynos4412_fb_data_t * dat)
{
	return dat->brightness;
}

/*
 * lcd module - VS070CXN
 */
static struct exynos4412_fb_data_t exynos4412_fb_data = {
	.regbase			= EXYNOS4412_LCD_BASE,

	.width				= 1024,
	.height				= 600,
	.xdpi				= 133,
	.ydpi				= 133,
	.bits_per_pixel		= 32,
	.bytes_per_pixel	= 4,
	.freq				= 60,

	.output				= EXYNOS4412_FB_OUTPUT_RGB,
	.rgb_mode			= EXYNOS4412_FB_MODE_RGB_P,
	.bpp_mode			= EXYNOS4412_FB_BPP_MODE_32BPP,
	.swap				= EXYNOS4412_FB_SWAP_WORD,

	.timing = {
		.h_fp			= 160,
		.h_bp			= 140,
		.h_sw			= 20,
		.v_fp			= 12,
		.v_fpe			= 1,
		.v_bp			= 20,
		.v_bpe			= 1,
		.v_sw			= 3,
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
	.name		= "exynos4412-fb",
	.id			= -1,
	.data		= &exynos4412_fb_data,
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
