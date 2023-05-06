/*
 * driver/g2d-t113.c
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
#include <dma/dma.h>
#include <g2d/g2d.h>
#include <t113/reg-g2d.h>

struct g2d_t113_pdata_t;

typedef enum
{
	G2D_FORMAT_ARGB8888,
	G2D_FORMAT_ABGR8888,
	G2D_FORMAT_RGBA8888,
	G2D_FORMAT_BGRA8888,
	G2D_FORMAT_XRGB8888,
	G2D_FORMAT_XBGR8888,
	G2D_FORMAT_RGBX8888,
	G2D_FORMAT_BGRX8888,
	G2D_FORMAT_RGB888,
	G2D_FORMAT_BGR888,
	G2D_FORMAT_RGB565,
	G2D_FORMAT_BGR565,
	G2D_FORMAT_ARGB4444,
	G2D_FORMAT_ABGR4444,
	G2D_FORMAT_RGBA4444,
	G2D_FORMAT_BGRA4444,
	G2D_FORMAT_ARGB1555,
	G2D_FORMAT_ABGR1555,
	G2D_FORMAT_RGBA5551,
	G2D_FORMAT_BGRA5551,
	G2D_FORMAT_ARGB2101010,
	G2D_FORMAT_ABGR2101010,
	G2D_FORMAT_RGBA1010102,
	G2D_FORMAT_BGRA1010102,

	/* invailed for UI channel */
	G2D_FORMAT_IYUV422_V0Y1U0Y0 = 0x20,
	G2D_FORMAT_IYUV422_Y1V0Y0U0,
	G2D_FORMAT_IYUV422_U0Y1V0Y0,
	G2D_FORMAT_IYUV422_Y1U0Y0V0,

	G2D_FORMAT_YUV422UVC_V1U1V0U0,
	G2D_FORMAT_YUV422UVC_U1V1U0V0,
	G2D_FORMAT_YUV422_PLANAR,

	G2D_FORMAT_YUV420UVC_V1U1V0U0 = 0x28,
	G2D_FORMAT_YUV420UVC_U1V1U0V0,
	G2D_FORMAT_YUV420_PLANAR,

	G2D_FORMAT_YUV411UVC_V1U1V0U0 = 0x2c,
	G2D_FORMAT_YUV411UVC_U1V1U0V0,
	G2D_FORMAT_YUV411_PLANAR,

	G2D_FORMAT_Y8 = 0x30,

	/* YUV 10bit format */
	G2D_FORMAT_YVU10_P010 = 0x34,

	G2D_FORMAT_YVU10_P210 = 0x36,

	G2D_FORMAT_YVU10_444 = 0x38,
	G2D_FORMAT_YUV10_444 = 0x39,
	G2D_FORMAT_MAX,
} g2d_fmt_enh;

typedef enum
{
	G2D_BLT_NONE = 0x00000000,
	G2D_BLT_PIXEL_ALPHA = 0x00000001,
	G2D_BLT_PLANE_ALPHA = 0x00000002,
	G2D_BLT_MULTI_ALPHA = 0x00000004,
	G2D_BLT_SRC_COLORKEY = 0x00000008,
	G2D_BLT_DST_COLORKEY = 0x00000010,
	G2D_BLT_FLIP_HORIZONTAL = 0x00000020,
	G2D_BLT_FLIP_VERTICAL = 0x00000040,
	G2D_BLT_ROTATE90 = 0x00000080,
	G2D_BLT_ROTATE180 = 0x00000100,
	G2D_BLT_ROTATE270 = 0x00000200,
	G2D_BLT_MIRROR45 = 0x00000400,
	G2D_BLT_MIRROR135 = 0x00000800,
	G2D_BLT_SRC_PREMULTIPLY = 0x00001000,
	G2D_BLT_DST_PREMULTIPLY = 0x00002000,
} g2d_blt_flags;

/* BLD LAYER ALPHA MODE*/
typedef enum
{
	G2D_PIXEL_ALPHA, G2D_GLOBAL_ALPHA, G2D_MIXER_ALPHA,
} g2d_alpha_mode_enh;

typedef struct
{
	int32_t x; /* left top point coordinate x */
	int32_t y; /* left top point coordinate y */
	uint32_t w; /* rectangle width */
	uint32_t h; /* rectangle height */
} g2d_rect;

struct g2d_image_t
{
	g2d_fmt_enh format;
	uint32_t laddr[3];
	uint32_t haddr[3];
	uint32_t width;
	uint32_t height;
	uint32_t align[3];
	g2d_rect clip_rect;
	int bpremul;
	uint8_t alpha;
	g2d_alpha_mode_enh mode;
};

#define G2DX_BASE 0x05410000

#define G2DX_TOP        (0x00000)
#define G2DX_MIXER      (0x00100)
#define G2DX_BLD        (0x00400)
#define G2DX_V0         (0x00800)
#define G2DX_UI0        (0x01000)
#define G2DX_UI1        (0x01800)
#define G2DX_UI2        (0x02000)
#define G2DX_WB         (0x03000)
#define G2DX_VSU        (0x08000)
#define G2DX_ROT        (0x28000)
#define G2DX_GSU        (0x30000)

/* register offset */
/* TOP register */
#define G2DX_SCLK_GATE  (0x00 + G2DX_TOP)
#define G2DX_HCLK_GATE  (0x04 + G2DX_TOP)
#define G2DX_AHB_RESET  (0x08 + G2DX_TOP)
#define G2DX_SCLK_DIV   (0x0C + G2DX_TOP)

/* MIXER GLB register */
#define G2DX_MIXER_CTL  (0x00 + G2DX_MIXER)
#define G2DX_MIXER_INT  (0x04 + G2DX_MIXER)
#define G2DX_MIXER_CLK  (0x08 + G2DX_MIXER)

/* LAY VIDEO register */
#define XV0_ATTCTL      (0x00 + G2DX_V0)
#define XV0_MBSIZE      (0x04 + G2DX_V0)
#define XV0_COOR        (0x08 + G2DX_V0)
#define XV0_PITCH0      (0x0C + G2DX_V0)
#define XV0_PITCH1      (0x10 + G2DX_V0)
#define XV0_PITCH2      (0x14 + G2DX_V0)
#define XV0_LADD0       (0x18 + G2DX_V0)
#define XV0_LADD1       (0x1C + G2DX_V0)
#define XV0_LADD2       (0x20 + G2DX_V0)
#define XV0_FILLC       (0x24 + G2DX_V0)
#define XV0_HADD        (0x28 + G2DX_V0)
#define XV0_SIZE        (0x2C + G2DX_V0)
#define XV0_HDS_CTL0    (0x30 + G2DX_V0)
#define XV0_HDS_CTL1    (0x34 + G2DX_V0)
#define XV0_VDS_CTL0    (0x38 + G2DX_V0)
#define XV0_VDS_CTL1    (0x3C + G2DX_V0)

