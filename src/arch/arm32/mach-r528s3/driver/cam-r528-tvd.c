/*
 * driver/cam-r528-tvd.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <clk/clk.h>
#include <reset/reset.h>
#include <interrupt/interrupt.h>
#include <dma/dma.h>
#include <camera/camera.h>

#define R528_TVD_TOP_BASE		(0x05c00000)

#define TVD_TOP_DIG_CTL(x)		(0x24 + 0x20 * (x))
#define TVD_TOP_ADC_CTL(x)		(0x28 + 0x20 * (x))
#define TVD_TOP_ADC_CFG(x)		(0x2c + 0x20 * (x))

enum {
	TVD_TOP_MAP		= 0x0000,
	TVD_TOP_3D_CTL1	= 0x0008,
	TVD_TOP_3D_CTL2	= 0x000c,
	TVD_TOP_3D_CTL3	= 0x0010,
	TVD_TOP_3D_CTL4	= 0x0014,
	TVD_TOP_3D_CTL5	= 0x0018,
};

enum {
	TVD_EN			= 0x0000,
	TVD_MODE		= 0x0004,
	TVD_CLAMP_AGC1	= 0x0008,
	TVD_CLAMP_AGC2	= 0x000c,
	TVD_HLOCK1		= 0x0010,
	TVD_HLOCK2		= 0x0014,
	TVD_HLOCK3		= 0x0018,
	TVD_HLOCK4		= 0x001C,
	TVD_HLOCK5		= 0x0020,
	TVD_VLOCK1		= 0x0024,
	TVD_VLOCK2		= 0x0028,
	TVD_CLOCK1		= 0x0030,
	TVD_CLOCK2		= 0x0034,
	TVD_YC_SEP1		= 0x0040,
	TVD_YC_SEP2		= 0x0044,
	TVD_ENHANCE1	= 0x0050,
	TVD_ENHANCE2	= 0x0054,
	TVD_ENHANCE3	= 0x0058,
	TVD_WB1			= 0x0060,
	TVD_WB2			= 0x0064,
	TVD_WB3			= 0x0068,
	TVD_WB4			= 0x006c,
	TVD_IRQ_CTL		= 0x0080,
	TVD_IRQ_STATUS	= 0x0090,
	TVD_DEBUG1		= 0x0100,
	TVD_DEBUG2		= 0x0104,
	TVD_DEBUG3		= 0x0108,
	TVD_DEBUG4		= 0x010c,
	TVD_DEBUG5		= 0x0110,
	TVD_DEBUG6		= 0x0114,
	TVD_DEBUG7		= 0x0118,
	TVD_DEBUG8		= 0x011c,
	TVD_DEBUG9		= 0x0120,
	TVD_DEBUG10		= 0x0124,
	TVD_DEBUG11		= 0x0128,
	TVD_DEBUG12		= 0x012c,
	TVD_DEBUG13		= 0x0130,
	TVD_DEBUG14		= 0x0134,
	TVD_DEBUG15		= 0x0138,
	TVD_STATUS1		= 0x0180,
	TVD_STATUS2		= 0x0184,
	TVD_STATUS3		= 0x0188,
	TVD_STATUS4		= 0x018c,
	TVD_STATUS5		= 0x0190,
	TVD_STATUS6		= 0x0194,
};

enum tvd_source_t {
	TVD_SOURCE_NONE	= 0,
	TVD_SOURCE_NTSC	= 1,
	TVD_SOURCE_PAL	= 2,
};

enum tvd_foramt_t {
	TVD_PL_YUV422	= 0,
	TVD_PL_YUV420	= 1,
	TVD_MB_YUV420	= 2,
};

struct cam_r528_tvd_pdata_t {
	virtual_addr_t virt_tvd_top;
	virtual_addr_t virt_tvd;
	struct clocks_t * clks;
	struct resets_t * rsts;
	int irq;
	int channel;

	unsigned char * yc;
	enum video_format_t fmt;
	int width;
	int height;
	int buflen;
	int ready;
};

static inline void r528_tvd_top_adc_config(struct cam_r528_tvd_pdata_t * pdat, int adc, int en)
{
	u32_t val;

	write32(pdat->virt_tvd_top + TVD_TOP_ADC_CFG(adc), 0x1007aaaa);

	val = read32(pdat->virt_tvd_top + TVD_TOP_DIG_CTL(adc));
	val &= ~(1 << 24);
	if(en)
		val |= 1 << 4;
	else
		val &= ~(1 << 4);
	write32(pdat->virt_tvd_top + TVD_TOP_DIG_CTL(adc), val);

	val = read32(pdat->virt_tvd_top + TVD_TOP_ADC_CTL(adc));
	if(en)
		val |= 0x3 << 0;
	else
		val &= ~(0x3 << 0);
	write32(pdat->virt_tvd_top + TVD_TOP_ADC_CTL(adc), val);
}

static inline void r528_tvd_top_select_channel(struct cam_r528_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd_top + TVD_TOP_MAP);
	if(pdat->channel == 0)
		val &= ~(1 << 4);
	else
		val |= (1 << 4);
	val &= ~(0x3 << 0);
	val |= 0x1 << 0;
	write32(pdat->virt_tvd_top + TVD_TOP_MAP, val);
}

static inline int r528_tvd_get_source(struct cam_r528_tvd_pdata_t * pdat, int timeout)
{
	u32_t val;

	ktime_t t = ktime_add_ms(ktime_get(), timeout);
	do {
		val = read32(pdat->virt_tvd + TVD_STATUS4);
		if((val & (0x7 << 0)) == 0x6)
		{
			if((val & (1 << 18)) && (val & (1 << 16)))
				return TVD_SOURCE_PAL;
			else
				return TVD_SOURCE_NTSC;
		}
		msleep(1);
	} while(ktime_before(ktime_get(), t));

	return TVD_SOURCE_NONE;
}

static inline void r528_tvd_set_saturation(struct cam_r528_tvd_pdata_t * pdat, int saturation)
{
	u32_t val;
	int v;

	v = map(saturation, -1000, 1000, 0x0, 0xff);
	val = read32(pdat->virt_tvd + TVD_ENHANCE2);
	val &= ~(0xff << 0);
	val |= (v & 0xff) << 0;
	write32(pdat->virt_tvd + TVD_ENHANCE2, val);
}

static inline int r528_tvd_get_saturation(struct cam_r528_tvd_pdata_t * pdat)
{
	int v = (read32(pdat->virt_tvd + TVD_ENHANCE2) >> 0) & 0xff;
	return map(v, 0x0, 0xff, -1000, 1000);
}

static inline void r528_tvd_set_brightness(struct cam_r528_tvd_pdata_t * pdat, int brightness)
{
	u32_t val;
	int v;

	if(brightness < 0)
		v = map(brightness, -1000, 0, 0x0, 0x20);
	else
		v = map(brightness, 0, 1000, 0x20, 0xff);
	val = read32(pdat->virt_tvd + TVD_ENHANCE1);
	val &= ~(0xff << 16);
	val |= (v & 0xff) << 16;
	write32(pdat->virt_tvd + TVD_ENHANCE1, val);
}

static inline int r528_tvd_get_brightness(struct cam_r528_tvd_pdata_t * pdat)
{
	int v = (read32(pdat->virt_tvd + TVD_ENHANCE1) >> 16) & 0xff;
	if(v < 0x20)
		return map(v, 0x0, 0x20, -1000, 0);
	else
		return map(v, 0x20, 0xff, 0, 1000);
}

static inline void r528_tvd_set_contrast(struct cam_r528_tvd_pdata_t * pdat, int contrast)
{
	u32_t val;
	int v;

	v = map(contrast, -1000, 1000, 0x0, 0xff);
	val = read32(pdat->virt_tvd + TVD_ENHANCE1);
	val &= ~(0xff << 8);
	val |= (v & 0xff) << 8;
	write32(pdat->virt_tvd + TVD_ENHANCE1, val);
}

static inline int r528_tvd_get_contrast(struct cam_r528_tvd_pdata_t * pdat)
{
	int v = (read32(pdat->virt_tvd + TVD_ENHANCE1) >> 8) & 0xff;
	return map(v, 0x0, 0xff, -1000, 1000);
}

static inline void r528_tvd_set_hue(struct cam_r528_tvd_pdata_t * pdat, int hue)
{
	u32_t val;
	int v;

	v = map(hue, -1000, 1000, -128, 127);
	val = read32(pdat->virt_tvd + TVD_DEBUG2);
	val &= ~(0xff << 20);
	val |= ((char)(v & 0xff)) << 20;
	write32(pdat->virt_tvd + TVD_DEBUG2, val);
}

static inline int r528_tvd_get_hue(struct cam_r528_tvd_pdata_t * pdat)
{
	int v = (read32(pdat->virt_tvd + TVD_DEBUG2) >> 20) & 0xff;
	return map((char)(v & 0xff), -128, 127, -1000, 1000);
}

static inline void r528_tvd_set_sharpness(struct cam_r528_tvd_pdata_t * pdat, int sharpness)
{
	u32_t val;
	int v;

	if(sharpness < 0)
		v = 0x0;
	else
		v = map(sharpness, 0, 1000, 0x0, 0x7);
	val = read32(pdat->virt_tvd + TVD_ENHANCE1);
	val &= ~(0x3f << 24);
	if(v <= 0)
		val |= (0x0 << 28) | (1 << 24);
	else
	{
		val |= (v & 0x7) << 25;
		val |= (0x1 << 28) | (1 << 24);
	}
	write32(pdat->virt_tvd + TVD_ENHANCE1, val);
}

static inline int r528_tvd_get_sharpness(struct cam_r528_tvd_pdata_t * pdat)
{
	int v = (read32(pdat->virt_tvd + TVD_ENHANCE1) >> 25) & 0x7;
	return map(v, 0x0, 0x7, 0, 1000);
}

static inline void r528_tvd_enable(struct cam_r528_tvd_pdata_t * pdat, int en)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_EN);
	if(en)
		val |= 1 << 0;
	else
		val &= ~(1 << 0);
	write32(pdat->virt_tvd + TVD_EN, val);
}

static inline void r528_tvd_set_blue(struct cam_r528_tvd_pdata_t * pdat, int flag)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_MODE);
	val &= ~(0x1 << 8);
	val &= ~(0x3 << 4);
	val |= ((flag & 0x3) << 4);
	write32(pdat->virt_tvd + TVD_MODE, val);
}

static inline void r528_tvd_set_wb_fmt(struct cam_r528_tvd_pdata_t * pdat, enum tvd_foramt_t fmt)
{
	u32_t val = read32(pdat->virt_tvd + TVD_WB1);

	switch(fmt)
	{
	case TVD_PL_YUV422:
		val &= ~(1 << 28);
		val &= ~(1 << 4);
		val |= 1 << 1;
		break;
	case TVD_PL_YUV420:
		val |= 1 << 28;
		val &= ~(1 << 4);
		val &= ~(1 << 1);
		break;
	case TVD_MB_YUV420:
		val |= 1 << 28;
		val |= 1 << 4;
		val &= ~(1 << 1);
		break;
	default:
		break;
	}
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void r528_tvd_set_wb_uv_swap(struct cam_r528_tvd_pdata_t * pdat, int swap)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	if(swap)
		val |= (1 << 31);
	else
		val &= ~(1 << 31);
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void r528_tvd_set_wb_field(struct cam_r528_tvd_pdata_t * pdat, int field)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	if(field)
	{
		val &= ~(1 << 5);
		val |= (1 << 3);
	}
	else
	{
		val |= (1 << 5);
		val &= ~(1 << 3);
	}
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void r528_tvd_set_wb_width_jump(struct cam_r528_tvd_pdata_t * pdat, int jump)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	val &= ~(0xfff << 16);
	val |= (jump & 0xfff) << 16;
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void r528_tvd_set_wb_width(struct cam_r528_tvd_pdata_t * pdat, int width)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB2);
	val &= ~(0xfff << 0);
	val |= (width & 0xfff) << 0;
	write32(pdat->virt_tvd + TVD_WB2, val);
}

static inline void r528_tvd_set_wb_height(struct cam_r528_tvd_pdata_t * pdat, int height)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB2);
	val &= ~(0x7ff << 16);
	val |= (height & 0x7ff) << 16;
	write32(pdat->virt_tvd + TVD_WB2, val);
}

static inline void r528_tvd_set_wb_addr(struct cam_r528_tvd_pdata_t * pdat, void * y, void * c)
{
	u32_t val;

	write32(pdat->virt_tvd + TVD_WB3, (u32_t)((unsigned long)y));
	write32(pdat->virt_tvd + TVD_WB4, (u32_t)((unsigned long)c));
	val = read32(pdat->virt_tvd + TVD_WB1);
	val |= 1 << 8;
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void r528_tvd_irq_enable(struct cam_r528_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_IRQ_CTL);
	val |= 1 << 24;
	write32(pdat->virt_tvd + TVD_IRQ_CTL, val);
}

static inline void r528_tvd_irq_disable(struct cam_r528_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_IRQ_CTL);
	val &= ~(1 << 24);
	write32(pdat->virt_tvd + TVD_IRQ_CTL, val);
}

static inline void r528_tvd_irq_clear(struct cam_r528_tvd_pdata_t * pdat)
{
	write32(pdat->virt_tvd + TVD_IRQ_STATUS, 1 << 24);
}

static inline void r528_tvd_irq_clear_all(struct cam_r528_tvd_pdata_t * pdat)
{
	write32(pdat->virt_tvd + TVD_IRQ_STATUS, read32(pdat->virt_tvd + TVD_IRQ_STATUS));
}

static inline void r528_tvd_capture_on(struct cam_r528_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	val |= 1 << 0;
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void r528_tvd_capture_off(struct cam_r528_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	val &= ~(1 << 0);
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void r528_tvd_config(struct cam_r528_tvd_pdata_t * pdat, enum tvd_source_t s, enum tvd_foramt_t fmt)
{
	u32_t val;

	if(s == TVD_SOURCE_NTSC)
	{
		r528_tvd_enable(pdat, 0);
		val = (160 << 24) | (1 << 16) | (221 << 8) | (1 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC1, val);
		val = (31 << 1) | (1666 << 16) | (100 << 8) | (64 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC2, val);
		val = (0 << 28) | (1 << 26) | (0 << 24) | (70 << 16) | (50 << 8) | (0 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLOCK1, val);
		val = 0x21f07c1f;
		write32(pdat->virt_tvd + TVD_CLOCK2, val);
		write32(pdat->virt_tvd + TVD_HLOCK1, 0x20000000);
		write32(pdat->virt_tvd + TVD_HLOCK2, 0x78d60000);
		write32(pdat->virt_tvd + TVD_HLOCK3, 0x0fe9502d);
		write32(pdat->virt_tvd + TVD_HLOCK4, 0x3e3e8000);
		write32(pdat->virt_tvd + TVD_HLOCK5, 0x42225082);
		write32(pdat->virt_tvd + TVD_VLOCK1, 0x00610220);
		write32(pdat->virt_tvd + TVD_VLOCK2, 0x000e0070);
		write32(pdat->virt_tvd + TVD_YC_SEP1, 0x05004209);
		write32(pdat->virt_tvd + TVD_YC_SEP2, 0x0b1543fa);
		write32(pdat->virt_tvd + TVD_ENHANCE1, 0x14208000);
		write32(pdat->virt_tvd + TVD_ENHANCE2, 0x00000680);
		write32(pdat->virt_tvd + TVD_ENHANCE3, 0x00000000);
		r528_tvd_enable(pdat, 1);
		if(fmt == TVD_MB_YUV420)
		{
			r528_tvd_set_wb_width(pdat, 704);
			r528_tvd_set_wb_width_jump(pdat, 704);
			r528_tvd_set_wb_height(pdat, 224);
		}
		else
		{
			r528_tvd_set_wb_width(pdat, 720);
			r528_tvd_set_wb_width_jump(pdat, 720);
			r528_tvd_set_wb_height(pdat, 240);
		}
	}
	else if(s == TVD_SOURCE_PAL)
	{
		r528_tvd_enable(pdat, 0);
		val = (144 << 24) | (1 << 16) | (220 << 8) | (1 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC1, val);
		val = (31 << 1) | (1666 << 16) | (100 << 8) | (64 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC2, val);
		val = (0 << 28) | (1 << 26) | (0 << 24) | (70 << 16) | (50 << 8) | (1 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLOCK1, val);
		val = 0x2a098acb;
		write32(pdat->virt_tvd + TVD_CLOCK2, val);
		write32(pdat->virt_tvd + TVD_HLOCK1, 0x20000000);
		write32(pdat->virt_tvd + TVD_HLOCK2, 0x78d60001);
		write32(pdat->virt_tvd + TVD_HLOCK3, 0x0fe9502d);
		write32(pdat->virt_tvd + TVD_HLOCK4, 0x3e3e8000);
		write32(pdat->virt_tvd + TVD_HLOCK5, 0x42225089);
		write32(pdat->virt_tvd + TVD_VLOCK1, 0x00c102a1);
		write32(pdat->virt_tvd + TVD_VLOCK2, 0x000e0070);
		write32(pdat->virt_tvd + TVD_YC_SEP1, 0x0100426c);
		write32(pdat->virt_tvd + TVD_YC_SEP2, 0x0b1441fa);
		write32(pdat->virt_tvd + TVD_ENHANCE1, 0x14208000);
		write32(pdat->virt_tvd + TVD_ENHANCE2, 0x00000680);
		write32(pdat->virt_tvd + TVD_ENHANCE3, 0x00000000);
		r528_tvd_enable(pdat, 1);
		if(fmt == TVD_MB_YUV420)
		{
			r528_tvd_set_wb_width(pdat, 704);
			r528_tvd_set_wb_width_jump(pdat, 704);
			r528_tvd_set_wb_height(pdat, 288);
		}
		else
		{
			r528_tvd_set_wb_width(pdat, 720);
			r528_tvd_set_wb_width_jump(pdat, 720);
			r528_tvd_set_wb_height(pdat, 288);
		}
	}
}

static inline void r528_tvd_init(struct cam_r528_tvd_pdata_t * pdat)
{
	enum tvd_source_t s;

	r528_tvd_top_adc_config(pdat, 0, 1);
	r528_tvd_top_select_channel(pdat);
	r528_tvd_enable(pdat, 1);
	s = r528_tvd_get_source(pdat, 100);
	if(s == TVD_SOURCE_NTSC)
	{
		pdat->fmt = VIDEO_FORMAT_NV12;
		pdat->width = 720;
		pdat->height = 480;
		pdat->buflen = pdat->width * pdat->height * 2;
		r528_tvd_config(pdat, s, TVD_PL_YUV420);
		r528_tvd_set_wb_fmt(pdat, TVD_PL_YUV420);
		r528_tvd_set_wb_uv_swap(pdat, 0);
		r528_tvd_set_wb_field(pdat, 0);
		r528_tvd_set_wb_addr(pdat, (void *)virt_to_phys((virtual_addr_t)&pdat->yc[0]), (void *)virt_to_phys((virtual_addr_t)&pdat->yc[pdat->width * pdat->height]));
	}
	else if(s == TVD_SOURCE_PAL)
	{
		pdat->fmt = VIDEO_FORMAT_NV12;
		pdat->width = 720;
		pdat->height = 576;
		pdat->buflen = pdat->width * pdat->height * 2;
		r528_tvd_config(pdat, s, TVD_PL_YUV420);
		r528_tvd_set_wb_fmt(pdat, TVD_PL_YUV420);
		r528_tvd_set_wb_uv_swap(pdat, 0);
		r528_tvd_set_wb_field(pdat, 0);
		r528_tvd_set_wb_addr(pdat, (void *)virt_to_phys((virtual_addr_t)&pdat->yc[0]), (void *)virt_to_phys((virtual_addr_t)&pdat->yc[pdat->width * pdat->height]));
	}
	r528_tvd_set_blue(pdat, 2);
	r528_tvd_capture_off(pdat);
	r528_tvd_irq_clear_all(pdat);
	r528_tvd_irq_disable(pdat);
}

static int cam_start(struct camera_t * cam, enum video_format_t fmt, int width, int height)
{
	struct cam_r528_tvd_pdata_t * pdat = (struct cam_r528_tvd_pdata_t *)cam->priv;

	r528_tvd_init(pdat);
	r528_tvd_irq_enable(pdat);
	r528_tvd_irq_clear_all(pdat);
	r528_tvd_capture_on(pdat);
	return 1;
}

static int cam_stop(struct camera_t * cam)
{
	struct cam_r528_tvd_pdata_t * pdat = (struct cam_r528_tvd_pdata_t *)cam->priv;

	r528_tvd_capture_off(pdat);
	r528_tvd_irq_clear_all(pdat);
	r528_tvd_irq_disable(pdat);
	return 1;
}

static int cam_capture(struct camera_t * cam, struct video_frame_t * frame)
{
	struct cam_r528_tvd_pdata_t * pdat = (struct cam_r528_tvd_pdata_t *)cam->priv;

	if(pdat->ready)
	{
		frame->fmt = pdat->fmt;
		frame->width = pdat->width;
		frame->height = pdat->height;
		frame->buflen = pdat->buflen;
		frame->buf = pdat->yc;
		pdat->ready = 0;
		return 1;
	}
	return 0;
}

static int cam_ioctl(struct camera_t * cam, const char * cmd, void * arg)
{
	struct cam_r528_tvd_pdata_t * pdat = (struct cam_r528_tvd_pdata_t *)cam->priv;
	int * p = arg;

	switch(shash(cmd))
	{
	case 0x440a6553: /* "camera-set-gain" */
		break;
	case 0x30f46ac7: /* "camera-get-gain" */
		break;
	case 0x8531e7af: /* "camera-set-exposure" */
		break;
	case 0x39c98723: /* "camera-get-exposure" */
		break;
	case 0x2d64af48: /* "camera-set-white-balance" */
		break;
	case 0x4b2e173c: /* "camera-get-white-balance" */
		break;
	case 0xc54c8c54: /* "camera-set-focus" */
		break;
	case 0x4f774048: /* "camera-get-focus" */
		break;
	case 0x7ecdcd0f: /* "camera-set-mirror" */
		break;
	case 0x4e4eff83: /* "camera-get-mirror" */
		break;
	case 0xd5d73dfe: /* "camera-set-saturation" */
		if(p)
		{
			r528_tvd_set_saturation(pdat, *p);
			return 0;
		}
		break;
	case 0x0ed48a72: /* "camera-get-saturation" */
		if(p)
		{
			*p = r528_tvd_get_saturation(pdat);
			return 0;
		}
		break;
	case 0xdae4842d: /* "camera-set-brightness" */
		if(p)
		{
			r528_tvd_set_brightness(pdat, *p);
			return 0;
		}
		break;
	case 0x13e1d0a1: /* "camera-get-brightness" */
		if(p)
		{
			*p = r528_tvd_get_brightness(pdat);
			return 0;
		}
		break;
	case 0xf3916322: /* "camera-set-contrast" */
		if(p)
		{
			r528_tvd_set_contrast(pdat, *p);
			return 0;
		}
		break;
	case 0xa8290296: /* "camera-get-contrast" */
		if(p)
		{
			*p = r528_tvd_get_contrast(pdat);
			return 0;
		}
		break;
	case 0x7e2ee316: /* "camera-set-hue" */
		if(p)
		{
			r528_tvd_set_hue(pdat, *p);
			return 0;
		}
		break;
	case 0xe2740a0a: /* "camera-get-hue" */
		if(p)
		{
			*p = r528_tvd_get_hue(pdat);
			return 0;
		}
		break;
	case 0x4a3b52eb: /* "camera-set-sharpness" */
		if(p)
		{
			r528_tvd_set_sharpness(pdat, *p);
			return 0;
		}
		break;
	case 0x91c6e0df: /* "camera-get-sharpness" */
		if(p)
		{
			*p = r528_tvd_get_sharpness(pdat);
			return 0;
		}
		break;
	default:
		break;
	}
	return -1;
}

