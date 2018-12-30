/*
 * driver/fb-s5p4418.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <dma/dma.h>
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <led/led.h>
#include <framebuffer/framebuffer.h>
#include <s5p4418-rstcon.h>
#include <s5p4418/reg-mlc.h>
#include <s5p4418/reg-dpc.h>
#include <s5p4418/reg-dpa.h>

enum s5p4418_mlc_rgbfmt_t
{
	S5P4418_MLC_RGBFMT_R5G6B5		= 0x4432,
	S5P4418_MLC_RGBFMT_B5G6R5		= 0xC432,

	S5P4418_MLC_RGBFMT_X1R5G5B5		= 0x4342,
	S5P4418_MLC_RGBFMT_X1B5G5R5		= 0xC342,
	S5P4418_MLC_RGBFMT_X4R4G4B4		= 0x4211,
	S5P4418_MLC_RGBFMT_X4B4G4R4		= 0xC211,
	S5P4418_MLC_RGBFMT_X8R3G3B2		= 0x4120,
	S5P4418_MLC_RGBFMT_X8B3G3R2		= 0xC120,

	S5P4418_MLC_RGBFMT_A1R5G5B5		= 0x3342,
	S5P4418_MLC_RGBFMT_A1B5G5R5		= 0xB342,
	S5P4418_MLC_RGBFMT_A4R4G4B4		= 0x2211,
	S5P4418_MLC_RGBFMT_A4B4G4R4		= 0xA211,
	S5P4418_MLC_RGBFMT_A8R3G3B2		= 0x1120,
	S5P4418_MLC_RGBFMT_A8B3G3R2		= 0x9120,

	S5P4418_MLC_RGBFMT_R8G8B8		= 0x4653,
	S5P4418_MLC_RGBFMT_B8G8R8		= 0xC653,

	S5P4418_MLC_RGBFMT_X8R8G8B8		= 0x4653,
	S5P4418_MLC_RGBFMT_X8B8G8R8		= 0xC653,
	S5P4418_MLC_RGBFMT_A8R8G8B8		= 0x0653,
	S5P4418_MLC_RGBFMT_A8B8G8R8		= 0x8653,
};

enum s5p4418_dpc_format_t
{
	S5P4418_DPC_FORMAT_RGB555		= 0x0,
	S5P4418_DPC_FORMAT_RGB565		= 0x1,
	S5P4418_DPC_FORMAT_RGB666		= 0x2,
	S5P4418_DPC_FORMAT_RGB888		= 0x3,
	S5P4418_DPC_FORMAT_MRGB555A		= 0x4,
	S5P4418_DPC_FORMAT_MRGB555B		= 0x5,
	S5P4418_DPC_FORMAT_MRGB565		= 0x6,
	S5P4418_DPC_FORMAT_MRGB666		= 0x7,
	S5P4418_DPC_FORMAT_MRGB888A		= 0x8,
	S5P4418_DPC_FORMAT_MRGB888B		= 0x9,
	S5P4418_DPC_FORMAT_CCIR656		= 0xa,
	S5P4418_DPC_FORMAT_CCIR601A		= 0xc,
	S5P4418_DPC_FORMAT_CCIR601B		= 0xd,
};

enum s5p4418_dpc_ycorder_t
{
	S5P4418_DPC_YCORDER_CBYCRY		= 0x0,
	S5P4418_DPC_YCORDER_CRYCBY		= 0x1,
	S5P4418_DPC_YCORDER_YCBYCR		= 0x2,
	S5P4418_DPC_YCORDER_YCRYCB		= 0x3,
};

enum s5p4418_dpc_dither_t
{
	S5P4418_DPC_DITHER_BYPASS		= 0x0,
	S5P4418_DPC_DITHER_4BIT			= 0x1,
	S5P4418_DPC_DITHER_5BIT			= 0x2,
	S5P4418_DPC_DITHER_6BIT			= 0x3,
};

struct fb_s5p4418_pdata_t
{
	virtual_addr_t virtmlc;
	virtual_addr_t virtdpc;

	char * clk;
	int width;
	int height;
	int pwidth;
	int pheight;
	int bits_per_pixel;
	int bytes_per_pixel;
	int index;
	void * vram[2];

	struct {
		int rgbmode;
		int scanmode;
		int enbedsync;
		int polfield;
		int swaprb;
		enum s5p4418_dpc_format_t format;
		enum s5p4418_dpc_ycorder_t ycorder;
		enum s5p4418_dpc_dither_t rdither;
		enum s5p4418_dpc_dither_t gdither;
		enum s5p4418_dpc_dither_t bdither;
	} mode;

	struct {
		int pixel_clock_hz;
		int	h_front_porch;
		int	h_back_porch;
		int	h_sync_len;
		int	v_front_porch;
		int	v_back_porch;
		int	v_sync_len;
		int vs_start_offset;
		int vs_end_offset;
		int ev_start_offset;
		int ev_end_offset;
		int d_rgb_pvd;
		int d_hsync_cp1;
		int d_vsync_fram;
		int d_de_cp2;
		int	h_sync_active;
		int	v_sync_active;
	} timing;

	struct led_t * backlight;
	int brightness;
};

static inline void s5p4418_mlc_pclk_bclk_enable(struct fb_s5p4418_pdata_t * pdat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(pdat->virtmlc + MLC_CLKENB);
	cfg &= ~((0x1 << 3) | (0x3 << 0));
	if(enable)
		cfg |= ((0x1 << 3) | (0x3 << 0));
	else
		cfg |= ((0x0 << 3) | (0x0 << 0));
	write32(pdat->virtmlc + MLC_CLKENB, cfg);
}

static inline void s5p4418_mlc_set_enable(struct fb_s5p4418_pdata_t * pdat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(pdat->virtmlc + MLC_CONTROLT);
	cfg &= ~(0x1 << 1 | 0x1 << 3);
	if(enable)
		cfg |= (0x1 << 1);
	write32(pdat->virtmlc + MLC_CONTROLT, cfg);
}

static inline void s5p4418_mlc_set_power_mode(struct fb_s5p4418_pdata_t * pdat, bool_t on)
{
	u32_t cfg;

	cfg = read32(pdat->virtmlc + MLC_CONTROLT);
	cfg &= ~(0x1 << 11 | 0x1 << 3);
	if(on)
		cfg |= (0x1 << 11);
	write32(pdat->virtmlc + MLC_CONTROLT, cfg);
}

static inline void s5p4418_mlc_set_sleep_mode(struct fb_s5p4418_pdata_t * pdat, bool_t on)
{
	u32_t cfg;

	cfg = read32(pdat->virtmlc + MLC_CONTROLT);
	cfg &= ~(0x1 << 10 | 0x1 << 3);
	if(!on)
		cfg |= (0x1 << 10);
	write32(pdat->virtmlc + MLC_CONTROLT, cfg);
}

static inline void s5p4418_mlc_set_top_dirty_flag(struct fb_s5p4418_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtmlc + MLC_CONTROLT);
	cfg |= (0x1 << 3);
	write32(pdat->virtmlc + MLC_CONTROLT, cfg);
}

static inline void s5p4418_mlc_set_layer_priority(struct fb_s5p4418_pdata_t * pdat, int priority)
{
	u32_t cfg;

	cfg = read32(pdat->virtmlc + MLC_CONTROLT);
	cfg &= ~(0x3 << 8 | 0x1 << 3);
	cfg |= ((priority & 0x3) << 8);
	write32(pdat->virtmlc + MLC_CONTROLT, cfg);
}

static inline void s5p4418_mlc_set_field_enable(struct fb_s5p4418_pdata_t * pdat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(pdat->virtmlc + MLC_CONTROLT);
	cfg &= ~(0x1 << 0 | 0x1 << 3);
	if(enable)
		cfg |= (0x1 << 0);
	write32(pdat->virtmlc + MLC_CONTROLT, cfg);
}

static inline void s5p4418_mlc_set_screen_size(struct fb_s5p4418_pdata_t * pdat, u32_t width, u32_t height)
{
	u32_t cfg;

	cfg = (((height - 1) & 0x3ff) << 16) | (((width - 1) & 0x3ff) << 0);
	write32(pdat->virtmlc + MLC_SCREENSIZE, cfg);
}

static inline void s5p4418_mlc_set_background_color(struct fb_s5p4418_pdata_t * pdat, u32_t color)
{
	write32(pdat->virtmlc + MLC_BGCOLOR, color);
}

static inline void s5p4418_mlc_set_layer_enable(struct fb_s5p4418_pdata_t * pdat, int layer, bool_t enable)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(pdat->virtmlc + MLC_CONTROL0);
		cfg &= ~(0x1 << 5 | 0x1 << 4);
		if(enable)
			cfg |= (0x1 << 5);
		write32(pdat->virtmlc + MLC_CONTROL0, cfg);
		break;

	case 1:
		cfg = read32(pdat->virtmlc + MLC_CONTROL1);
		cfg &= ~(0x1 << 5 | 0x1 << 4);
		if(enable)
			cfg |= (0x1 << 5);
		write32(pdat->virtmlc + MLC_CONTROL1, cfg);
		break;

	case 2:
		cfg = read32(pdat->virtmlc + MLC_CONTROL2);
		cfg &= ~(0x1 << 5 | 0x1 << 4);
		if(enable)
			cfg |= (0x1 << 5);
		write32(pdat->virtmlc + MLC_CONTROL2, cfg);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_dirty_flag(struct fb_s5p4418_pdata_t * pdat, int layer)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(pdat->virtmlc + MLC_CONTROL0);
		cfg |= (0x1 << 4);
		write32(pdat->virtmlc + MLC_CONTROL0, cfg);
		break;

	case 1:
		cfg = read32(pdat->virtmlc + MLC_CONTROL1);
		cfg |= (0x1 << 4);
		write32(pdat->virtmlc + MLC_CONTROL1, cfg);
		break;

	case 2:
		cfg = read32(pdat->virtmlc + MLC_CONTROL2);
		cfg |= (0x1 << 4);
		write32(pdat->virtmlc + MLC_CONTROL2, cfg);
		break;

	default:
		break;
	}
}

static inline bool_t s5p4418_mlc_get_dirty_flag(struct fb_s5p4418_pdata_t * pdat, int layer)
{
	switch(layer)
	{
	case 0:
		return (read32(pdat->virtmlc + MLC_CONTROL0) & (0x1 << 4)) ? TRUE : FALSE;
	case 1:
		return (read32(pdat->virtmlc + MLC_CONTROL1) & (0x1 << 4)) ? TRUE : FALSE;
	case 2:
		return (read32(pdat->virtmlc + MLC_CONTROL2) & (0x1 << 4)) ? TRUE : FALSE;
	default:
		break;
	}
	return FALSE;
}

static inline void s5p4418_mlc_wait_vsync(struct fb_s5p4418_pdata_t * pdat, int layer)
{
	while(s5p4418_mlc_get_dirty_flag(pdat, layer));
}

static inline void s5p4418_mlc_set_lock_size(struct fb_s5p4418_pdata_t * pdat, int layer, int size)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(pdat->virtmlc + MLC_CONTROL0);
		cfg &= ~(0x3 << 12 | 0x1 << 4);
		cfg |= (((size >> 8) & 0x3) << 12);
		write32(pdat->virtmlc + MLC_CONTROL0, cfg);
		break;

	case 1:
		cfg = read32(pdat->virtmlc + MLC_CONTROL1);
		cfg &= ~(0x3 << 12 | 0x1 << 4);
		cfg |= (((size >> 8) & 0x3) << 12);
		write32(pdat->virtmlc + MLC_CONTROL1, cfg);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_alpha_blending(struct fb_s5p4418_pdata_t * pdat, int layer, bool_t enable, u32_t alpha)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(pdat->virtmlc + MLC_CONTROL0);
		cfg &= ~(0x1 << 2 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 2);
		write32(pdat->virtmlc + MLC_CONTROL0, cfg);

		cfg = read32(pdat->virtmlc + MLC_TPCOLOR0);
		cfg &= ~(0xff << 24);
		cfg |= ((alpha & 0xff) << 24);
		write32(pdat->virtmlc + MLC_TPCOLOR0, cfg);
		break;

	case 1:
		cfg = read32(pdat->virtmlc + MLC_CONTROL1);
		cfg &= ~(0x1 << 2 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 2);
		write32(pdat->virtmlc + MLC_CONTROL1, cfg);

		cfg = read32(pdat->virtmlc + MLC_TPCOLOR1);
		cfg &= ~(0xff << 24);
		cfg |= ((alpha & 0xff) << 24);
		write32(pdat->virtmlc + MLC_TPCOLOR1, cfg);
		break;

	case 2:
		cfg = read32(pdat->virtmlc + MLC_CONTROL2);
		cfg &= ~(0x1 << 2 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 2);
		write32(pdat->virtmlc + MLC_CONTROL2, cfg);

		cfg = read32(pdat->virtmlc + MLC_TPCOLOR2);
		cfg &= ~(0xff << 24);
		cfg |= ((alpha & 0xff) << 24);
		write32(pdat->virtmlc + MLC_TPCOLOR2, cfg);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_transparency(struct fb_s5p4418_pdata_t * pdat, int layer, bool_t enable, u32_t color)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(pdat->virtmlc + MLC_CONTROL0);
		cfg &= ~(0x1 << 0 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 0);
		write32(pdat->virtmlc + MLC_CONTROL0, cfg);

		cfg = read32(pdat->virtmlc + MLC_TPCOLOR0);
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(pdat->virtmlc + MLC_TPCOLOR0, cfg);
		break;

	case 1:
		cfg = read32(pdat->virtmlc + MLC_CONTROL1);
		cfg &= ~(0x1 << 0 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 0);
		write32(pdat->virtmlc + MLC_CONTROL1, cfg);

		cfg = read32(pdat->virtmlc + MLC_TPCOLOR1);
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(pdat->virtmlc + MLC_TPCOLOR1, cfg);
		break;

	case 2:
		cfg = read32(pdat->virtmlc + MLC_CONTROL2);
		cfg &= ~(0x1 << 0 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 0);
		write32(pdat->virtmlc + MLC_CONTROL2, cfg);

		cfg = read32(pdat->virtmlc + MLC_TPCOLOR2);
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(pdat->virtmlc + MLC_TPCOLOR2, cfg);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_color_inversion(struct fb_s5p4418_pdata_t * pdat, int layer, bool_t enable, u32_t color)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(pdat->virtmlc + MLC_CONTROL0);
		cfg &= ~(0x1 << 1 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 1);
		write32(pdat->virtmlc + MLC_CONTROL0, cfg);

		cfg = read32(pdat->virtmlc + MLC_INVCOLOR0);
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(pdat->virtmlc + MLC_INVCOLOR0, cfg);
		break;

	case 1:
		cfg = read32(pdat->virtmlc + MLC_CONTROL1);
		cfg &= ~(0x1 << 1 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 1);
		write32(pdat->virtmlc + MLC_CONTROL1, cfg);

		cfg = read32(pdat->virtmlc + MLC_INVCOLOR1);
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(pdat->virtmlc + MLC_INVCOLOR1, cfg);
		break;

	case 2:
		cfg = read32(pdat->virtmlc + MLC_CONTROL2);
		cfg &= ~(0x1 << 1 | 0x1 << 4);
		cfg |= ((enable ? 1 : 0) << 1);
		write32(pdat->virtmlc + MLC_CONTROL2, cfg);

		cfg = read32(pdat->virtmlc + MLC_INVCOLOR2);
		cfg &= ~(0xfffff << 0);
		cfg |= ((color & 0xfffff) << 0);
		write32(pdat->virtmlc + MLC_INVCOLOR2, cfg);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_layer_invalid_position(struct fb_s5p4418_pdata_t * pdat, int layer, u32_t region, s32_t sx, s32_t sy, s32_t ex, s32_t ey, bool_t enable)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		if(region == 0)
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDLEFTRIGHT0_0, cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDTOPBOTTOM0_0, cfg);
		}
		else
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDLEFTRIGHT0_1, cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDTOPBOTTOM0_1, cfg);
		}
		break;

	case 1:
		if(region == 0)
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDLEFTRIGHT1_0, cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDTOPBOTTOM1_0, cfg);
		}
		else
		{
			cfg = ((enable ? 1 : 0) << 28) | ((sx & 0x7ff) << 16) | (ex & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDLEFTRIGHT1_1, cfg);
			cfg = ((sy & 0x7ff) << 16) | (ey & 0x7ff);
			write32(pdat->virtmlc + MLC_INVALIDTOPBOTTOM1_1, cfg);
		}
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_rgb_format(struct fb_s5p4418_pdata_t * pdat, int layer, enum s5p4418_mlc_rgbfmt_t fmt)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = read32(pdat->virtmlc + MLC_CONTROL0);
		cfg &= ~(0xffff << 16 | 0x1 << 4);
		cfg |= (fmt << 16);
		write32(pdat->virtmlc + MLC_CONTROL0, cfg);
		break;

	case 1:
		cfg = read32(pdat->virtmlc + MLC_CONTROL1);
		cfg &= ~(0xffff << 16 | 0x1 << 4);
		cfg |= (fmt << 16);
		write32(pdat->virtmlc + MLC_CONTROL1, cfg);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_position(struct fb_s5p4418_pdata_t * pdat, int layer, s32_t sx, s32_t sy, s32_t ex, s32_t ey)
{
	u32_t cfg;

	switch(layer)
	{
	case 0:
		cfg = ((sx & 0xfff) << 16) | (ex & 0xfff);
		write32(pdat->virtmlc + MLC_LEFTRIGHT0, cfg);
		cfg = ((sy & 0xfff) << 16) | (ey & 0xfff);
		write32(pdat->virtmlc + MLC_TOPBOTTOM0, cfg);
		break;

	case 1:
		cfg = ((sx & 0xfff) << 16) | (ex & 0xfff);
		write32(pdat->virtmlc + MLC_LEFTRIGHT1, cfg);
		cfg = ((sy & 0xfff) << 16) | (ey & 0xfff);
		write32(pdat->virtmlc + MLC_TOPBOTTOM1, cfg);
		break;

	case 2:
		cfg = ((sx & 0xfff) << 16) | (ex & 0xfff);
		write32(pdat->virtmlc + MLC_LEFTRIGHT2, cfg);
		cfg = ((sy & 0xfff) << 16) | (ey & 0xfff);
		write32(pdat->virtmlc + MLC_TOPBOTTOM2, cfg);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_layer_stride(struct fb_s5p4418_pdata_t * pdat, int layer, s32_t hstride, s32_t vstride)
{
	switch(layer)
	{
	case 0:
		write32(pdat->virtmlc + MLC_HSTRIDE0, hstride);
		write32(pdat->virtmlc + MLC_VSTRIDE0, vstride);
		break;

	case 1:
		write32(pdat->virtmlc + MLC_HSTRIDE1, hstride);
		write32(pdat->virtmlc + MLC_VSTRIDE1, vstride);
		break;

	default:
		break;
	}
}

static inline void s5p4418_mlc_set_layer_address(struct fb_s5p4418_pdata_t * pdat, int layer, void * vram)
{
	switch(layer)
	{
	case 0:
		write32(pdat->virtmlc + MLC_ADDRESS0, (u32_t)vram);
		break;
	case 1:
		write32(pdat->virtmlc + MLC_ADDRESS1, (u32_t)vram);
		break;
	case 2:
		write32(pdat->virtmlc + MLC_ADDRESS2, (u32_t)vram);
		break;
	default:
		break;
	}
}

static inline void s5p4418_dpc_pclk_enable(struct fb_s5p4418_pdata_t * pdat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(pdat->virtdpc + DPC_CLKENB);
	cfg &= ~((0x1 << 3) | (0x1 << 2));
	if(enable)
		cfg |= ((0x1 << 3) | (0x1 << 2));
	else
		cfg |= ((0x0 << 3) | (0x0 << 2));
	write32(pdat->virtdpc + DPC_CLKENB, cfg);
}

static inline void s5p4418_dpc_set_clock(struct fb_s5p4418_pdata_t * pdat)
{
	u64_t rate;
	u32_t cfg;
	int i;

	rate = clk_get_rate(pdat->clk);

	/* DPC_CLKGEN0L */
	for(i = 0; i < 256; i++)
	{
		if((rate / (i + 1)) <= pdat->timing.pixel_clock_hz)
			break;
	}
	rate = rate / (i + 1);

	cfg = read32(pdat->virtdpc + DPC_CLKGEN0L);
	cfg &= ~((0x7 << 2) | (0xff << 5));
	cfg |= 0x2 << 2;
	cfg |= (i & 0xff) << 5;
	write32(pdat->virtdpc + DPC_CLKGEN0L, cfg);

	/* DPC_CLKGEN1L */
	for(i = 0; i < 256; i++)
	{
		if((rate / (i + 1)) <= pdat->timing.pixel_clock_hz)
			break;
	}
	rate = rate / (i + 1);

	cfg = read32(pdat->virtdpc + DPC_CLKGEN1L);
	cfg &= ~((0x7 << 2) | (0xff << 5));
	cfg |= 0x7 << 2;
	cfg |= (i & 0xff) << 5;
	write32(pdat->virtdpc + DPC_CLKGEN1L, cfg);

	/* OUTCLKDELAY */
	write32(pdat->virtdpc + DPC_CLKGEN0H, 0x00000000);
	write32(pdat->virtdpc + DPC_CLKGEN1H, 0x00000000);
}