/* LAY0 UI register */
#define XUI0_ATTR       (0x00 + G2DX_UI0)
#define XUI0_MBSIZE     (0x04 + G2DX_UI0)
#define XUI0_COOR       (0x08 + G2DX_UI0)
#define XUI0_PITCH      (0x0C + G2DX_UI0)
#define XUI0_LADD       (0x10 + G2DX_UI0)
#define XUI0_FILLC      (0x14 + G2DX_UI0)
#define XUI0_HADD       (0x18 + G2DX_UI0)
#define XUI0_SIZE       (0x1C + G2DX_UI0)

/* LAY1 UI register */
#define XUI1_ATTR       (0x00 + G2DX_UI1)
#define XUI1_MBSIZE     (0x04 + G2DX_UI1)
#define XUI1_COOR       (0x08 + G2DX_UI1)
#define XUI1_PITCH      (0x0C + G2DX_UI1)
#define XUI1_LADD       (0x10 + G2DX_UI1)
#define XUI1_FILLC      (0x14 + G2DX_UI1)
#define XUI1_HADD       (0x18 + G2DX_UI1)
#define XUI1_SIZE       (0x1C + G2DX_UI1)

/* LAY2 UI register */
#define XUI2_ATTR       (0x00 + G2DX_UI2)
#define XUI2_MBSIZE     (0x04 + G2DX_UI2)
#define XUI2_COOR       (0x08 + G2DX_UI2)
#define XUI2_PITCH      (0x0C + G2DX_UI2)
#define XUI2_LADD       (0x10 + G2DX_UI2)
#define XUI2_FILLC      (0x14 + G2DX_UI2)
#define XUI2_HADD       (0x18 + G2DX_UI2)
#define XUI2_SIZE       (0x1C + G2DX_UI2)

/* VSU register */
#define XVS_CTRL           (0x000 + G2DX_VSU)
#define XVS_OUT_SIZE       (0x040 + G2DX_VSU)
#define XVS_GLB_ALPHA      (0x044 + G2DX_VSU)
#define XVS_Y_SIZE         (0x080 + G2DX_VSU)
#define XVS_Y_HSTEP        (0x088 + G2DX_VSU)
#define XVS_Y_VSTEP        (0x08C + G2DX_VSU)
#define XVS_Y_HPHASE       (0x090 + G2DX_VSU)
#define XVS_Y_VPHASE0      (0x098 + G2DX_VSU)
#define XVS_C_SIZE         (0x0C0 + G2DX_VSU)
#define XVS_C_HSTEP        (0x0C8 + G2DX_VSU)
#define XVS_C_VSTEP        (0x0CC + G2DX_VSU)
#define XVS_C_HPHASE       (0x0D0 + G2DX_VSU)
#define XVS_C_VPHASE0      (0x0D8 + G2DX_VSU)
#define XVS_Y_HCOEF0       (0x200 + G2DX_VSU)
#define XVS_Y_VCOEF0       (0x300 + G2DX_VSU)
#define XVS_C_HCOEF0       (0x400 + G2DX_VSU)

/* BLD register */
#define XBLD_EN_CTL         (0x000 + G2DX_BLD)
#define XBLD_FILLC0         (0x010 + G2DX_BLD)
#define XBLD_FILLC1         (0x014 + G2DX_BLD)
#define XBLD_CH_ISIZE0      (0x020 + G2DX_BLD)
#define XBLD_CH_ISIZE1      (0x024 + G2DX_BLD)
#define XBLD_CH_OFFSET0     (0x030 + G2DX_BLD)
#define XBLD_CH_OFFSET1     (0x034 + G2DX_BLD)
#define XBLD_PREMUL_CTL     (0x040 + G2DX_BLD)
#define XBLD_BK_COLOR       (0x044 + G2DX_BLD)
#define XBLD_SIZE           (0x048 + G2DX_BLD)
#define XBLD_CTL            (0x04C + G2DX_BLD)
#define XBLD_KEY_CTL        (0x050 + G2DX_BLD)
#define XBLD_KEY_CON        (0x054 + G2DX_BLD)
#define XBLD_KEY_MAX        (0x058 + G2DX_BLD)
#define XBLD_KEY_MIN        (0x05C + G2DX_BLD)
#define XBLD_OUT_COLOR      (0x060 + G2DX_BLD)
#define XROP_CTL            (0x080 + G2DX_BLD)
#define XROP_INDEX0         (0x084 + G2DX_BLD)
#define XROP_INDEX1         (0x088 + G2DX_BLD)
#define XBLD_CSC_CTL        (0x100 + G2DX_BLD)
#define XBLD_CSC0_COEF00    (0x110 + G2DX_BLD)
#define XBLD_CSC0_COEF01    (0x114 + G2DX_BLD)
#define XBLD_CSC0_COEF02    (0x118 + G2DX_BLD)
#define XBLD_CSC0_CONST0    (0x11C + G2DX_BLD)
#define XBLD_CSC0_COEF10    (0x120 + G2DX_BLD)
#define XBLD_CSC0_COEF11    (0x124 + G2DX_BLD)
#define XBLD_CSC0_COEF12    (0x128 + G2DX_BLD)
#define XBLD_CSC0_CONST1    (0x12C + G2DX_BLD)
#define XBLD_CSC0_COEF20    (0x130 + G2DX_BLD)
#define XBLD_CSC0_COEF21    (0x134 + G2DX_BLD)
#define XBLD_CSC0_COEF22    (0x138 + G2DX_BLD)
#define XBLD_CSC0_CONST2    (0x13C + G2DX_BLD)
#define XBLD_CSC1_COEF00    (0x140 + G2DX_BLD)
#define XBLD_CSC1_COEF01    (0x144 + G2DX_BLD)
#define XBLD_CSC1_COEF02    (0x148 + G2DX_BLD)
#define XBLD_CSC1_CONST0    (0x14C + G2DX_BLD)
#define XBLD_CSC1_COEF10    (0x150 + G2DX_BLD)
#define XBLD_CSC1_COEF11    (0x154 + G2DX_BLD)
#define XBLD_CSC1_COEF12    (0x158 + G2DX_BLD)
#define XBLD_CSC1_CONST1    (0x15C + G2DX_BLD)
#define XBLD_CSC1_COEF20    (0x160 + G2DX_BLD)
#define XBLD_CSC1_COEF21    (0x164 + G2DX_BLD)
#define XBLD_CSC1_COEF22    (0x168 + G2DX_BLD)
#define XBLD_CSC1_CONST2    (0x16C + G2DX_BLD)
#define XBLD_CSC2_COEF00    (0x170 + G2DX_BLD)
#define XBLD_CSC2_COEF01    (0x174 + G2DX_BLD)
#define XBLD_CSC2_COEF02    (0x178 + G2DX_BLD)
#define XBLD_CSC2_CONST0    (0x17C + G2DX_BLD)
#define XBLD_CSC2_COEF10    (0x180 + G2DX_BLD)
#define XBLD_CSC2_COEF11    (0x184 + G2DX_BLD)
#define XBLD_CSC2_COEF12    (0x188 + G2DX_BLD)
#define XBLD_CSC2_CONST1    (0x18C + G2DX_BLD)
#define XBLD_CSC2_COEF20    (0x190 + G2DX_BLD)
#define XBLD_CSC2_COEF21    (0x194 + G2DX_BLD)
#define XBLD_CSC2_COEF22    (0x198 + G2DX_BLD)
#define XBLD_CSC2_CONST2    (0x19C + G2DX_BLD)

