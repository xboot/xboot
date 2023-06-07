/*
 * driver/cam-t113-tvd.c
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
#include <camera/camera.h>

#define T113_TVD_TOP_BASE		(0x05c00000)
#define T113_TVD_TOP_RESET		(1040)

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

struct cam_t113_tvd_pdata_t {
	virtual_addr_t virt_tvd_top;
	virtual_addr_t virt_tvd;
	char * clk;
	int reset;
	int channel;

	unsigned char yc[720 * 576 * 2];
	enum video_format_t fmt;
	int width;
	int height;
	int buflen;
};

static inline void t113_tvd_top_adc_config(struct cam_t113_tvd_pdata_t * pdat, int adc, int en)
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

static inline void t113_tvd_top_select_channel(struct cam_t113_tvd_pdata_t * pdat)
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

static inline int t113_tvd_get_source(struct cam_t113_tvd_pdata_t * pdat, int timeout)
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

static inline void t113_tvd_set_saturation(struct cam_t113_tvd_pdata_t * pdat, int saturation)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_ENHANCE2);
	val &= ~(0xff << 0);
	val |= (saturation & 0xff) << 0;
	write32(pdat->virt_tvd + TVD_ENHANCE2, val);
}

static inline int t113_tvd_get_saturation(struct cam_t113_tvd_pdata_t * pdat)
{
	return (read32(pdat->virt_tvd + TVD_ENHANCE2) >> 0) & 0xff;
}

static inline void t113_tvd_set_brightness(struct cam_t113_tvd_pdata_t * pdat, int brightness)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_ENHANCE1);
	val &= ~(0xff << 16);
	val |= (brightness & 0xff) << 16;
	write32(pdat->virt_tvd + TVD_ENHANCE1, val);
}

static inline int t113_tvd_get_brightness(struct cam_t113_tvd_pdata_t * pdat)
{
	return (read32(pdat->virt_tvd + TVD_ENHANCE1) >> 16) & 0xff;
}

static inline void t113_tvd_set_contrast(struct cam_t113_tvd_pdata_t * pdat, int contrast)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_ENHANCE1);
	val &= ~(0xff << 8);
	val |= (contrast & 0xff) << 8;
	write32(pdat->virt_tvd + TVD_ENHANCE1, val);
}

static inline int t113_tvd_get_contrast(struct cam_t113_tvd_pdata_t * pdat)
{
	return (read32(pdat->virt_tvd + TVD_ENHANCE1) >> 8) & 0xff;
}

static inline void t113_tvd_enable(struct cam_t113_tvd_pdata_t * pdat, int en)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_EN);
	if(en)
		val |= 1 << 0;
	else
		val &= ~(1 << 0);
	write32(pdat->virt_tvd + TVD_EN, val);
}

static inline void t113_tvd_set_blue(struct cam_t113_tvd_pdata_t * pdat, int flag)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_MODE);
	val &= ~(0x1 << 8);
	val &= ~(0x3 << 4);
	val |= ((flag & 0x3) << 4);
	write32(pdat->virt_tvd + TVD_MODE, val);
}

static inline void t113_tvd_set_wb_fmt(struct cam_t113_tvd_pdata_t * pdat, enum tvd_foramt_t fmt)
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

static inline void t113_tvd_set_wb_uv_swap(struct cam_t113_tvd_pdata_t * pdat, int swap)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	if(swap)
		val |= (1 << 31);
	else
		val &= ~(1 << 31);
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void t113_tvd_set_wb_width_jump(struct cam_t113_tvd_pdata_t * pdat, int jump)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	val &= ~(0xfff << 16);
	val |= (jump & 0xfff) << 16;
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void t113_tvd_set_wb_width(struct cam_t113_tvd_pdata_t * pdat, int width)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB2);
	val &= ~(0xfff << 0);
	val |= (width & 0xfff) << 0;
	write32(pdat->virt_tvd + TVD_WB2, val);
}

static inline void t113_tvd_set_wb_height(struct cam_t113_tvd_pdata_t * pdat, int height)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB2);
	val &= ~(0x7ff << 16);
	val |= (height & 0x7ff) << 16;
	write32(pdat->virt_tvd + TVD_WB2, val);
}

static inline void t113_tvd_set_wb_addr(struct cam_t113_tvd_pdata_t * pdat, void * y, void * c)
{
	u32_t val;

	write32(pdat->virt_tvd + TVD_WB3, (u32_t)y);
	write32(pdat->virt_tvd + TVD_WB4, (u32_t)c);
	val = read32(pdat->virt_tvd + TVD_WB1);
	val |= 1 << 8;
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void t113_tvd_irq_enable(struct cam_t113_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_IRQ_CTL);
	val |= 1 << 24;
	write32(pdat->virt_tvd + TVD_IRQ_CTL, val);
}

static inline void t113_tvd_irq_disable(struct cam_t113_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_IRQ_CTL);
	val &= ~(1 << 24);
	write32(pdat->virt_tvd + TVD_IRQ_CTL, val);
}

static inline int t113_tvd_irq_status(struct cam_t113_tvd_pdata_t * pdat)
{
	return read32(pdat->virt_tvd + TVD_IRQ_STATUS) & (1 << 24) ? 1 : 0;
}

static inline void t113_tvd_irq_clear(struct cam_t113_tvd_pdata_t * pdat)
{
	write32(pdat->virt_tvd + TVD_IRQ_STATUS, 1 << 24);
}

static inline void t113_tvd_irq_clear_all(struct cam_t113_tvd_pdata_t * pdat)
{
	write32(pdat->virt_tvd + TVD_IRQ_STATUS, read32(pdat->virt_tvd + TVD_IRQ_STATUS));
}

static inline void t113_tvd_capture_on(struct cam_t113_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	val |= 1 << 0;
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void t113_tvd_capture_off(struct cam_t113_tvd_pdata_t * pdat)
{
	u32_t val;

	val = read32(pdat->virt_tvd + TVD_WB1);
	val &= ~(1 << 0);
	write32(pdat->virt_tvd + TVD_WB1, val);
}

static inline void t113_tvd_config(struct cam_t113_tvd_pdata_t * pdat, enum tvd_source_t s, enum tvd_foramt_t fmt)
{
	u32_t val;

	if(s == TVD_SOURCE_NTSC)
	{
		t113_tvd_enable(pdat, 0);
		val = (160 << 24) | (1 << 16) | (221 << 8) | (1 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC1, val);
		val = (31 << 1) | (1666 << 16) | (100 << 8) | (64 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC2, val);
		val = (0 << 28) | (1 << 26) | (0 << 24) | (70 << 16) | (50 << 8) | (0 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLOCK1, val);
		val = 0x21f07c1f;
		write32(pdat->virt_tvd + TVD_CLOCK2, val);
		write32(pdat->virt_tvd + TVD_HLOCK1, 0x20000000);
		write32(pdat->virt_tvd + TVD_HLOCK2, 0x4ed60000);
		write32(pdat->virt_tvd + TVD_HLOCK3, 0x0fe9502d);
		write32(pdat->virt_tvd + TVD_HLOCK4, 0x3e3e8000);
		write32(pdat->virt_tvd + TVD_HLOCK5, 0x4e225082);
		write32(pdat->virt_tvd + TVD_VLOCK1, 0x00610220);
		write32(pdat->virt_tvd + TVD_VLOCK2, 0x000e0070);
		write32(pdat->virt_tvd + TVD_YC_SEP1, 0x00004209);
		write32(pdat->virt_tvd + TVD_YC_SEP2, 0xff6440af);
		write32(pdat->virt_tvd + TVD_ENHANCE1, 0x14208000);
		write32(pdat->virt_tvd + TVD_ENHANCE2, 0x00000680);
		write32(pdat->virt_tvd + TVD_ENHANCE3, 0x00000000);
		t113_tvd_enable(pdat, 1);
		if(fmt == TVD_MB_YUV420)
		{
			t113_tvd_set_wb_width(pdat, 704);
			t113_tvd_set_wb_width_jump(pdat, 704);
			t113_tvd_set_wb_height(pdat, 224);
		}
		else
		{
			t113_tvd_set_wb_width(pdat, 720);
			t113_tvd_set_wb_width_jump(pdat, 720);
			t113_tvd_set_wb_height(pdat, 240);
		}
	}
	else if(s == TVD_SOURCE_PAL)
	{
		t113_tvd_enable(pdat, 0);
		val = (144 << 24) | (1 << 16) | (220 << 8) | (1 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC1, val);
		val = (31 << 1) | (1666 << 16) | (100 << 8) | (64 << 0);
		write32(pdat->virt_tvd + TVD_CLAMP_AGC2, val);
		val = (0 << 28) | (1 << 26) | (0 << 24) | (70 << 16) | (50 << 8) | (1 << 1) | (1 << 0);
		write32(pdat->virt_tvd + TVD_CLOCK1, val);
		val = 0x2a098acb;
		write32(pdat->virt_tvd + TVD_CLOCK2, val);
		write32(pdat->virt_tvd + TVD_HLOCK1, 0x20000000);
		write32(pdat->virt_tvd + TVD_HLOCK2, 0x4ed60000);
		write32(pdat->virt_tvd + TVD_HLOCK3, 0x0fe9502d);
		write32(pdat->virt_tvd + TVD_HLOCK4, 0x3e3e8000);
		write32(pdat->virt_tvd + TVD_HLOCK5, 0x4e225082);
		write32(pdat->virt_tvd + TVD_VLOCK1, 0x00610220);
		write32(pdat->virt_tvd + TVD_VLOCK2, 0x000e0070);
		write32(pdat->virt_tvd + TVD_YC_SEP1, 0x00004209);
		write32(pdat->virt_tvd + TVD_YC_SEP2, 0xff6440af);
		write32(pdat->virt_tvd + TVD_ENHANCE1, 0x14208000);
		write32(pdat->virt_tvd + TVD_ENHANCE2, 0x00000680);
		write32(pdat->virt_tvd + TVD_ENHANCE3, 0x00000000);
		t113_tvd_enable(pdat, 1);
		if(fmt == TVD_MB_YUV420)
		{
			t113_tvd_set_wb_width(pdat, 704);
			t113_tvd_set_wb_width_jump(pdat, 704);
			t113_tvd_set_wb_height(pdat, 288);
		}
		else
		{
			t113_tvd_set_wb_width(pdat, 720);
			t113_tvd_set_wb_width_jump(pdat, 720);
			t113_tvd_set_wb_height(pdat, 288);
		}
	}
}

static inline void t113_tvd_init(struct cam_t113_tvd_pdata_t * pdat)
{
	enum tvd_source_t s;

	t113_tvd_top_adc_config(pdat, 0, 1);
	t113_tvd_top_select_channel(pdat);
	t113_tvd_enable(pdat, 1);
	s = t113_tvd_get_source(pdat, 100);
	if(s == TVD_SOURCE_NTSC)
	{
		pdat->fmt = VIDEO_FORMAT_NV12;
		pdat->width = 720;
		pdat->height = 480;
		pdat->buflen = pdat->width * pdat->height * 2;
		t113_tvd_config(pdat, s, TVD_PL_YUV420);
		t113_tvd_set_wb_fmt(pdat, TVD_PL_YUV420);
		t113_tvd_set_wb_uv_swap(pdat, 0);
		t113_tvd_set_wb_addr(pdat, &pdat->yc[0], &pdat->yc[pdat->width * pdat->height]);
	}
	else if(s == TVD_SOURCE_PAL)
	{
		pdat->fmt = VIDEO_FORMAT_NV12;
		pdat->width = 720;
		pdat->height = 576;
		pdat->buflen = pdat->width * pdat->height * 2;
		t113_tvd_config(pdat, s, TVD_PL_YUV420);
		t113_tvd_set_wb_fmt(pdat, TVD_PL_YUV420);
		t113_tvd_set_wb_uv_swap(pdat, 0);
		t113_tvd_set_wb_addr(pdat, &pdat->yc[0], &pdat->yc[pdat->width * pdat->height]);
	}
	t113_tvd_set_blue(pdat, 2);
	t113_tvd_capture_off(pdat);
	t113_tvd_irq_clear_all(pdat);
	t113_tvd_irq_disable(pdat);
}

static int cam_start(struct camera_t * cam, enum video_format_t fmt, int width, int height)
{
	struct cam_t113_tvd_pdata_t * pdat = (struct cam_t113_tvd_pdata_t *)cam->priv;

	t113_tvd_init(pdat);
	t113_tvd_irq_enable(pdat);
	t113_tvd_irq_clear_all(pdat);
	t113_tvd_capture_on(pdat);
	return 1;
}

static int cam_stop(struct camera_t * cam)
{
	struct cam_t113_tvd_pdata_t * pdat = (struct cam_t113_tvd_pdata_t *)cam->priv;

	t113_tvd_capture_off(pdat);
	t113_tvd_irq_clear_all(pdat);
	t113_tvd_irq_disable(pdat);
	return 1;
}

static int cam_capture(struct camera_t * cam, struct video_frame_t * frame)
{
	struct cam_t113_tvd_pdata_t * pdat = (struct cam_t113_tvd_pdata_t *)cam->priv;

	if(t113_tvd_irq_status(pdat))
	{
		t113_tvd_irq_clear(pdat);
		frame->fmt = pdat->fmt;
		frame->width = pdat->width;
		frame->height = pdat->height;
		frame->buflen = pdat->buflen;
		frame->buf = pdat->yc;
		return 1;
	}
	return 0;
}

static int cam_ioctl(struct camera_t * cam, const char * cmd, void * arg)
{
	struct cam_t113_tvd_pdata_t * pdat = (struct cam_t113_tvd_pdata_t *)cam->priv;
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
			t113_tvd_set_saturation(pdat, *p);
			return 0;
		}
		break;
	case 0x0ed48a72: /* "camera-get-saturation" */
		if(p)
		{
			*p = t113_tvd_get_saturation(pdat);
			return 0;
		}
		break;
	case 0xdae4842d: /* "camera-set-brightness" */
		if(p)
		{
			t113_tvd_set_brightness(pdat, *p);
			return 0;
		}
		break;
	case 0x13e1d0a1: /* "camera-get-brightness" */
		if(p)
		{
			*p = t113_tvd_get_brightness(pdat);
			return 0;
		}
		break;
	case 0xf3916322: /* "camera-set-contrast" */
		if(p)
		{
			t113_tvd_set_contrast(pdat, *p);
			return 0;
		}
		break;
	case 0xa8290296: /* "camera-get-contrast" */
		if(p)
		{
			*p = t113_tvd_get_contrast(pdat);
			return 0;
		}
		break;
	default:
		break;
	}
	return -1;
}

