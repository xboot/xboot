/*
 * driver/s5p6818-fb.c
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

#include <s5p6818-fb.h>

static inline void s5p6818_mlc_pclk_bclk_enable(struct s5p6818_fb_data_t * dat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->mlcbase + MLC_CLKENB));
	cfg &= ~((0x1 << 3) | (0x3 << 0));
	if(enable)
		cfg |= ((0x1 << 3) | (0x3 << 0));
	else
		cfg |= ((0x0 << 3) | (0x0 << 0));
	write32(phys_to_virt(dat->mlcbase + MLC_CLKENB), cfg);
}

static inline void s5p6818_mlc_set_enable(struct s5p6818_fb_data_t * dat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROLT));
	cfg &= ~(0x1 << 1 | 0x1 << 3);
	if(enable)
		cfg |= (0x1 << 1);
	write32(phys_to_virt(dat->mlcbase + MLC_CONTROLT), cfg);
}

static inline void s5p6818_mlc_set_power_mode(struct s5p6818_fb_data_t * dat, bool_t on)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROLT));
	cfg &= ~(0x1 << 11 | 0x1 << 3);
	if(on)
		cfg |= (0x1 << 11);
	write32(phys_to_virt(dat->mlcbase + MLC_CONTROLT), cfg);
}

static inline void s5p6818_mlc_set_sleep_mode(struct s5p6818_fb_data_t * dat, bool_t on)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROLT));
	cfg &= ~(0x1 << 10 | 0x1 << 3);
	if(!on)
		cfg |= (0x1 << 10);
	write32(phys_to_virt(dat->mlcbase + MLC_CONTROLT), cfg);
}

static inline void s5p6818_mlc_set_top_dirty_flag(struct s5p6818_fb_data_t * dat)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROLT));
	cfg |= (0x1 << 3);
	write32(phys_to_virt(dat->mlcbase + MLC_CONTROLT), cfg);
}

static inline void s5p6818_mlc_set_layer_priority(struct s5p6818_fb_data_t * dat, int priority)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROLT));
	cfg &= ~(0x3 << 8 | 0x1 << 3);
	cfg |= ((priority & 0x3) << 8);
	write32(phys_to_virt(dat->mlcbase + MLC_CONTROLT), cfg);
}

static inline void s5p6818_mlc_set_field_enable(struct s5p6818_fb_data_t * dat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROLT));
	cfg &= ~(0x1 << 0 | 0x1 << 3);
	if(enable)
		cfg |= (0x1 << 0);
	write32(phys_to_virt(dat->mlcbase + MLC_CONTROLT), cfg);
}

static inline void s5p6818_mlc_set_screen_size(struct s5p6818_fb_data_t * dat, u32_t width, u32_t height)
{
	u32_t cfg;

	cfg = (((height - 1) & 0x3ff) << 16) | (((width - 1) & 0x3ff) << 0);
	write32(phys_to_virt(dat->mlcbase + MLC_SCREENSIZE), cfg);
}

static inline void s5p6818_mlc_set_background_color(struct s5p6818_fb_data_t * dat, u32_t color)
{
	write32(phys_to_virt(dat->mlcbase + MLC_BGCOLOR), color);
}

static inline void s5p6818_mlc_set_layer_enable(struct s5p6818_fb_data_t * dat, int layer, bool_t enable)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0));
		cfg &= ~(0x1 << 5 | 0x1 << 4);
		if(enable)
			cfg |= (0x1 << 5);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL0), cfg);
		break;

	case 1:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1));
		cfg &= ~(0x1 << 5 | 0x1 << 4);
		if(enable)
			cfg |= (0x1 << 5);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL1), cfg);
		break;

	case 2:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL2));
		cfg &= ~(0x1 << 5 | 0x1 << 4);
		if(enable)
			cfg |= (0x1 << 5);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL2), cfg);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_dirty_flag(struct s5p6818_fb_data_t * dat, int layer)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0));
		cfg |= (0x1 << 4);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL0), cfg);
		break;

	case 1:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1));
		cfg |= (0x1 << 4);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL1), cfg);
		break;

	case 2:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL2));
		cfg |= (0x1 << 4);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL2), cfg);
		break;

	default:
		break;
	}
}

static inline bool_t s5p6818_mlc_get_dirty_flag(struct s5p6818_fb_data_t * dat, int layer)
{
	switch(layer)
	{
	case 0:
		return (read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0)) & (0x1 << 4)) ? TRUE : FALSE;
	case 1:
		return (read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1)) & (0x1 << 4)) ? TRUE : FALSE;
	case 2:
		return (read32(phys_to_virt(dat->mlcbase + MLC_CONTROL2)) & (0x1 << 4)) ? TRUE : FALSE;
	default:
		break;
	}
	return FALSE;
}

static inline void s5p6818_mlc_wait_vsync(struct s5p6818_fb_data_t * dat, int layer)
{
	while(s5p6818_mlc_get_dirty_flag(dat, layer));
}

static inline void s5p6818_mlc_set_lock_size(struct s5p6818_fb_data_t * dat, int layer, int size)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0));
		cfg &= ~(0x3 << 12 | 0x1 << 4);
		cfg |= (((size >> 8) & 0x3) << 12);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL0), cfg);
		break;

	case 1:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1));
		cfg &= ~(0x3 << 12 | 0x1 << 4);
		cfg |= (((size >> 8) & 0x3) << 12);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL1), cfg);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_alpha_blending(struct s5p6818_fb_data_t * dat, int layer, bool_t enable, u32_t alpha)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0));
		cfg &= ~(0x1 << 2 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 2);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL0), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR0));
		cfg &= ~(0xff << 24);
		cfg |= ((alpha & 0xff) << 24);
		write32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR0), cfg);
		break;

	case 1:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1));
		cfg &= ~(0x1 << 2 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 2);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL1), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR1));
		cfg &= ~(0xff << 24);
		cfg |= ((alpha & 0xff) << 24);
		write32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR1), cfg);
		break;

	case 2:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL2));
		cfg &= ~(0x1 << 2 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 2);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL2), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR2));
		cfg &= ~(0xff << 24);
		cfg |= ((alpha & 0xff) << 24);
		write32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR2), cfg);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_transparency(struct s5p6818_fb_data_t * dat, int layer, bool_t enable, u32_t color)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0));
		cfg &= ~(0x1 << 0 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL0), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR0));
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR0), cfg);
		break;

	case 1:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1));
		cfg &= ~(0x1 << 0 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL1), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR1));
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR1), cfg);
		break;

	case 2:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL2));
		cfg &= ~(0x1 << 0 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL2), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR2));
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_TPCOLOR2), cfg);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_color_inversion(struct s5p6818_fb_data_t * dat, int layer, bool_t enable, u32_t color)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0));
		cfg &= ~(0x1 << 1 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 1);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL0), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_INVCOLOR0));
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_INVCOLOR0), cfg);
		break;

	case 1:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1));
		cfg &= ~(0x1 << 1 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 1);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL1), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_INVCOLOR1));
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_INVCOLOR1), cfg);
		break;

	case 2:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL2));
		cfg &= ~(0x1 << 1 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 1);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL2), cfg);

		cfg = read32(phys_to_virt(dat->mlcbase + MLC_INVCOLOR2));
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(phys_to_virt(dat->mlcbase + MLC_INVCOLOR2), cfg);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_layer_invalid_position(struct s5p6818_fb_data_t * dat, int layer, u32_t region, s32_t sx, s32_t sy, s32_t ex, s32_t ey, bool_t enable)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		if(region == 0)
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDLEFTRIGHT0_0), cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDTOPBOTTOM0_0), cfg);
		}
		else
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDLEFTRIGHT0_1), cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDTOPBOTTOM0_1), cfg);
		}
		break;

	case 1:
		if(region == 0)
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDLEFTRIGHT1_0), cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDTOPBOTTOM1_0), cfg);
		}
		else
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDLEFTRIGHT1_1), cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(phys_to_virt(dat->mlcbase + MLC_INVALIDTOPBOTTOM1_1), cfg);
		}
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_rgb_format(struct s5p6818_fb_data_t * dat, int layer, enum s5p6818_mlc_rgbfmt_t fmt)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL0));
		cfg &= ~(0xffff << 16 | 0x1 << 4);
		cfg |= (fmt << 16);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL0), cfg);
		break;

	case 1:
		cfg = read32(phys_to_virt(dat->mlcbase + MLC_CONTROL1));
		cfg &= ~(0xffff << 16 | 0x1 << 4);
		cfg |= (fmt << 16);
		write32(phys_to_virt(dat->mlcbase + MLC_CONTROL1), cfg);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_position(struct s5p6818_fb_data_t * dat, int layer, s32_t sx, s32_t sy, s32_t ex, s32_t ey)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = ((sx & 0xfff) << 16) | (ex & 0xfff);
		write32(phys_to_virt(dat->mlcbase + MLC_LEFTRIGHT0), cfg);
		cfg = ((sy & 0xfff) << 16) | (ey & 0xfff);
		write32(phys_to_virt(dat->mlcbase + MLC_TOPBOTTOM0), cfg);
		break;

	case 1:
		cfg = ((sx & 0xfff) << 16) | (ex & 0xfff);
		write32(phys_to_virt(dat->mlcbase + MLC_LEFTRIGHT1), cfg);
		cfg = ((sy & 0xfff) << 16) | (ey & 0xfff);
		write32(phys_to_virt(dat->mlcbase + MLC_TOPBOTTOM1), cfg);
		break;

	case 2:
		cfg = ((sx & 0xfff) << 16) | (ex & 0xfff);
		write32(phys_to_virt(dat->mlcbase + MLC_LEFTRIGHT2), cfg);
		cfg = ((sy & 0xfff) << 16) | (ey & 0xfff);
		write32(phys_to_virt(dat->mlcbase + MLC_TOPBOTTOM2), cfg);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_layer_stride(struct s5p6818_fb_data_t * dat, int layer, s32_t hstride, s32_t vstride)
{
	switch(layer)
	{
	case 0:
		write32(phys_to_virt(dat->mlcbase + MLC_HSTRIDE0), hstride);
		write32(phys_to_virt(dat->mlcbase + MLC_VSTRIDE0), vstride);
		break;

	case 1:
		write32(phys_to_virt(dat->mlcbase + MLC_HSTRIDE1), hstride);
		write32(phys_to_virt(dat->mlcbase + MLC_VSTRIDE1), vstride);
		break;

	default:
		break;
	}
}

static inline void s5p6818_mlc_set_layer_address(struct s5p6818_fb_data_t * dat, int layer, void * vram)
{
	switch(layer)
	{
	case 0:
		write32(phys_to_virt(dat->mlcbase + MLC_ADDRESS0), (u32_t)((u64_t)vram));
		break;
	case 1:
		write32(phys_to_virt(dat->mlcbase + MLC_ADDRESS1), (u32_t)(u64_t)vram);
		break;
	case 2:
		write32(phys_to_virt(dat->mlcbase + MLC_ADDRESS2), (u32_t)(u64_t)vram);
		break;
	default:
		break;
	}
}

static inline void s5p6818_dpc_pclk_enable(struct s5p6818_fb_data_t * dat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CLKENB));
	cfg &= ~((0x1 << 3) | (0x1 << 2));
	if(enable)
		cfg |= ((0x1 << 3) | (0x1 << 2));
	else
		cfg |= ((0x0 << 3) | (0x0 << 2));
	write32(phys_to_virt(dat->dpcbase + DPC_CLKENB), cfg);
}

static inline void s5p6818_dpc_set_clock(struct s5p6818_fb_data_t * dat)
{
	u64_t rate;
	u32_t cfg;
	int i;

	rate = clk_get_rate("PLL2");

	/* DPC_CLKGEN0L */
	for(i = 0; i < 256; i++)
	{
		if((rate / (i + 1)) <= dat->timing.pixel_clock_hz)
			break;
	}
	rate = rate / (i + 1);

	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CLKGEN0L));
	cfg &= ~((0x7 << 2) | (0xff << 5));
	cfg |= 0x2 << 2;
	cfg |= (i & 0xff) << 5;
	write32(phys_to_virt(dat->dpcbase + DPC_CLKGEN0L), cfg);

	/* DPC_CLKGEN1L */
	for(i = 0; i < 256; i++)
	{
		if((rate / (i + 1)) <= dat->timing.pixel_clock_hz)
			break;
	}
	rate = rate / (i + 1);

	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CLKGEN1L));
	cfg &= ~((0x7 << 2) | (0xff << 5));
	cfg |= 0x7 << 2;
	cfg |= (i & 0xff) << 5;
	write32(phys_to_virt(dat->dpcbase + DPC_CLKGEN1L), cfg);

	/* OUTCLKDELAY */
	write32(phys_to_virt(dat->dpcbase + DPC_CLKGEN0H), 0x00000000);
	write32(phys_to_virt(dat->dpcbase + DPC_CLKGEN1H), 0x00000000);
}