static inline void s5p4418_dpc_set_mode(struct fb_s5p4418_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtdpc + DPC_CTRL0);
	cfg &= ~(0x1 << 12);
	cfg |= ((pdat->mode.rgbmode ? 1 : 0) << 12);
	cfg &= ~(0x1 << 9);
	cfg |= ((pdat->mode.scanmode ? 1 : 0) << 9);
	cfg &= ~(0x1 << 8);
	cfg |= ((pdat->mode.enbedsync ? 1 : 0) << 8);
	cfg &= ~(0x1 << 2);
	cfg |= ((pdat->mode.polfield ? 1 : 0) << 2);
	write32(pdat->virtdpc + DPC_CTRL0, cfg);

	cfg = read32(pdat->virtdpc + DPC_CTRL1);
	cfg &= ~(0x1 << 15);
	cfg |= ((pdat->mode.swaprb ? 1 : 0) << 15);
	cfg &= ~(0xf << 8);
	cfg |= (pdat->mode.format << 8);
	cfg &= ~(0x3 << 6);
	cfg |= (pdat->mode.ycorder << 6);
	cfg &= ~(0x3f << 0);
	cfg |= ((pdat->mode.rdither << 0) | (pdat->mode.gdither << 2) | (pdat->mode.bdither << 4));
	write32(pdat->virtdpc + DPC_CTRL1, cfg);

	cfg = 0x0;
	write32(pdat->virtdpc + DPC_CTRL2, cfg);
}