/* WB register */
#define XWB_ATT             (0x00 + G2DX_WB)
#define XWB_SIZE            (0x04 + G2DX_WB)
#define XWB_PITCH0          (0x08 + G2DX_WB)
#define XWB_PITCH1          (0x0C + G2DX_WB)
#define XWB_PITCH2          (0x10 + G2DX_WB)
#define XWB_LADD0           (0x14 + G2DX_WB)
#define XWB_HADD0           (0x18 + G2DX_WB)
#define XWB_LADD1           (0x1C + G2DX_WB)
#define XWB_HADD1           (0x20 + G2DX_WB)
#define XWB_LADD2           (0x24 + G2DX_WB)
#define XWB_HADD2           (0x28 + G2DX_WB)

/* Rotate register */
#define XROT_CTL            (0x00 + G2DX_ROT)
#define XROT_INT            (0x04 + G2DX_ROT)
#define XROT_TIMEOUT        (0x08 + G2DX_ROT)
#define XROT_IFMT           (0x20 + G2DX_ROT)
#define XROT_ISIZE          (0x24 + G2DX_ROT)
#define XROT_IPITCH0        (0x30 + G2DX_ROT)
#define XROT_IPITCH1        (0x34 + G2DX_ROT)
#define XROT_IPITCH2        (0x38 + G2DX_ROT)
#define XROT_ILADD0         (0x40 + G2DX_ROT)
#define XROT_IHADD0         (0x44 + G2DX_ROT)
#define XROT_ILADD1         (0x48 + G2DX_ROT)
#define XROT_IHADD1         (0x4C + G2DX_ROT)
#define XROT_ILADD2         (0x50 + G2DX_ROT)
#define XROT_IHADD2         (0x54 + G2DX_ROT)
#define XROT_OSIZE          (0x84 + G2DX_ROT)
#define XROT_OPITCH0        (0x90 + G2DX_ROT)
#define XROT_OPITCH1        (0x94 + G2DX_ROT)
#define XROT_OPITCH2        (0x98 + G2DX_ROT)
#define XROT_OLADD0         (0xA0 + G2DX_ROT)
#define XROT_OHADD0         (0xA4 + G2DX_ROT)
#define XROT_OLADD1         (0xA8 + G2DX_ROT)
#define XROT_OHADD1         (0xAC + G2DX_ROT)
#define XROT_OLADD2         (0xB0 + G2DX_ROT)
#define XROT_OHADD2         (0xB4 + G2DX_ROT)

#define get_wvalue(n)					(*((volatile uint32_t *)(n)))
#define put_wvalue(n, c)				(*((volatile uint32_t *)(n)) = (c))

#define read_wvalue(offset)				get_wvalue(0x05410000 + offset)
#define write_wvalue(offset, value)		put_wvalue(0x05410000 + offset, value)

static int32_t g2d_byte_cal(uint32_t format, uint32_t *ycnt, uint32_t *ucnt, uint32_t *vcnt)
{
	*ycnt = 0;
	*ucnt = 0;
	*vcnt = 0;
	if(format <= G2D_FORMAT_BGRX8888)
		*ycnt = 4;

	else if(format <= G2D_FORMAT_BGR888)
		*ycnt = 3;

	else if(format <= G2D_FORMAT_BGRA5551)
		*ycnt = 2;

	else if(format <= G2D_FORMAT_BGRA1010102)
		*ycnt = 4;

	else if(format <= 0x23)
	{
		*ycnt = 2;
	}

	else if(format <= 0x25)
	{
		*ycnt = 1;
		*ucnt = 2;
	}

	else if(format == 0x26)
	{
		*ycnt = 1;
		*ucnt = 1;
		*vcnt = 1;
	}

	else if(format <= 0x29)
	{
		*ycnt = 1;
		*ucnt = 2;
	}

	else if(format == 0x2a)
	{
		*ycnt = 1;
		*ucnt = 1;
		*vcnt = 1;
	}

	else if(format <= 0x2d)
	{
		*ycnt = 1;
		*ucnt = 2;
	}

	else if(format == 0x2e)
	{
		*ycnt = 1;
		*ucnt = 1;
		*vcnt = 1;
	}

	else if(format == 0x30)
		*ycnt = 1;

	else if(format <= 0x36)
	{
		*ycnt = 2;
		*ucnt = 4;
	}

	else if(format <= 0x39)
		*ycnt = 6;
	return 0;
}

static uint32_t cal_align(uint32_t width, uint32_t align)
{
	switch(align)
	{
	case 0:
		return width;
	case 4:
		return (width + 3) >> 1 << 1;
	case 8:
		return (width + 7) >> 3 << 3;
	case 16:
		return (width + 15) >> 4 << 4;
	case 32:
		return (width + 31) >> 5 << 5;
	case 64:
		return (width + 63) >> 6 << 6;
	case 128:
		return (width + 127) >> 7 << 7;
	default:
		return (width + 31) >> 5 << 5;
	}
}