static inline void s5p6818_dpc_set_mode(struct s5p6818_fb_data_t * dat)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CTRL0));
	cfg &= ~(0x1 << 12);
	cfg |= ((dat->mode.rgbmode ? 1 : 0) << 12);
	cfg &= ~(0x1 << 9);
	cfg |= ((dat->mode.scanmode ? 1 : 0) << 9);
	cfg &= ~(0x1 << 8);
	cfg |= ((dat->mode.enbedsync ? 1 : 0) << 8);
	cfg &= ~(0x1 << 2);
	cfg |= ((dat->mode.polfield ? 1 : 0) << 2);
	write32(phys_to_virt(dat->dpcbase + DPC_CTRL0), cfg);

	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CTRL1));
	cfg &= ~(0x1 << 15);
	cfg |= ((dat->mode.swaprb ? 1 : 0) << 15);
	cfg &= ~(0xf << 8);
	cfg |= (dat->mode.format << 8);
	cfg &= ~(0x3 << 6);
	cfg |= (dat->mode.ycorder << 6);
	cfg &= ~(0x3f << 0);
	cfg |= ((dat->mode.rdither << 0) | (dat->mode.gdither << 2) | (dat->mode.bdither << 4));
	write32(phys_to_virt(dat->dpcbase + DPC_CTRL1), cfg);

	cfg = 0x0;
	write32(phys_to_virt(dat->dpcbase + DPC_CTRL2), cfg);
}