static void r528_tvd_interrupt(void * data)
{
	struct camera_t * cam = (struct camera_t *)data;
	struct cam_r528_tvd_pdata_t * pdat = (struct cam_r528_tvd_pdata_t *)cam->priv;
	pdat->ready = 1;
	r528_tvd_irq_clear(pdat);
}

static struct device_t * cam_r528_tvd_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cam_r528_tvd_pdata_t * pdat;
	struct camera_t * cam;
	struct device_t * dev;
	int irq = dt_read_int(n, "interrupt", -1);

	if(!irq_is_valid(irq))
		return NULL;

	pdat = malloc(sizeof(struct cam_r528_tvd_pdata_t));
	if(!pdat)
		return NULL;

	cam = malloc(sizeof(struct camera_t));
	if(!cam)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt_tvd_top = phys_to_virt(R528_TVD_TOP_BASE);
	pdat->virt_tvd = phys_to_virt(dt_read_address(n));
	pdat->clks = clocks_alloc(n, "clocks");
	pdat->rsts = resets_alloc(n, "resets");
	pdat->irq = irq;
	pdat->channel = clamp(dt_read_int(n, "channel", 0), 0, 1);
	pdat->yc = dma_alloc_noncoherent(720 * 576 * 2);
	pdat->ready = 0;

	cam->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	cam->start = cam_start;
	cam->stop = cam_stop;
	cam->capture = cam_capture;
	cam->ioctl = cam_ioctl;
	cam->priv = pdat;

	clocks_enable(pdat->clks);
	resets_reset(pdat->rsts, 1);
	request_irq(pdat->irq, r528_tvd_interrupt, IRQ_TYPE_NONE, cam);

	if(!(dev = register_camera(cam, drv)))
	{
		clocks_disable(pdat->clks);
		clocks_free(pdat->clks);
		resets_free(pdat->rsts);
		dma_free_noncoherent(pdat->yc);
		free_device_name(cam->name);
		free(cam->priv);
		free(cam);
		return NULL;
	}
	return dev;
}