static inline void s5p4418_dpc_set_timing(struct fb_s5p4418_pdata_t * pdat)
{
	u32_t cfg;

	/* horizontal */
	cfg = pdat->width + pdat->timing.h_front_porch + pdat->timing.h_sync_len + pdat->timing.h_back_porch - 1;
	write32(pdat->virtdpc + DPC_HTOTAL, cfg);

	cfg = pdat->timing.h_sync_len - 1;
	write32(pdat->virtdpc + DPC_HSWIDTH, cfg);

	cfg = pdat->timing.h_sync_len + pdat->timing.h_back_porch - 1;
	write32(pdat->virtdpc + DPC_HASTART, cfg);

	cfg = pdat->width + pdat->timing.h_sync_len + pdat->timing.h_back_porch - 1;
	write32(pdat->virtdpc + DPC_HAEND, cfg);

	/* vertical - progressive */
	cfg = pdat->height + pdat->timing.v_front_porch + pdat->timing.v_sync_len + pdat->timing.v_back_porch - 1;
	write32(pdat->virtdpc + DPC_VTOTAL, cfg);

	cfg = pdat->timing.v_sync_len - 1;
	write32(pdat->virtdpc + DPC_VSWIDTH, cfg);

	cfg = pdat->timing.v_sync_len + pdat->timing.v_back_porch - 1;
	write32(pdat->virtdpc + DPC_VASTART, cfg);

	cfg = pdat->height + pdat->timing.v_sync_len + pdat->timing.v_back_porch - 1;
	write32(pdat->virtdpc + DPC_VAEND, cfg);

	/* vertical - interlaced */
	cfg = pdat->height + pdat->timing.v_front_porch + pdat->timing.v_sync_len + pdat->timing.v_back_porch - 1;
	write32(pdat->virtdpc + DPC_EVTOTAL, cfg);

	cfg = pdat->timing.v_sync_len - 1;
	write32(pdat->virtdpc + DPC_EVSWIDTH, cfg);

	cfg = pdat->timing.v_sync_len + pdat->timing.v_back_porch - 1;
	write32(pdat->virtdpc + DPC_EVASTART, cfg);

	cfg = pdat->height + pdat->timing.v_sync_len + pdat->timing.v_back_porch - 1;
	write32(pdat->virtdpc + DPC_EVAEND, cfg);

	/* Sync offset */
	write32(pdat->virtdpc + DPC_VSSOFFSET, pdat->timing.vs_start_offset);
	write32(pdat->virtdpc + DPC_VSEOFFSET, pdat->timing.vs_end_offset);
	write32(pdat->virtdpc + DPC_EVSSOFFSET, pdat->timing.ev_start_offset);
	write32(pdat->virtdpc + DPC_EVSEOFFSET, pdat->timing.ev_end_offset);

	/* Delay value */
	cfg = read32(pdat->virtdpc + DPC_CTRL0);
	cfg &= ~(0x1 << 10);
	cfg &= ~(0xf << 4);
	cfg |= (pdat->timing.d_rgb_pvd & 0xf) << 4;
	write32(pdat->virtdpc + DPC_CTRL0, cfg);

	cfg = ((pdat->timing.d_hsync_cp1 & 0xf) << 0) | ((pdat->timing.d_vsync_fram & 0xf) << 8);
	write32(pdat->virtdpc + DPC_DELAY0, cfg);

	cfg = (pdat->timing.d_de_cp2 & 0x3f) << 0;
	write32(pdat->virtdpc + DPC_DELAY1, cfg);
}