static int32_t g2d_vlayer_set(struct g2d_t113_pdata_t *pdat, uint32_t sel, struct g2d_image_t *image)
{
	unsigned long long addr0, addr1, addr2;
	uint32_t tmp;
	uint32_t ycnt, ucnt, vcnt;
	uint32_t pitch0, pitch1, pitch2;
	uint32_t /*ch, */cw, cy, cx;

	switch(sel)
	{
	case 0:
		/* base_addr = G2D_V0; */
		break;
	default:
		return -1;
	}
	tmp = ((image->alpha & 0xff) << 24);
	if(image->bpremul)
		tmp |= (0x1 << 17);
	tmp |= (image->format << 8);
	tmp |= (image->mode << 1);
	tmp |= 1;
	write_wvalue(XV0_ATTCTL, tmp);
	tmp = (((image->clip_rect.h == 0 ? 0 : image->clip_rect.h - 1) & 0x1fff) << 16) | ((image->clip_rect.w == 0 ? 0 : image->clip_rect.w - 1) & 0x1fff);
	write_wvalue(XV0_MBSIZE, tmp);

	/* offset is set to 0, ovl size is set to layer size */
	write_wvalue(XV0_SIZE, tmp);
	write_wvalue(XV0_COOR, 0);
	if((image->format >= G2D_FORMAT_YUV422UVC_V1U1V0U0) && (image->format <= G2D_FORMAT_YUV422_PLANAR))
	{
		cw = image->width >> 1;
		//ch = image->height;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y;
	}

	else if((image->format >= G2D_FORMAT_YUV420UVC_V1U1V0U0) && (image->format <= G2D_FORMAT_YUV420_PLANAR))
	{
		cw = image->width >> 1;
		//ch = image->height >> 1;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y >> 1;
	}

	else if((image->format >= G2D_FORMAT_YUV411UVC_V1U1V0U0) && (image->format <= G2D_FORMAT_YUV411_PLANAR))
	{
		cw = image->width >> 2;
		//ch = image->height;
		cx = image->clip_rect.x >> 2;
		cy = image->clip_rect.y;
	}

	else
	{
		cw = 0;
		//ch = 0;
		cx = 0;
		cy = 0;
	}
	g2d_byte_cal(image->format, &ycnt, &ucnt, &vcnt);
	pitch0 = cal_align(ycnt * image->width, image->align[0]);
	write_wvalue(XV0_PITCH0, pitch0);
	pitch1 = cal_align(ucnt * cw, image->align[1]);
	write_wvalue(XV0_PITCH1, pitch1);
	pitch2 = cal_align(vcnt * cw, image->align[2]);
	write_wvalue(XV0_PITCH2, pitch2);
	addr0 = image->laddr[0] + ((uint64_t)image->haddr[0] << 32) + pitch0 * image->clip_rect.y + ycnt * image->clip_rect.x;
	write_wvalue(XV0_LADD0, addr0 & 0xffffffff);
	addr1 = image->laddr[1] + ((uint64_t)image->haddr[1] << 32) + pitch1 * cy + ucnt * cx;
	write_wvalue(XV0_LADD1, addr1 & 0xffffffff);
	addr2 = image->laddr[2] + ((uint64_t)image->haddr[2] << 32) + pitch2 * cy + vcnt * cx;
	write_wvalue(XV0_LADD2, addr2 & 0xffffffff);
	tmp = ((addr0 >> 32) & 0xff) | ((addr1 >> 32) & 0xff) << 8 | ((addr2 >> 32) & 0xff) << 16;
	write_wvalue(XV0_HADD, tmp);
	return 0;
}

static int32_t g2d_uilayer_set(struct g2d_t113_pdata_t *pdat, uint32_t sel, struct g2d_image_t *img)
{
	uint64_t addr0;
	uint32_t base_addr_u, tmp;
	uint32_t ycnt, ucnt, vcnt;
	uint32_t pitch0;

	switch(sel)
	{
	case 0:
		base_addr_u = G2DX_UI0;
		break;
	case 1:
		base_addr_u = G2DX_UI1;
		break;
	case 2:
		base_addr_u = G2DX_UI2;
		break;
	default:
		return -1;
	}
	tmp = (img->alpha & 0xff) << 24;
	if(img->bpremul)
		tmp |= 0x1 << 17;
	tmp |= img->format << 8;
	tmp |= img->mode << 1;
	tmp |= 1;
	write_wvalue(base_addr_u, tmp);
	tmp = (((img->clip_rect.h == 0 ? 0 : img->clip_rect.h - 1) & 0x1fff) << 16) | ((img->clip_rect.w == 0 ? 0 : img->clip_rect.w - 1) & 0x1fff);
	write_wvalue(base_addr_u + 0x4, tmp);
	write_wvalue(base_addr_u + 0x1C, tmp);
	write_wvalue(base_addr_u + 0x8, 0);
	g2d_byte_cal(img->format, &ycnt, &ucnt, &vcnt);
	pitch0 = cal_align(ycnt * img->width, img->align[0]);
	write_wvalue(base_addr_u + 0xC, pitch0);
	addr0 = img->laddr[0] + ((uint64_t)img->haddr[0] << 32) + pitch0 * img->clip_rect.y + ycnt * img->clip_rect.x;
	write_wvalue(base_addr_u + 0x10, addr0 & 0xffffffff);
	write_wvalue(base_addr_u + 0x18, (addr0 >> 32) & 0xff);
	return 0;
}

static int32_t g2d_wb_set(struct g2d_t113_pdata_t *pdat, struct g2d_image_t *image)
{
	uint64_t addr0, addr1, addr2;
	uint32_t tmp;
	uint32_t ycnt, ucnt, vcnt;
	uint32_t pitch0, pitch1, pitch2;
	uint32_t /*ch, */cw, cy, cx;

	write_wvalue(XWB_ATT, image->format);
	tmp = (((image->clip_rect.h == 0 ? 0 : image->clip_rect.h - 1) & 0x1fff) << 16) | ((image->clip_rect.w == 0 ? 0 : image->clip_rect.w - 1) & 0x1fff);
	write_wvalue(XWB_SIZE, tmp);
	/*write to the bld out reg */
	write_wvalue(XBLD_SIZE, tmp);
	/* set outdata premul */
	tmp = read_wvalue(XBLD_OUT_COLOR);

	if(image->bpremul)
		write_wvalue(XBLD_OUT_COLOR, tmp | 0x1);
	else
		write_wvalue(XBLD_OUT_COLOR, tmp & 0x2);
	if((image->format >= G2D_FORMAT_YUV422UVC_V1U1V0U0) && (image->format <= G2D_FORMAT_YUV422_PLANAR))
	{
		cw = image->width >> 1;
		//ch = image->height;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y;
	}

	else if((image->format >= G2D_FORMAT_YUV420UVC_V1U1V0U0) && (image->format <= G2D_FORMAT_YUV420_PLANAR))
	{
		cw = image->width >> 1;
		//ch = image->height >> 1;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y >> 1;
	}

	else if((image->format >= G2D_FORMAT_YUV411UVC_V1U1V0U0) && (image->format <= G2D_FORMAT_YUV411_PLANAR))
	{
		cw = image->width >> 2;
		//ch = image->height;
		cx = image->clip_rect.x >> 2;
		cy = image->clip_rect.y;
	}

	else
	{
		cw = 0;
		//ch = 0;
		cx = 0;
		cy = 0;
	}
	g2d_byte_cal(image->format, &ycnt, &ucnt, &vcnt);
	pitch0 = cal_align(ycnt * image->width, image->align[0]);
	write_wvalue(XWB_PITCH0, pitch0);
	pitch1 = cal_align(ucnt * cw, image->align[1]);
	write_wvalue(XWB_PITCH1, pitch1);
	pitch2 = cal_align(vcnt * cw, image->align[2]);
	write_wvalue(XWB_PITCH2, pitch2);

	addr0 = image->laddr[0] + ((uint64_t)image->haddr[0] << 32) + pitch0 * image->clip_rect.y + ycnt * image->clip_rect.x;
	write_wvalue(XWB_LADD0, addr0 & 0xffffffff);
	write_wvalue(XWB_HADD0, (addr0 >> 32) & 0xff);
	addr1 = image->laddr[1] + ((uint64_t)image->haddr[1] << 32) + pitch1 * cy + ucnt * cx;
	write_wvalue(XWB_LADD1, addr1 & 0xffffffff);
	write_wvalue(XWB_HADD1, (addr1 >> 32) & 0xff);
	addr2 = image->laddr[2] + ((uint64_t)image->haddr[2] << 32) + pitch2 * cy + vcnt * cx;
	write_wvalue(XWB_LADD2, addr2 & 0xffffffff);
	write_wvalue(XWB_HADD2, (addr2 >> 32) & 0xff);
	return 0;
}