static inline void s5p6818_dpc_set_timing(struct s5p6818_fb_data_t * dat)
{
	u32_t cfg;

	/* horizontal */
	cfg = dat->width + dat->timing.h_front_porch + dat->timing.h_sync_width + dat->timing.h_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_HTOTAL), cfg);

	cfg = dat->timing.h_sync_width - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_HSWIDTH), cfg);

	cfg = dat->timing.h_sync_width + dat->timing.h_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_HASTART), cfg);

	cfg = dat->width + dat->timing.h_sync_width + dat->timing.h_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_HAEND), cfg);

	/* vertical - progressive */
	cfg = dat->height + dat->timing.v_front_porch + dat->timing.v_sync_width + dat->timing.v_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_VTOTAL), cfg);

	cfg = dat->timing.v_sync_width - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_VSWIDTH), cfg);

	cfg = dat->timing.v_sync_width + dat->timing.v_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_VASTART), cfg);

	cfg = dat->height + dat->timing.v_sync_width + dat->timing.v_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_VAEND), cfg);

	/* vertical - interlaced */
	cfg = dat->height + dat->timing.v_front_porch + dat->timing.v_sync_width + dat->timing.v_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_EVTOTAL), cfg);

	cfg = dat->timing.v_sync_width - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_EVSWIDTH), cfg);

	cfg = dat->timing.v_sync_width + dat->timing.v_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_EVASTART), cfg);

	cfg = dat->height + dat->timing.v_sync_width + dat->timing.v_back_porch - 1;
	write32(phys_to_virt(dat->dpcbase + DPC_EVAEND), cfg);

	/* Sync offset */
	write32(phys_to_virt(dat->dpcbase + DPC_VSSOFFSET), dat->timing.vs_start_offset);
	write32(phys_to_virt(dat->dpcbase + DPC_VSEOFFSET), dat->timing.vs_end_offset);
	write32(phys_to_virt(dat->dpcbase + DPC_EVSSOFFSET), dat->timing.ev_start_offset);
	write32(phys_to_virt(dat->dpcbase + DPC_EVSEOFFSET), dat->timing.ev_end_offset);

	/* Delay value */
	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CTRL0));
	cfg &= ~(0x1 << 10);
	cfg &= ~(0xf << 4);
	cfg |= (dat->timing.d_rgb_pvd & 0xf) << 4;
	write32(phys_to_virt(dat->dpcbase + DPC_CTRL0), cfg);

	cfg = ((dat->timing.d_hsync_cp1 & 0xf) << 0) | ((dat->timing.d_vsync_fram & 0xf) << 8);
	write32(phys_to_virt(dat->dpcbase + DPC_DELAY0), cfg);

	cfg = (dat->timing.d_de_cp2 & 0x3f) << 0;
	write32(phys_to_virt(dat->dpcbase + DPC_DELAY1), cfg);
}