static inline void s5p4418_dpc_set_polarity(struct fb_s5p4418_pdata_t * pdat)
{
	u32_t cfg;

	cfg = read32(pdat->virtdpc + DPC_CTRL0);
	cfg &= ~(0x1 << 10 | 0x1 << 1 | 0x1 << 0);
	if(pdat->timing.h_sync_active)
		cfg |= (0x1 << 0);
	if(pdat->timing.v_sync_active)
		cfg |= (0x1 << 1);
	write32(pdat->virtdpc + DPC_CTRL0, cfg);
}

static inline void s5p4418_dpc_set_enable(struct fb_s5p4418_pdata_t * pdat, bool_t enable)
{
	u32_t cfg;

	cfg = read32(pdat->virtdpc + DPC_CTRL0);
	cfg &= ~(0x1 << 10 | 0x1 << 15);
	if(enable)
		cfg |= (0x1 << 15);
	write32(pdat->virtdpc + DPC_CTRL0, cfg);
}

static inline void s5p4418_dpa_rgb_mux_select(int sel)
{
	write32(phys_to_virt(S5P4418_DPA_BASE + DPA_TFT_MUXCTRL), (sel & 0x3));
}

static inline void s5p4418_fb_cfg_gpios(int base, int nr, int cfg, enum gpio_pull_t pull, enum gpio_drv_t drv)
{
	for(; nr > 0; nr--, base++)
	{
		gpio_set_cfg(base, cfg);
		gpio_set_pull(base, pull);
		gpio_set_drv(base, drv);
	}
}