/*
 * sel: 0-->pipe0 1-->pipe1 other:error
 */
static int32_t g2d_bldin_set(struct g2d_t113_pdata_t *pdat, uint32_t sel, g2d_rect rect, int premul)
{
	uint32_t tmp;
	uint32_t offset;

	if(sel == 0)
	{
		offset = 0;
		tmp = read_wvalue(XBLD_EN_CTL);
		tmp |= 0x1 << 8;
		write_wvalue(XBLD_EN_CTL, tmp);
		if(premul)
		{
			tmp = read_wvalue(XBLD_PREMUL_CTL);
			tmp |= 0x1;
			write_wvalue(XBLD_PREMUL_CTL, tmp);
		}
	}
	else if(sel == 1)
	{
		offset = 0x4;
		tmp = read_wvalue(XBLD_EN_CTL);
		tmp |= 0x1 << 9;
		write_wvalue(XBLD_EN_CTL, tmp);
		if(premul)
		{
			tmp = read_wvalue(XBLD_PREMUL_CTL);
			tmp |= 0x1 << 1;
			write_wvalue(XBLD_PREMUL_CTL, tmp);
		}
	}
	else
		return -1;
	tmp = ((rect.h - 1) << 16) | (rect.w - 1);

	write_wvalue(XBLD_CH_ISIZE0 + offset, tmp);
	tmp = ((rect.y <= 0 ? 0 : rect.y - 1) << 16) | (rect.x <= 0 ? 0 : rect.x - 1);

	write_wvalue(XBLD_CH_OFFSET0 + offset, tmp);
	return 0;
}

struct g2d_t113_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
	struct mutex_t m;
};

static inline int dcmp(double a, double b)
{
	return (fabs(a - b) < 0.000001);
}

static inline u32_t lo32(void * addr)
{
	return (u32_t)addr;
}

static inline u32_t hi32(void * addr)
{
	if(sizeof(addr) == sizeof(u32_t))
		return 0;
	return (u32_t)(((u64_t)(unsigned long)addr) >> 32);
}

static inline void t113_g2d_init(struct g2d_t113_pdata_t * pdat)
{
	struct g2d_top_t * g2d_top = (struct g2d_top_t *)(pdat->virt + T113_G2D_TOP);

	g2d_top->G2D_SCLK_GATE = 0x3;
	g2d_top->G2D_HCLK_GATE = 0x3;
	g2d_top->G2D_SCLK_DIV = (0x0 << 4) | (0x0 << 0);
	g2d_top->G2D_AHB_RESET = 0x0;
	g2d_top->G2D_AHB_RESET = 0x3;
}

static inline void t113_g2d_mixer_reset(struct g2d_t113_pdata_t * pdat)
{
	struct g2d_top_t * g2d_top = (struct g2d_top_t *)(pdat->virt + T113_G2D_TOP);

	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET & ~(0x1 << 0);
	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET | (0x1 << 0);
}

static inline void t113_g2d_rot_reset(struct g2d_t113_pdata_t * pdat)
{
	struct g2d_top_t * g2d_top = (struct g2d_top_t *)(pdat->virt + T113_G2D_TOP);

	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET & ~(0x1 << 1);
	g2d_top->G2D_AHB_RESET = g2d_top->G2D_AHB_RESET | (0x1 << 1);
}

static inline void t113_g2d_mixer_start(struct g2d_t113_pdata_t * pdat)
{
	struct g2d_mixer_t * g2d_mixer = (struct g2d_mixer_t *)(pdat->virt + T113_G2D_MIXER);

	g2d_mixer->G2D_MIXER_INT = 0x10;
	g2d_mixer->G2D_MIXER_CTL |= (0x1 << 31);
}

static inline int t113_g2d_wait(struct g2d_t113_pdata_t * pdat, int timeout)
{
	struct g2d_mixer_t * g2d_mixer = (struct g2d_mixer_t *)(pdat->virt + T113_G2D_MIXER);
	struct g2d_rot_t * g2d_rot = (struct g2d_rot_t *)(pdat->virt + T113_G2D_ROT);
	ktime_t t = ktime_add_ms(ktime_get(), timeout);

	do {
		if(g2d_mixer->G2D_MIXER_INT & (0x1 << 0))
		{
			g2d_mixer->G2D_MIXER_INT = 0x1 << 0;
			t113_g2d_mixer_reset(pdat);
			return 1;
		}
		else if(g2d_rot->ROT_INT & (0x1 << 0))
		{
			g2d_rot->ROT_INT = 0x1 << 0;
			t113_g2d_rot_reset(pdat);
			return 1;
		}
	} while(ktime_before(ktime_get(), t));

	return 0;
}