static inline void s5p6818_dpc_set_polarity(struct s5p6818_fb_data_t * dat)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CTRL0));
	cfg &= ~(0x1 << 10 | 0x1 << 1 | 0x1 << 0);
	if(dat->polarity.h_sync_invert)
		cfg |= (0x1 << 0);
	if(dat->polarity.v_sync_invert)
		cfg |= (0x1 << 1);
	write32(phys_to_virt(dat->dpcbase + DPC_CTRL0), cfg);
}

static inline void s5p6818_dpc_set_enable(struct s5p6818_fb_data_t * dat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(phys_to_virt(dat->dpcbase + DPC_CTRL0));
	cfg &= ~(0x1 << 10 | 0x1 << 15);
	if(enable)
		cfg |= (0x1 << 15);
	write32(phys_to_virt(dat->dpcbase + DPC_CTRL0), cfg);
}

static inline void s5p6818_dpa_rgb_mux_select(int sel)
{
	write32(phys_to_virt(S5P6818_DPA_BASE + DPA_TFT_MUXCTRL), (sel & 0x3));
}

static inline void s5p6818_fb_cfg_gpios(int base, int nr, int cfg, enum gpio_pull_t pull, enum gpio_drv_t drv)
{
	for(; nr > 0; nr--, base++)
	{
		gpio_set_cfg(base, cfg);
		gpio_set_pull(base, pull);
		gpio_set_drv(base, drv);
	}
}