static void s5p4418_fb_init(struct fb_s5p4418_pdata_t * pdat)
{
	s5p4418_ip_reset(RESET_ID_DISP_TOP, 0);
	s5p4418_ip_reset(RESET_ID_DISPLAY, 0);
	s5p4418_ip_reset(RESET_ID_LCDIF, 0);
	s5p4418_ip_reset(RESET_ID_LVDS, 0);

	/*
	 * Initial digital rgb lcd port
	 */
	s5p4418_fb_cfg_gpios(0, 28, 0x1, GPIO_PULL_NONE, GPIO_DRV_STRONGER);

	/*
	 * Enable some clocks
	 */
	s5p4418_mlc_pclk_bclk_enable(pdat, TRUE);
	s5p4418_dpc_pclk_enable(pdat, TRUE);
	s5p4418_dpc_set_clock(pdat);

	/*
	 * Initial mlc top layer
	 */
	s5p4418_mlc_set_layer_priority(pdat, 0);
	s5p4418_mlc_set_background_color(pdat, 0);
	s5p4418_mlc_set_field_enable(pdat, FALSE);
	s5p4418_mlc_set_power_mode(pdat, TRUE);
	s5p4418_mlc_set_sleep_mode(pdat, FALSE);

	/*
	 * Initial mlc fb layer
	 */
	s5p4418_mlc_set_lock_size(pdat, 0, 16);
	s5p4418_mlc_set_alpha_blending(pdat, 0, FALSE, 0);
	s5p4418_mlc_set_transparency(pdat, 0, FALSE, 0);
	s5p4418_mlc_set_color_inversion(pdat, 0, FALSE, 0);
	s5p4418_mlc_set_layer_invalid_position(pdat, 0, 0, 0, 0, 0, 0, FALSE);
	s5p4418_mlc_set_layer_invalid_position(pdat, 0, 1, 0, 0, 0, 0, FALSE);
	s5p4418_mlc_set_rgb_format(pdat, 0, S5P4418_MLC_RGBFMT_X8R8G8B8);
	s5p4418_mlc_set_position(pdat, 0, 0, 0, pdat->width - 1, pdat->height - 1);
	s5p4418_mlc_set_layer_stride(pdat, 0, pdat->bytes_per_pixel, pdat->bytes_per_pixel * pdat->width);
	s5p4418_mlc_set_screen_size(pdat, pdat->width, pdat->height);
	s5p4418_mlc_set_layer_address(pdat, 0, pdat->vram[0]);

	/*
	 * Enable mlc controller
	 */
	s5p4418_mlc_set_enable(pdat, TRUE);
	s5p4418_mlc_set_top_dirty_flag(pdat);
	s5p4418_mlc_set_layer_enable(pdat, 0, TRUE);
	s5p4418_mlc_set_dirty_flag(pdat, 0);

	/*
	 * Initial dpc
	 */
	s5p4418_dpc_set_mode(pdat);
	s5p4418_dpc_set_timing(pdat);
	s5p4418_dpc_set_polarity(pdat);

	/*
	 * Enable dpc controller
	 */
	s5p4418_dpc_set_enable(pdat, TRUE);

	/*
	 * MUX select for primary dpc(TFT)
	 */
	s5p4418_dpa_rgb_mux_select(0);
}