#if 0
static int t113_g2d_blit(struct g2d_t113_pdata_t *pdat, uint32_t *dst, uint16_t tdx, uint16_t tdy, uint32_t *src, uint16_t sdx, uint16_t sdy, uint16_t sw,
        uint16_t sh)
{
	struct g2d_vsu_t * g2d_vsu = (struct g2d_vsu_t *)(pdat->virt + T113_G2D_VSU);
	struct g2d_rot_t * g2d_rot = (struct g2d_rot_t *)(pdat->virt + T113_G2D_ROT);
	struct g2d_bld_t * g2d_bld = (struct g2d_bld_t *)(pdat->virt + T113_G2D_BLD);
	struct g2d_wb_t * g2d_wb = (struct g2d_wb_t *)(pdat->virt + T113_G2D_WB);
	struct g2d_vi_t * g2d_vi = (struct g2d_vi_t *)(pdat->virt + T113_G2D_VI);
	struct g2d_ui_t * g2d_ui0 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI0);
	struct g2d_ui_t * g2d_ui1 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI1);
	struct g2d_ui_t * g2d_ui2 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI2);

	int ret;

	unsigned tstride = (tdx) * 4;
	unsigned sstride = (sdx) * 4;
	void * taddr = (void * ) dst;
	void * saddr = (void * ) src;
	uint32_t ssizehw = ((sh - 1) << 16) | ((sw - 1) << 0);
	uint32_t tsizehw = ((sh - 1) << 16) | ((sw - 1) << 0);

//	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
//	dcache_clean(srcinvalidateaddr, srcinvalidatesize);


	mutex_lock(&pdat->m);

	g2d_vsu->VS_CTRL = 0;
	g2d_rot->ROT_CTL = 0;
	g2d_bld->BLD_EN_CTL = 0;
	g2d_vi->V0_ATTR = 0;
	g2d_ui0->UI_ATTR = 0;
	g2d_ui1->UI_ATTR = 0;
	g2d_ui2->UI_ATTR = 0;

	g2d_bld->BLD_KEY_CTL = 0;

	if (0)
	{
		uint32_t keycolor24 = 0x00ff00ff;
		/* 5.10.9.10 BLD color key control register */
		//g2d_bld->BLD_KEY_CTL = 0x03;	/* G2D_CK_SRC = 0x03, G2D_CK_DST = 0x01 */
		g2d_bld->BLD_KEY_CTL =
			(0x01u << 1) |		// KEY0_MATCH_DIR 1: when the pixel value matches source image, it displays the pixel form destination image.
			(1u << 0) |			// KEY0_EN 1: enable color key in Alpha Blender0.
			0;

		/* 5.10.9.11 BLD color key configuration register */
		g2d_bld->BLD_KEY_CFG =
			0 * (1u << 2) |		// KEY0R_MATCH 0: match color if value inside keys range
			0 * (1u << 1) |		// KEY0G_MATCH 0: match color if value inside keys range
			0 * (1u << 0) |		// KEY0B_MATCH 0: match color if value inside keys range
			0;

		g2d_bld->BLD_KEY_MAX = keycolor24;
		g2d_bld->BLD_KEY_MIN = keycolor24;

		/* установка поверхности - источника (анализируется) */
		g2d_ui2->UI_ATTR = 0x0;//awxx_get_ui_attr(srcFormat);
		g2d_ui2->UI_PITCH = sstride;
		g2d_ui2->UI_FILLC = 0;
		g2d_ui2->UI_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		g2d_ui2->UI_MBSIZE = ssizehw; // сколько брать от исходного буфера
		g2d_ui2->UI_SIZE = ssizehw;		// параметры окна исходного буфера
		g2d_ui2->UI_LADD = lo32(saddr);
		g2d_ui2->UI_HADD = hi32(saddr);

		/* эта поверхность источник данных когда есть совпадение с ключевым цветом */
		g2d_vi->V0_ATTR = 0;
		g2d_vi->V0_PITCH0 = tstride;
		g2d_vi->V0_FILLC = 0;
		g2d_vi->V0_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		g2d_vi->V0_MBSIZE = tsizehw; 	// сколько брать от исходного буфера
		g2d_vi->V0_SIZE = tsizehw;		// параметры окна исходного буфера
		g2d_vi->V0_LADD0 = lo32(taddr);
		g2d_vi->V0_HADD = hi32(taddr);

		g2d_bld->BLD_SIZE = tsizehw;	// размер выходного буфера после scaler
		/* источник когда есть совпадние ??? */
		g2d_bld->BLD_CH_ISIZE [0] = tsizehw;
		g2d_bld->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);
		/* источник для анализа ??? */
		g2d_bld->BLD_CH_ISIZE [1] = tsizehw;
		g2d_bld->BLD_CH_OFFSET [1] = 0;// ((row) << 16) | ((col) << 0);

		g2d_bld->BLD_EN_CTL =
			(1u << 8) |	// 8: P0_EN Pipe0 enable
			(1u << 9) |	// 9: P1_EN Pipe1 enable
			0;

		g2d_bld->ROP_CTL = 0x00F0;	// 0x00F0 g2d_vi, 0x55F0 UI1, 0xAAF0 UI2

		g2d_bld->BLD_CTL = 0x03010301;//awxx_bld_ctl2(3, 1); //awxx_bld_ctl(3, 1, 3, 1); //0x03010301;	// G2D_BLD_SRCOVER - default value
	}
	else
	{
		/* без keycolor */
		/* установка поверхности - источника (безусловно) */
//		G2D_UI2->UI_ATTR = awxx_get_ui_attr();
//		G2D_UI2->UI_PITCH = sstride;
//		G2D_UI2->UI_FILLC = 0;
//		G2D_UI2->UI_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
//		G2D_UI2->UI_MBSIZE = ssizehw; // сколько брать от исходного буфера
//		G2D_UI2->UI_SIZE = ssizehw;		// параметры окна исходного буфера
//		G2D_UI2->UI_LADD = ptr_lo32(saddr);
//		G2D_UI2->UI_HADD = ptr_hi32(saddr);

        g2d_vi->V0_ATTR = 0x0;//xxx awxx_get_vi_attr(srcFormat);
        g2d_vi->V0_PITCH0 = sstride;
        g2d_vi->V0_FILLC = 0;//TFTRGB(255, 0, 0);    // unused
        g2d_vi->V0_COOR = 0;            // координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
        g2d_vi->V0_MBSIZE = ssizehw;     // сколько брать от исходного буфера
        g2d_vi->V0_SIZE = ssizehw;        // параметры окна исходного буфера
        g2d_vi->V0_LADD0 = lo32(saddr);
        g2d_vi->V0_HADD = hi32(saddr);

		g2d_bld->BLD_SIZE = tsizehw;	// размер выходного буфера после scaler
		g2d_bld->BLD_CH_ISIZE [0] = ssizehw;
		g2d_bld->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);
		g2d_bld->BLD_CH_ISIZE [1] = ssizehw;
		g2d_bld->BLD_CH_OFFSET [1] = 0;// ((row) << 16) | ((col) << 0);

		g2d_bld->BLD_KEY_CTL = 0;
		g2d_bld->BLD_KEY_CFG = 0;

		g2d_bld->BLD_EN_CTL =
			(1u << 8) |	// 8: P0_EN Pipe0 enable - VI0
			//(1u << 9) |	// 9: P1_EN Pipe1 enable - UI2
			0;

		g2d_bld->ROP_CTL = 0x00F0;	// 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2

		//g2d_bld->BLD_CTL = 0x00010001;	// G2D_BLD_COPY
		//g2d_bld->BLD_CTL = 0x00000000;	// G2D_BLD_CLEAR
		g2d_bld->BLD_CTL = 0x03010301;//awxx_bld_ctl2(3, 1); //awxx_bld_ctl(3, 1, 3, 1); //0x03010301;	// G2D_BLD_SRCOVER - default value
	}


	//g2d_bld->BLD_FILLC0 = ~ 0;
	//g2d_bld->BLD_PREMUL_CTL |= (1u << 0);	// 0 or 1 - sel 1 or sel 0
	g2d_bld->BLD_OUT_COLOR=0x00000000; /* 0x00000000 */
	g2d_bld->BLD_CSC_CTL=0x00000000; /* 0x00000000 */
	g2d_bld->BLD_BG_COLOR = 0;
	g2d_bld->BLD_PREMUL_CTL=0x00000000; /* 0x00000000 */

	/* Write-back settings */
	g2d_wb->WB_ATTR = 0x0;//WB_ImageFormat;//G2D_FMT_RGB565; //G2D_FMT_XRGB8888;
	g2d_wb->WB_SIZE = tsizehw;
	g2d_wb->WB_PITCH0 = tstride;	/* taddr buffer stride */
	g2d_wb->WB_LADD0 = lo32(taddr);
	g2d_wb->WB_HADD0 = hi32(taddr);

	t113_g2d_mixer_start(pdat);
	ret = t113_g2d_wait(pdat, 100);
	mutex_unlock(&pdat->m);

	return ret;
}
#endif