static void cam_r528_tvd_remove(struct device_t * dev)
{
	struct camera_t * cam = (struct camera_t *)dev->priv;
	struct cam_r528_tvd_pdata_t * pdat = (struct cam_r528_tvd_pdata_t *)cam->priv;

	if(cam)
	{
		unregister_camera(cam);
		resets_free(pdat->rsts);
		clocks_disable(pdat->clks);
		clocks_free(pdat->clks);
		dma_free_noncoherent(pdat->yc);
		free_device_name(cam->name);
		free(cam->priv);
		free(cam);
	}
}

static void cam_r528_tvd_suspend(struct device_t * dev)
{
}

static void cam_r528_tvd_resume(struct device_t * dev)
{
}

static struct driver_t cam_r528_tvd = {
	.name		= "cam-r528-tvd",
	.probe		= cam_r528_tvd_probe,
	.remove		= cam_r528_tvd_remove,
	.suspend	= cam_r528_tvd_suspend,
	.resume		= cam_r528_tvd_resume,
};

static __init void cam_r528_tvd_driver_init(void)
{
	register_driver(&cam_r528_tvd);
}

static __exit void cam_r528_tvd_driver_exit(void)
{
	unregister_driver(&cam_r528_tvd);
}

driver_initcall(cam_r528_tvd_driver_init);
driver_exitcall(cam_r528_tvd_driver_exit);