static void fb_setbl(struct framebuffer_t * fb, int brightness)
{
	struct fb_s5p4418_pdata_t * pdat = (struct fb_s5p4418_pdata_t *)fb->priv;
	led_set_brightness(pdat->backlight, brightness);
}

static int fb_getbl(struct framebuffer_t * fb)
{
	struct fb_s5p4418_pdata_t * pdat = (struct fb_s5p4418_pdata_t *)fb->priv;
	return led_get_brightness(pdat->backlight);
}

static struct render_t * fb_create(struct framebuffer_t * fb)
{
	struct fb_s5p4418_pdata_t * pdat = (struct fb_s5p4418_pdata_t *)fb->priv;
	struct render_t * render;
	void * pixels;
	size_t pixlen;

	pixlen = pdat->width * pdat->height * pdat->bytes_per_pixel;
	pixels = memalign(4, pixlen);
	if(!pixels)
		return NULL;

	render = malloc(sizeof(struct render_t));
	if(!render)
	{
		free(pixels);
		return NULL;
	}

	render->width = pdat->width;
	render->height = pdat->height;
	render->pitch = (pdat->width * pdat->bytes_per_pixel + 0x3) & ~0x3;
	render->format = PIXEL_FORMAT_ARGB32;
	render->pixels = pixels;
	render->pixlen = pixlen;
	render->priv = NULL;