static int g2d_bsp_bld(struct g2d_t113_pdata_t *pdat, struct g2d_image_t *src, struct g2d_image_t *dst)
{
	struct g2d_vsu_t * g2d_vsu = (struct g2d_vsu_t *)(pdat->virt + T113_G2D_VSU);
	struct g2d_rot_t * g2d_rot = (struct g2d_rot_t *)(pdat->virt + T113_G2D_ROT);
	struct g2d_bld_t * g2d_bld = (struct g2d_bld_t *)(pdat->virt + T113_G2D_BLD);
	struct g2d_wb_t * g2d_wb = (struct g2d_wb_t *)(pdat->virt + T113_G2D_WB);
	struct g2d_vi_t * g2d_vi = (struct g2d_vi_t *)(pdat->virt + T113_G2D_VI);
	struct g2d_ui_t * g2d_ui0 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI0);
	struct g2d_ui_t * g2d_ui1 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI1);
	struct g2d_ui_t * g2d_ui2 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI2);

	g2d_rect rect0, rect1;
	int ret;

	mutex_lock(&pdat->m);

	g2d_vlayer_set(pdat, 0, dst);
	g2d_uilayer_set(pdat, 2, src);

	g2d_bld->ROP_CTL = 0xf0;

	rect0.x = 0;
	rect0.y = 0;
	rect0.w = dst->clip_rect.w;
	rect0.h = dst->clip_rect.h;

	rect1.x = 0;
	rect1.y = 0;
	rect1.w = src->clip_rect.w;
	rect1.h = src->clip_rect.h;
	g2d_bldin_set(pdat, 0, rect0, dst->bpremul);
	g2d_bldin_set(pdat, 1, rect1, src->bpremul);

	g2d_bld->BLD_CTL = 0x03010301;
	g2d_bld->BLD_KEY_CTL = 0x3;

	g2d_wb_set(pdat, dst);

	t113_g2d_mixer_start(pdat);
	ret = t113_g2d_wait(pdat, 100);
	mutex_unlock(&pdat->m);

	return ret;
}

static int t113_g2d_blit(struct g2d_t113_pdata_t * pdat, struct surface_t * s, struct surface_t * o, int x, int y, int w, int h)
{
	struct g2d_image_t src_tmp, dst_tmp;
	struct g2d_image_t *src = &src_tmp;
	struct g2d_image_t *dst = &dst_tmp;
	int ret;

	memset(src, 0, sizeof(struct g2d_image_t));
	memset(dst, 0, sizeof(struct g2d_image_t));

	src->format = G2D_FORMAT_ARGB8888;
	src->laddr[0] = lo32(o->pixels);
	src->laddr[1] = 0;
	src->laddr[2] = 0;
	src->haddr[0] = hi32(o->pixels);
	src->haddr[1] = 0;
	src->haddr[2] = 0;
	src->width = o->width;
	src->height = o->height;
	src->clip_rect.x = 0;
	src->clip_rect.y = 0;
	src->clip_rect.w = w;
	src->clip_rect.h = h;
	src->alpha = 0xff;

	dst->format = G2D_FORMAT_ARGB8888;
	dst->laddr[0] = lo32(s->pixels);
	dst->laddr[1] = 0;
	dst->laddr[2] = 0;
	dst->haddr[0] = lo32(s->pixels);
	dst->haddr[1] = 0;
	dst->haddr[2] = 0;
	dst->width = s->width;;
	dst->height = s->height;
	dst->clip_rect.x = x;
	dst->clip_rect.y = y;
	dst->clip_rect.w = src->clip_rect.w;
	dst->clip_rect.h = src->clip_rect.h;
	dst->alpha = 0xff;

	src->bpremul = 1;
	dst->bpremul = 1;
	src->mode = G2D_MIXER_ALPHA;
	dst->mode = G2D_MIXER_ALPHA;

	dma_cache_sync(s->pixels, s->pixlen, DMA_TO_DEVICE);
	dma_cache_sync(o->pixels, o->pixlen, DMA_FROM_DEVICE);
	ret = g2d_bsp_bld(pdat, src, dst);
	return ret;
}

