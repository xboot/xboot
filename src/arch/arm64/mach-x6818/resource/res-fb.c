/*
 * resource/res-fb.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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
#include <s5p6818-fb.h>

struct lcd_backlight_data_t {
	struct pwm_t * pwm;
	u32_t period;
	u32_t duty_from;
	u32_t duty_to;
	int polarity;
	int brightness;
};

static void lcd_init(struct s5p6818_fb_data_t * dat)
{
	struct lcd_backlight_data_t * bl;

	bl = malloc(sizeof(struct lcd_backlight_data_t));
	if(!bl)
		return;

	bl->pwm = search_pwm("pwm0");
	bl->period = 1000 * 1000;
	bl->duty_from = 0;
	bl->duty_to = 1000 * 1000;
	bl->polarity = 1;
	bl->brightness = 0;
	dat->priv = bl;

	pwm_config(bl->pwm, bl->duty_from, bl->period, bl->polarity ? TRUE : FALSE);
	pwm_enable(bl->pwm);

	/* Enable backlight power */
	gpio_set_cfg(S5P6818_GPIOC(10), 0x1);
	gpio_set_pull(S5P6818_GPIOC(10), GPIO_PULL_UP);
	gpio_direction_output(S5P6818_GPIOC(10), 1);
}

static void lcd_exit(struct s5p6818_fb_data_t * dat)
{
	struct lcd_backlight_data_t * bl = (struct lcd_backlight_data_t *)(dat->priv);
	if(bl)
		pwm_disable(bl->pwm);
}

static void lcd_setbl(struct s5p6818_fb_data_t * dat, int brightness)
{
	struct lcd_backlight_data_t * bl = (struct lcd_backlight_data_t *)(dat->priv);
	if(bl)
	{
		bl->brightness = brightness;
		pwm_config(bl->pwm, bl->duty_from + (bl->duty_to - bl->duty_from) * bl->brightness / (CONFIG_MAX_BRIGHTNESS + 1), bl->period, bl->polarity);
		pwm_enable(bl->pwm);
	}
}

static int lcd_getbl(struct s5p6818_fb_data_t * dat)
{
	struct lcd_backlight_data_t * bl = (struct lcd_backlight_data_t *)(dat->priv);
	if(bl)
		return bl->brightness;
	return 0;
}

#if defined(CONFIG_VGA_1024X768)
static struct s5p6818_fb_data_t s5p6818_fb_data = {
	.dpcbase				= S5P6818_DPC0_BASE,
	.mlcbase				= S5P6818_MLC0_BASE,

	.width					= 1024,
	.height					= 768,
	.xdpi					= 133,
	.ydpi					= 133,
	.bits_per_pixel			= 32,
	.bytes_per_pixel		= 4,

	.mode = {
		.rgbmode			= 1,
		.scanmode			= 0,
		.enbedsync			= 0,
		.polfield			= 0,
		.swaprb				= 0,
		.format				= S5P6818_DPC_FORMAT_RGB888,
		.ycorder			= S5P6818_DPC_YCORDER_CBYCRY,
		.rdither			= S5P6818_DPC_DITHER_BYPASS,
		.gdither			= S5P6818_DPC_DITHER_BYPASS,
		.bdither			= S5P6818_DPC_DITHER_BYPASS,
	},

	.timing = {
		.pixel_clock_hz		= 65 * 1000 * 1000,
		.h_front_porch		= 24,
		.h_sync_width		= 136,
		.h_back_porch		= 160,
		.v_front_porch		= 3,
		.v_sync_width		= 6,
		.v_back_porch		= 29,

		.vs_start_offset	= 1,
		.vs_end_offset		= 1,
		.ev_start_offset	= 1,
		.ev_end_offset		= 1,

		.d_rgb_pvd			= 0,
		.d_hsync_cp1		= 7,
		.d_vsync_fram		= 7,
		.d_de_cp2			= 7,
	},

	.polarity = {
		.h_sync_invert		= 0,
		.v_sync_invert		= 0,
	},

	.init					= lcd_init,
	.exit					= lcd_exit,
	.setbl					= lcd_setbl,
	.getbl					= lcd_getbl,
};
#else
static struct s5p6818_fb_data_t s5p6818_fb_data = {
	.dpcbase				= S5P6818_DPC0_BASE,
	.mlcbase				= S5P6818_MLC0_BASE,

	.width					= 1024,
	.height					= 600,
	.xdpi					= 133,
	.ydpi					= 133,
	.bits_per_pixel			= 32,
	.bytes_per_pixel		= 4,

	.mode = {
		.rgbmode			= 1,
		.scanmode			= 0,
		.enbedsync			= 0,
		.polfield			= 0,
		.swaprb				= 0,
		.format				= S5P6818_DPC_FORMAT_RGB888,
		.ycorder			= S5P6818_DPC_YCORDER_CBYCRY,
		.rdither			= S5P6818_DPC_DITHER_BYPASS,
		.gdither			= S5P6818_DPC_DITHER_BYPASS,
		.bdither			= S5P6818_DPC_DITHER_BYPASS,
	},

	.timing = {
		.pixel_clock_hz		= 52 * 1000 * 1000,
		.h_front_porch		= 160,
		.h_sync_width		= 20,
		.h_back_porch		= 140,
		.v_front_porch		= 12,
		.v_sync_width		= 3,
		.v_back_porch		= 20,

		.vs_start_offset	= 1,
		.vs_end_offset		= 1,
		.ev_start_offset	= 1,
		.ev_end_offset		= 1,

		.d_rgb_pvd			= 0,
		.d_hsync_cp1		= 7,
		.d_vsync_fram		= 7,
		.d_de_cp2			= 7,
	},

	.polarity = {
		.h_sync_invert		= 1,
		.v_sync_invert		= 1,
	},

	.init					= lcd_init,
	.exit					= lcd_exit,
	.setbl					= lcd_setbl,
	.getbl					= lcd_getbl,
};
#endif

static struct resource_t res_fb = {
	.name		= "s5p6818-fb",
	.id			= -1,
	.data		= &s5p6818_fb_data,
};

static __init void resource_fb_init(void)
{
	if(register_resource(&res_fb))
		LOG("Register resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
	else
		LOG("Failed to register resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
}

static __exit void resource_fb_exit(void)
{
	if(unregister_resource(&res_fb))
		LOG("Unregister resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
	else
		LOG("Failed to unregister resource %s:'%s.%d'", res_fb.mach, res_fb.name, res_fb.id);
}

resource_initcall(resource_fb_init);
resource_exitcall(resource_fb_exit);