	return render;
}

static void fb_destroy(struct framebuffer_t * fb, struct render_t * render)
{
	if(render)
	{
		free(render->pixels);
		free(render);
	}
}

static void fb_present(struct framebuffer_t * fb, struct render_t * render, struct dirty_rect_t * rect, int nrect)
{
	struct fb_s5p4418_pdata_t * pdat = (struct fb_s5p4418_pdata_t *)fb->priv;

	if(render && render->pixels)
	{
		pdat->index = (pdat->index + 1) & 0x1;
		memcpy(pdat->vram[pdat->index], render->pixels, render->pixlen);
		dma_cache_sync(pdat->vram[pdat->index], render->pixlen, DMA_TO_DEVICE);
		s5p4418_mlc_wait_vsync(pdat, 0);
		s5p4418_mlc_set_layer_address(pdat, 0, pdat->vram[pdat->index]);
		s5p4418_mlc_set_dirty_flag(pdat, 0);
	}
}

static struct device_t * fb_s5p4418_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct fb_s5p4418_pdata_t * pdat;
	struct framebuffer_t * fb;
	struct device_t * dev;
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct fb_s5p4418_pdata_t));
	if(!pdat)
		return NULL;

	fb = malloc(sizeof(struct framebuffer_t));
	if(!fb)
	{
		free(pdat);
		return NULL;
	}

	pdat->virtmlc = phys_to_virt(S5P4418_MLC0_BASE);
	pdat->virtdpc = phys_to_virt(S5P4418_DPC0_BASE);
	pdat->clk = strdup(clk);
	pdat->width = dt_read_int(n, "width", 1024);
	pdat->height = dt_read_int(n, "height", 600);
	pdat->pwidth = dt_read_int(n, "physical-width", 216);
	pdat->pheight = dt_read_int(n, "physical-height", 135);
	pdat->bits_per_pixel = dt_read_int(n, "bits-per-pixel", 32);
	pdat->bytes_per_pixel = dt_read_int(n, "bytes-per-pixel", 4);
	pdat->index = 0;
	pdat->vram[0] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes_per_pixel);
	pdat->vram[1] = dma_alloc_noncoherent(pdat->width * pdat->height * pdat->bytes_per_pixel);

	pdat->mode.rgbmode = 1;
	pdat->mode.scanmode = 0;
	pdat->mode.enbedsync = 0;
	pdat->mode.polfield = 0;
	pdat->mode.swaprb = 0;
	pdat->mode.format = S5P4418_DPC_FORMAT_RGB888;
	pdat->mode.ycorder = S5P4418_DPC_YCORDER_CBYCRY;
	pdat->mode.rdither = S5P4418_DPC_DITHER_BYPASS;
	pdat->mode.gdither = S5P4418_DPC_DITHER_BYPASS;
	pdat->mode.bdither = S5P4418_DPC_DITHER_BYPASS;

	pdat->timing.pixel_clock_hz = dt_read_long(n, "clock-frequency", 52000000);
	pdat->timing.h_front_porch = dt_read_int(n, "hfront-porch", 1);
	pdat->timing.h_back_porch = dt_read_int(n, "hback-porch", 1);
	pdat->timing.h_sync_len = dt_read_int(n, "hsync-len", 1);
	pdat->timing.v_front_porch = dt_read_int(n, "vfront-porch", 1);
	pdat->timing.v_back_porch = dt_read_int(n, "vback-porch", 1);
	pdat->timing.v_sync_len = dt_read_int(n, "vsync-len", 1);
	pdat->timing.vs_start_offset = 1;
	pdat->timing.vs_end_offset = 1;
	pdat->timing.ev_start_offset = 1;
	pdat->timing.ev_end_offset = 1;
	pdat->timing.d_rgb_pvd = 0;
	pdat->timing.d_hsync_cp1 = 7;
	pdat->timing.d_vsync_fram = 7;
	pdat->timing.d_de_cp2 = 7;
	pdat->timing.h_sync_active = dt_read_bool(n, "hsync-active", 0);
	pdat->timing.v_sync_active = dt_read_bool(n, "vsync-active", 0);
	pdat->backlight = search_led(dt_read_string(n, "backlight", NULL));

	fb->name = alloc_device_name(dt_read_name(n), -1);
	fb->width = pdat->width;
	fb->height = pdat->height;
	fb->pwidth = pdat->pwidth;
	fb->pheight = pdat->pheight;
	fb->bpp = pdat->bits_per_pixel;
	fb->setbl = fb_setbl;
	fb->getbl = fb_getbl;
	fb->create = fb_create;
	fb->destroy = fb_destroy;
	fb->present = fb_present;
	fb->priv = pdat;

	clk_enable(pdat->clk);
	s5p4418_fb_init(pdat);

	if(!register_framebuffer(&dev, fb))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void fb_s5p4418_remove(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_s5p4418_pdata_t * pdat = (struct fb_s5p4418_pdata_t *)fb->priv;

	if(fb && unregister_framebuffer(fb))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		dma_free_noncoherent(pdat->vram[0]);
		dma_free_noncoherent(pdat->vram[1]);

		free_device_name(fb->name);
		free(fb->priv);
		free(fb);
	}
}

static void fb_s5p4418_suspend(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_s5p4418_pdata_t * pdat = (struct fb_s5p4418_pdata_t *)fb->priv;

	pdat->brightness = led_get_brightness(pdat->backlight);
	led_set_brightness(pdat->backlight, 0);
}

static void fb_s5p4418_resume(struct device_t * dev)
{
	struct framebuffer_t * fb = (struct framebuffer_t *)dev->priv;
	struct fb_s5p4418_pdata_t * pdat = (struct fb_s5p4418_pdata_t *)fb->priv;

	led_set_brightness(pdat->backlight, pdat->brightness);
}

static struct driver_t fb_s5p4418 = {
	.name		= "fb-s5p4418",
	.probe		= fb_s5p4418_probe,
	.remove		= fb_s5p4418_remove,
	.suspend	= fb_s5p4418_suspend,
	.resume		= fb_s5p4418_resume,
};

static __init void fb_s5p4418_driver_init(void)
{
	register_driver(&fb_s5p4418);
}

static __exit void fb_s5p4418_driver_exit(void)
{
	unregister_driver(&fb_s5p4418);
}

driver_initcall(fb_s5p4418_driver_init);
driver_exitcall(fb_s5p4418_driver_exit);