static void fb_init(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5p6818_fb_data_t * dat = (struct s5p6818_fb_data_t *)res->data;

	s5p6818_ip_reset(RESET_ID_DISP_TOP, 0);
	s5p6818_ip_reset(RESET_ID_DISPLAY, 0);
	s5p6818_ip_reset(RESET_ID_LCDIF, 0);
	s5p6818_ip_reset(RESET_ID_LVDS, 0);

	/*
	 * Initial digital rgb lcd port
	 */
	s5p6818_fb_cfg_gpios(S5P6818_GPIOA(0), 28, 0x1, GPIO_PULL_NONE, GPIO_DRV_HIGH);

	/*
	 * Lcd init function
	 */
	if(dat->init)
		dat->init(dat);

	/*
	 * Enable some clocks
	 */
	s5p6818_mlc_pclk_bclk_enable(dat, TRUE);
	s5p6818_dpc_pclk_enable(dat, TRUE);
	s5p6818_dpc_set_clock(dat);

	/*
	 * Initial mlc top layer
	 */
	s5p6818_mlc_set_layer_priority(dat, 0);
	s5p6818_mlc_set_background_color(dat, 0);
	s5p6818_mlc_set_field_enable(dat, FALSE);
	s5p6818_mlc_set_power_mode(dat, TRUE);
	s5p6818_mlc_set_sleep_mode(dat, FALSE);

	/*
	 * Initial mlc fb layer
	 */
	s5p6818_mlc_set_lock_size(dat, 0, 16);
	s5p6818_mlc_set_alpha_blending(dat, 0, FALSE, 0);
	s5p6818_mlc_set_transparency(dat, 0, FALSE, 0);
	s5p6818_mlc_set_color_inversion(dat, 0, FALSE, 0);
	s5p6818_mlc_set_layer_invalid_position(dat, 0, 0, 0, 0, 0, 0, FALSE);
	s5p6818_mlc_set_layer_invalid_position(dat, 0, 1, 0, 0, 0, 0, FALSE);
	s5p6818_mlc_set_rgb_format(dat, 0, S5P6818_MLC_RGBFMT_X8R8G8B8);
	s5p6818_mlc_set_position(dat, 0, 0, 0, dat->width - 1, dat->height - 1);
	s5p6818_mlc_set_layer_stride(dat, 0, dat->bytes_per_pixel, dat->bytes_per_pixel * dat->width);
	s5p6818_mlc_set_screen_size(dat, dat->width, dat->height);

	/*
	 * Enable mlc controller
	 */
	s5p6818_mlc_set_enable(dat, TRUE);
	s5p6818_mlc_set_top_dirty_flag(dat);
	s5p6818_mlc_set_layer_enable(dat, 0, TRUE);
	s5p6818_mlc_set_dirty_flag(dat, 0);

	/*
	 * Initial dpc
	 */
	s5p6818_dpc_set_mode(dat);
	s5p6818_dpc_set_timing(dat);
	s5p6818_dpc_set_polarity(dat);

	/*
	 * Enable dpc controller
	 */
	s5p6818_dpc_set_enable(dat, TRUE);

	/*
	 * MUX select for primary dpc(TFT)
	 */
	s5p6818_dpa_rgb_mux_select(0);
}