static int t113_g2d_fill(struct g2d_t113_pdata_t * pdat, struct surface_t * s, int x, int y, int w, int h, struct color_t * c)
{
	struct g2d_vsu_t * g2d_vsu = (struct g2d_vsu_t *)(pdat->virt + T113_G2D_VSU);
	struct g2d_rot_t * g2d_rot = (struct g2d_rot_t *)(pdat->virt + T113_G2D_ROT);
	struct g2d_bld_t * g2d_bld = (struct g2d_bld_t *)(pdat->virt + T113_G2D_BLD);
	struct g2d_wb_t * g2d_wb = (struct g2d_wb_t *)(pdat->virt + T113_G2D_WB);
	struct g2d_vi_t * g2d_vi = (struct g2d_vi_t *)(pdat->virt + T113_G2D_VI);
	struct g2d_ui_t * g2d_ui0 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI0);
	struct g2d_ui_t * g2d_ui1 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI1);
	struct g2d_ui_t * g2d_ui2 = (struct g2d_ui_t *)(pdat->virt + T113_G2D_UI2);
	void * addr = s->pixels + y * s->stride + (x << 2);
	u32_t hw = ((h - 1) << 16) | ((w - 1) << 0);
	int ret;

	mutex_lock(&pdat->m);
	g2d_vsu->VS_CTRL = 0x0;
	g2d_rot->ROT_CTL = 0x0;
	g2d_vi->V0_ATTR = 0x0;
	g2d_ui0->UI_ATTR = 0x0;
	g2d_ui1->UI_ATTR = 0x0;
	g2d_ui2->UI_ATTR = 0x0;

	g2d_bld->BLD_CH_ISIZE[0] = hw;
	g2d_bld->BLD_CH_OFFSET[0] = 0x0;
	g2d_bld->BLD_SIZE = hw;
	g2d_bld->BLD_FILL_COLOR[0] = color_get_premult(c);
	g2d_bld->BLD_EN_CTL = (0x1 << 8) | (0x1 << 0);
	g2d_bld->BLD_PREMUL_CTL = (0x1 << 0);
	g2d_bld->BLD_OUT_COLOR = (0x1 << 0);
	g2d_bld->ROP_CTL = 0x0;
	g2d_bld->BLD_CTL = 0x03010301;

	g2d_wb->WB_ATTR = 0x0;
	g2d_wb->WB_SIZE = hw;
	g2d_wb->WB_PITCH0 = s->stride;
	g2d_wb->WB_LADD0 = lo32(addr);
	g2d_wb->WB_HADD0 = hi32(addr);

	dma_cache_sync(s->pixels, s->pixlen, DMA_FROM_DEVICE);
	t113_g2d_mixer_start(pdat);
	ret = t113_g2d_wait(pdat, 100);
	mutex_unlock(&pdat->m);

	return ret;
}

static bool_t g2d_t113_blit(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * o)
{
	struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;
	struct region_t r, region;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, surface_get_width(o), surface_get_height(o), &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	if((r.w >= 2) && (r.h >= 2))
	{
		if(dcmp(m->b, 0) && dcmp(m->c, 0))
		{
			if(dcmp(m->a - m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0 */
				{
					return t113_g2d_blit(pdat, s, o, r.x, r.y, r.w, r.h);
				}
				else if(dcmp(m->a, -1))	/* rotate 180 */
				{
					return FALSE;
				}
			}
			else if(dcmp(m->a + m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0, v-flip | rotate 180, h-flip */
				{
					return FALSE;
				}
				else if(dcmp(m->a, -1))	/* rotate 0, h-flip | rotate 180, v-flip */
				{
					return FALSE;
				}
			}
		}
		else if(dcmp(m->a, 0) && dcmp(m->d, 0))
		{
			if(dcmp(m->b + m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90 */
				{
					return FALSE;
				}
				else if(dcmp(m->b, -1))	/* rotate 270 */
				{
					return FALSE;
				}
			}
			else if(dcmp(m->b - m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90, v-flip | rotate 270, h-flip */
				{
					return FALSE;
				}
				else if(dcmp(m->b, -1))	/* rotate 90, h-flip | rotate 270, v-flip */
				{
					return FALSE;
				}
			}
		}
	}
	return FALSE;
}

static bool_t g2d_t113_fill(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;
	struct region_t r, region;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, w, h, &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	if((r.w >= 2) && (r.h >= 2) && (c->a == 255))
	{
		if(dcmp(m->b, 0) && dcmp(m->c, 0))
		{
			if(dcmp(m->a - m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0 */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->a, -1))	/* rotate 180 */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
			else if(dcmp(m->a + m->d, 0))
			{
				if(dcmp(m->a, 1))		/* rotate 0, v-flip | rotate 180, h-flip */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->a, -1))	/* rotate 0, h-flip | rotate 180, v-flip */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
		}
		else if(dcmp(m->a, 0) && dcmp(m->d, 0))
		{
			if(dcmp(m->b + m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90 */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->b, -1))	/* rotate 270 */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
			else if(dcmp(m->b - m->c, 0))
			{
				if(dcmp(m->b, 1))		/* rotate 90, v-flip | rotate 270, h-flip */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
				else if(dcmp(m->b, -1))	/* rotate 90, h-flip | rotate 270, v-flip */
				{
					return t113_g2d_fill(pdat, s, r.x, r.y, r.w, r.h, c);
				}
			}
		}
	}
	return FALSE;
}

static struct device_t * g2d_t113_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct g2d_t113_pdata_t * pdat;
	struct g2d_t * g2d;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct g2d_t113_pdata_t));
	if(!pdat)
		return NULL;

	g2d = malloc(sizeof(struct g2d_t));
	if(!g2d)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->clk = strdup(clk);
	pdat->reset = dt_read_int(n, "reset", -1);
	mutex_init(&pdat->m);

	g2d->name = alloc_device_name(dt_read_name(n), -1);
	g2d->blit = g2d_t113_blit;
	g2d->fill = g2d_t113_fill;
	g2d->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	t113_g2d_init(pdat);

	if(!(dev = register_g2d(g2d, drv)))
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(g2d->name);
		free(g2d->priv);
		free(g2d);
		return NULL;
	}
	return dev;
}

static void g2d_t113_remove(struct device_t * dev)
{
	struct g2d_t * g2d = (struct g2d_t *)dev->priv;
	struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;

	if(g2d)
	{
		unregister_g2d(g2d);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(g2d->name);
		free(g2d->priv);
		free(g2d);
	}
}

static void g2d_t113_suspend(struct device_t * dev)
{
}

static void g2d_t113_resume(struct device_t * dev)
{
}

static struct driver_t g2d_t113 = {
	.name		= "g2d-t113",
	.probe		= g2d_t113_probe,
	.remove		= g2d_t113_remove,
	.suspend	= g2d_t113_suspend,
	.resume		= g2d_t113_resume,
};

static __init void g2d_t113_driver_init(void)
{
	register_driver(&g2d_t113);
}

static __exit void g2d_t113_driver_exit(void)
{
	unregister_driver(&g2d_t113);
}

driver_initcall(g2d_t113_driver_init);
driver_exitcall(g2d_t113_driver_exit);