static struct device_t * cam_t113_tvd_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cam_t113_tvd_pdata_t * pdat;
	struct camera_t * cam;
	struct device_t * dev;
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct cam_t113_tvd_pdata_t));
	if(!pdat)
		return NULL;

	cam = malloc(sizeof(struct camera_t));
	if(!cam)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt_tvd_top = phys_to_virt(T113_TVD_TOP_BASE);
	pdat->virt_tvd = phys_to_virt(dt_read_address(n));
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->channel = clamp(dt_read_int(n, "channel", 0), 0, 1);

	cam->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	cam->start = cam_start;
	cam->stop = cam_stop;
	cam->capture = cam_capture;
	cam->ioctl = cam_ioctl;
	cam->priv = pdat;

	clk_enable(pdat->clk);
	reset_deassert(T113_TVD_TOP_RESET);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);

	if(!(dev = register_camera(cam, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(cam->name);
		free(cam->priv);
		free(cam);
		return NULL;
	}
	return dev;
}

static void cam_t113_tvd_remove(struct device_t * dev)
{
	struct camera_t * cam = (struct camera_t *)dev->priv;
	struct cam_t113_tvd_pdata_t * pdat = (struct cam_t113_tvd_pdata_t *)cam->priv;

	if(cam)
	{
		unregister_camera(cam);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(cam->name);
		free(cam->priv);
		free(cam);
	}
}

static void cam_t113_tvd_suspend(struct device_t * dev)
{
}

static void cam_t113_tvd_resume(struct device_t * dev)
{
}

static struct driver_t cam_t113_tvd = {
	.name		= "cam-t113-tvd",
	.probe		= cam_t113_tvd_probe,
	.remove		= cam_t113_tvd_remove,
	.suspend	= cam_t113_tvd_suspend,
	.resume		= cam_t113_tvd_resume,
};

static __init void cam_t113_tvd_driver_init(void)
{
	register_driver(&cam_t113_tvd);
}

static __exit void cam_t113_tvd_driver_exit(void)
{
	unregister_driver(&cam_t113_tvd);
}

driver_initcall(cam_t113_tvd_driver_init);
driver_exitcall(cam_t113_tvd_driver_exit);