static void fb_exit(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5p6818_fb_data_t * dat = (struct s5p6818_fb_data_t *)res->data;

	if(dat->exit)
		dat->exit(dat);
}

static void fb_setbl(struct fb_t * fb, int brightness)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5p6818_fb_data_t * dat = (struct s5p6818_fb_data_t *)res->data;
	if(dat->setbl)
		dat->setbl(dat, brightness);
}

static int fb_getbl(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5p6818_fb_data_t * dat = (struct s5p6818_fb_data_t *)res->data;
	if(dat->getbl)
		return dat->getbl(dat);
	return 0;
}

struct render_t * fb_create(struct fb_t * fb)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5p6818_fb_data_t * dat = (struct s5p6818_fb_data_t *)res->data;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = dat->width * dat->height * dat->bytes_per_pixel;
	pixels = dma_zalloc(pixlen);
	if(!pixels)
		return NULL;

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = dat->width;
	render->height = dat->height;
	render->pitch = (dat->width * dat->bytes_per_pixel + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	render->clear = sw_render_clear;
	render->snapshot = sw_render_snapshot;
	render->alloc_texture = sw_render_alloc_texture;
	render->alloc_texture_similar = sw_render_alloc_texture_similar;
	render->free_texture = sw_render_free_texture;
	render->fill_texture = sw_render_fill_texture;
	render->blit_texture = sw_render_blit_texture;
	sw_render_create_data(render);

	return render;
}

void fb_destroy(struct fb_t * fb, struct render_t * render)
{
	if(render)
	{
		sw_render_destroy_data(render);
		dma_free(render->pixels);
		free(render);
	}
}

void fb_present(struct fb_t * fb, struct render_t * render)
{
	struct resource_t * res = (struct resource_t *)fb->priv;
	struct s5p6818_fb_data_t * dat = (struct s5p6818_fb_data_t *)res->data;
	void * pixels = render->pixels;

	if(pixels)
	{
		s5p6818_mlc_wait_vsync(dat, 0);
		s5p6818_mlc_set_layer_address(dat, 0, pixels);
		s5p6818_mlc_set_dirty_flag(dat, 0);
	}
}

static void fb_suspend(struct fb_t * fb)
{
}

static void fb_resume(struct fb_t * fb)
{
}

static bool_t s5p6818_register_framebuffer(struct resource_t * res)
{
	struct s5p6818_fb_data_t * dat = (struct s5p6818_fb_data_t *)res->data;
	struct fb_t * fb;
	char name[64];

	fb = malloc(sizeof(struct fb_t));
	if(!fb)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb->name = strdup(name);
	fb->width = dat->width;
	fb->height = dat->height;
	fb->xdpi = dat->xdpi;
	fb->ydpi = dat->ydpi;
	fb->bpp = dat->bits_per_pixel;
	fb->init = fb_init,
	fb->exit = fb_exit,
	fb->setbl = fb_setbl,
	fb->getbl = fb_getbl,
	fb->create = fb_create,
	fb->destroy = fb_destroy,
	fb->present = fb_present,
	fb->suspend = fb_suspend,
	fb->resume = fb_resume,
	fb->priv = res;

	if(register_framebuffer(fb))
		return TRUE;

	free(fb->name);
	free(fb);
	return FALSE;
}

static bool_t s5p6818_unregister_framebuffer(struct resource_t * res)
{
	struct fb_t * fb;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	fb = search_framebuffer(name);
	if(!fb)
		return FALSE;

	if(!unregister_framebuffer(fb))
		return FALSE;

	free(fb->name);
	free(fb);
	return TRUE;
}

static __init void s5p6818_fb_init(void)
{
	resource_for_each("s5p6818-fb", s5p6818_register_framebuffer);
}

static __exit void s5p6818_fb_exit(void)
{
	resource_for_each("s5p6818-fb", s5p6818_unregister_framebuffer);
}

device_initcall(s5p6818_fb_init);
device_exitcall(s5p6818_fb_exit);
