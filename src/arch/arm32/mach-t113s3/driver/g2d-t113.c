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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * g2d_bsp.h
 */
#define	G2D_TOP_BASE	 ((uintptr_t) 0x05410000)


#define G2D_INFO_MSG(...) do { } while (0)

#define pr_info(...) do { } while (0)
#define pr_err printf
#define pr_warn printf


typedef struct {
	unsigned long g2d_base;
} g2d_init_para;

typedef struct {
	g2d_init_para init_para;
} g2d_dev_t;

typedef enum {
	G2D_RGB2YUV_709,
	G2D_YUV2RGB_709,
	G2D_RGB2YUV_601,
	G2D_YUV2RGB_601,
	G2D_RGB2YUV_2020,
	G2D_YUV2RGB_2020,
} g2d_csc_sel;

typedef enum {
	VSU_FORMAT_YUV422 = 0x00,
	VSU_FORMAT_YUV420 = 0x01,
	VSU_FORMAT_YUV411 = 0x02,
	VSU_FORMAT_RGB = 0x03,
	VSU_FORMAT_BUTT = 0x04,
} vsu_pixel_format;

#define VSU_ZOOM0_SIZE	1
#define VSU_ZOOM1_SIZE	8
#define VSU_ZOOM2_SIZE	4
#define VSU_ZOOM3_SIZE	1
#define VSU_ZOOM4_SIZE	1
#define VSU_ZOOM5_SIZE	1

#define VSU_PHASE_NUM            32
#define VSU_PHASE_FRAC_BITWIDTH  19
#define VSU_PHASE_FRAC_REG_SHIFT 1
#define VSU_FB_FRAC_BITWIDTH     32

#define VI_LAYER_NUMBER 1
#define UI_LAYER_NUMBER 3
/*
int32_t g2d_bsp_open(void);
int32_t g2d_bsp_close(void);
int32_t g2d_bsp_reset(void);
int32_t mixer_irq_query(void);
int32_t rot_irq_query(void);
int32_t g2d_mixer_reset(void);
int32_t g2d_rot_reset(void);
int32_t g2d_bsp_bld(g2d_image_enh *, g2d_image_enh *, uint32_t, g2d_ck *);
int32_t g2d_fillrectangle(g2d_image_enh *dst, uint32_t color_value);
int32_t g2d_bsp_maskblt(g2d_image_enh *src, g2d_image_enh *ptn,
		      g2d_image_enh *mask, g2d_image_enh *dst,
		      uint32_t back_flag, uint32_t fore_flag);
int32_t g2d_bsp_bitblt(g2d_image_enh *src, g2d_image_enh *dst, uint32_t flag);
int32_t g2d_byte_cal(uint32_t format, uint32_t *ycnt, uint32_t *ucnt, uint32_t *vcnt);

extern int g2d_wait_cmd_finish(void);

uint32_t	mixer_reg_init(void);
int32_t	mixer_blt(g2d_blt *para, enum g2d_scan_order scan_order);
int32_t	mixer_fillrectangle(g2d_fillrect *para);
int32_t	mixer_stretchblt(g2d_stretchblt *para, enum g2d_scan_order scan_order);
int32_t	mixer_maskblt(g2d_maskblt *para);
uint32_t	mixer_set_palette(g2d_palette *para);
uint64_t	mixer_get_addr(uint32_t buffer_addr, uint32_t format,
			uint32_t stride, uint32_t x, uint32_t y);
uint32_t	mixer_set_reg_base(unsigned long addr);
uint32_t	mixer_get_irq(void);
uint32_t	mixer_get_irq0(void);
uint32_t	mixer_clear_init(void);
uint32_t	mixer_clear_init0(void);
int32_t	mixer_cmdq(uint32_t addr);
uint32_t	mixer_premultiply_set(uint32_t flag);
uint32_t	mixer_micro_block_set(g2d_blt *para);*/

/*
 * g2d_driver.h
 */
/* data format */
typedef enum {
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
/* mixer data format */
typedef enum {
	/* share data format */
	G2D_FMT_ARGB_AYUV8888	= (0x0),
	G2D_FMT_BGRA_VUYA8888	= (0x1),
	G2D_FMT_ABGR_AVUY8888	= (0x2),
	G2D_FMT_RGBA_YUVA8888	= (0x3),

	G2D_FMT_XRGB8888		= (0x4),
	G2D_FMT_BGRX8888		= (0x5),
	G2D_FMT_XBGR8888		= (0x6),
	G2D_FMT_RGBX8888		= (0x7),

	G2D_FMT_ARGB4444		= (0x8),
	G2D_FMT_ABGR4444		= (0x9),
	G2D_FMT_RGBA4444		= (0xA),
	G2D_FMT_BGRA4444		= (0xB),

	G2D_FMT_ARGB1555		= (0xC),
	G2D_FMT_ABGR1555		= (0xD),
	G2D_FMT_RGBA5551		= (0xE),
	G2D_FMT_BGRA5551		= (0xF),

	G2D_FMT_RGB565			= (0x10),
	G2D_FMT_BGR565			= (0x11),

	G2D_FMT_IYUV422			= (0x12),

	G2D_FMT_8BPP_MONO		= (0x13),
	G2D_FMT_4BPP_MONO		= (0x14),
	G2D_FMT_2BPP_MONO		= (0x15),
	G2D_FMT_1BPP_MONO		= (0x16),

	G2D_FMT_PYUV422UVC		= (0x17),
	G2D_FMT_PYUV420UVC		= (0x18),
	G2D_FMT_PYUV411UVC		= (0x19),

	/* just for output format */
	G2D_FMT_PYUV422			= (0x1A),
	G2D_FMT_PYUV420			= (0x1B),
	G2D_FMT_PYUV411			= (0x1C),

	/* just for input format */
	G2D_FMT_8BPP_PALETTE	= (0x1D),
	G2D_FMT_4BPP_PALETTE	= (0x1E),
	G2D_FMT_2BPP_PALETTE	= (0x1F),
	G2D_FMT_1BPP_PALETTE	= (0x20),

	G2D_FMT_PYUV422UVC_MB16	= (0x21),
	G2D_FMT_PYUV420UVC_MB16	= (0x22),
	G2D_FMT_PYUV411UVC_MB16	= (0x23),
	G2D_FMT_PYUV422UVC_MB32	= (0x24),
	G2D_FMT_PYUV420UVC_MB32	= (0x25),
	G2D_FMT_PYUV411UVC_MB32	= (0x26),
	G2D_FMT_PYUV422UVC_MB64	= (0x27),
	G2D_FMT_PYUV420UVC_MB64	= (0x28),
	G2D_FMT_PYUV411UVC_MB64	= (0x29),
	G2D_FMT_PYUV422UVC_MB128 = (0x2A),
	G2D_FMT_PYUV420UVC_MB128 = (0x2B),
	G2D_FMT_PYUV411UVC_MB128 = (0x2C),

} g2d_data_fmt;

/* pixel sequence in double word */
typedef enum {
	G2D_SEQ_NORMAL = 0x0,

	/* for interleaved yuv422 */
	G2D_SEQ_VYUY   = 0x1,			/* pixel 0�ڵ�16λ */
	G2D_SEQ_YVYU   = 0x2,			/* pixel 1�ڵ�16λ */

	/* for uv_combined yuv420 */
	G2D_SEQ_VUVU   = 0x3,

	/* for 16bpp rgb */
	G2D_SEQ_P10    = 0x4,			/* pixel 0�ڵ�16λ */
	G2D_SEQ_P01    = 0x5,			/* pixel 1�ڵ�16λ */

	/* planar format or 8bpp rgb */
	G2D_SEQ_P3210  = 0x6,			/* pixel 0�ڵ�8λ */
	G2D_SEQ_P0123  = 0x7,			/* pixel 3�ڵ�8λ */

	/* for 4bpp rgb */
	G2D_SEQ_P76543210  = 0x8,			/* 7,6,5,4,3,2,1,0 */
	G2D_SEQ_P67452301  = 0x9,			/* 6,7,4,5,2,3,0,1 */
	G2D_SEQ_P10325476  = 0xA,			/* 1,0,3,2,5,4,7,6 */
	G2D_SEQ_P01234567  = 0xB,			/* 0,1,2,3,4,5,6,7 */

	/* for 2bpp rgb */
	/* 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 */
	G2D_SEQ_2BPP_BIG_BIG       = 0xC,
	/* 12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3 */
	G2D_SEQ_2BPP_BIG_LITTER    = 0xD,
	/* 3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12 */
	G2D_SEQ_2BPP_LITTER_BIG    = 0xE,
	/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 */
	G2D_SEQ_2BPP_LITTER_LITTER = 0xF,

	/* for 1bpp rgb */
	/* 31,30,29,28,27,26,25,24,23,22,21,20,
	 * 19,18,17,16,15,14,13,12,11,10,9,8,7,
	 * 6,5,4,3,2,1,0
	 */
	G2D_SEQ_1BPP_BIG_BIG       = 0x10,
	/* 24,25,26,27,28,29,30,31,16,17,
	 * 18,19,20,21,22,23,8,9,10,11,12,
	 * 13,14,15,0,1,2,3,4,5,6,7
	 */
	G2D_SEQ_1BPP_BIG_LITTER    = 0x11,
	/* 7,6,5,4,3,2,1,0,15,14,13,12,11,
	 * 10,9,8,23,22,21,20,19,18,17,16,
	 * 31,30,29,28,27,26,25,24
	 */
	G2D_SEQ_1BPP_LITTER_BIG    = 0x12,
	/* 0,1,2,3,4,5,6,7,8,9,10,11,12,13,
	 * 14,15,16,17,18,19,20,21,22,23,24,
	 * 25,26,27,28,29,30,31
	 */
	G2D_SEQ_1BPP_LITTER_LITTER = 0x13,
} g2d_pixel_seq;

/*  */
typedef enum {
	G2D_BLT_NONE_H = 0x0,
	G2D_BLT_BLACKNESS,
	G2D_BLT_NOTMERGEPEN,
	G2D_BLT_MASKNOTPEN,
	G2D_BLT_NOTCOPYPEN,
	G2D_BLT_MASKPENNOT,
	G2D_BLT_NOT,
	G2D_BLT_XORPEN,
	G2D_BLT_NOTMASKPEN,
	G2D_BLT_MASKPEN,
	G2D_BLT_NOTXORPEN,
	G2D_BLT_NOP,
	G2D_BLT_MERGENOTPEN,
	G2D_BLT_COPYPEN,
	G2D_BLT_MERGEPENNOT,
	G2D_BLT_MERGEPEN,
	G2D_BLT_WHITENESS = 0x000000ff,

	G2D_ROT_90  = 0x00000100,
	G2D_ROT_180 = 0x00000200,
	G2D_ROT_270 = 0x00000300,
	G2D_ROT_0   = 0x00000400,
	G2D_ROT_H = 0x00001000,
	G2D_ROT_V = 0x00002000,

/*	G2D_SM_TDLR_1  =    0x10000000, */
	G2D_SM_DTLR_1 = 0x10000000,
/*	G2D_SM_TDRL_1  =    0x20000000, */
/*	G2D_SM_DTRL_1  =    0x30000000, */
} g2d_blt_flags_h;

/* ROP3 command */
typedef enum {
	G2D_ROP3_BLACKNESS = 0x00,
	G2D_ROP3_NOTSRCERASE = 0x11,
	G2D_ROP3_NOTSRCCOPY = 0x33,
	G2D_ROP3_SRCERASE = 0x44,
	G2D_ROP3_DSTINVERT = 0x55,
	G2D_ROP3_PATINVERT = 0x5A,
	G2D_ROP3_SRCINVERT = 0x66,
	G2D_ROP3_SRCAND = 0x88,
	G2D_ROP3_MERGEPAINT = 0xBB,
	G2D_ROP3_MERGECOPY = 0xC0,
	G2D_ROP3_SRCCOPY = 0xCC,
	G2D_ROP3_SRCPAINT = 0xEE,
	G2D_ROP3_PATCOPY = 0xF0,
	G2D_ROP3_PATPAINT = 0xFB,
	G2D_ROP3_WHITENESS = 0xFF,
} g2d_rop3_cmd_flag;

typedef enum {
	G2D_FIL_NONE			= 0x00000000,
	G2D_FIL_PIXEL_ALPHA		= 0x00000001,
	G2D_FIL_PLANE_ALPHA		= 0x00000002,
	G2D_FIL_MULTI_ALPHA		= 0x00000004,
} g2d_fillrect_flags;

typedef enum {
	G2D_BLT_NONE			= 0x00000000,
	G2D_BLT_PIXEL_ALPHA		= 0x00000001,
	G2D_BLT_PLANE_ALPHA		= 0x00000002,
	G2D_BLT_MULTI_ALPHA		= 0x00000004,
	G2D_BLT_SRC_COLORKEY	= 0x00000008,
	G2D_BLT_DST_COLORKEY	= 0x00000010,
	G2D_BLT_FLIP_HORIZONTAL	= 0x00000020,
	G2D_BLT_FLIP_VERTICAL	= 0x00000040,
	G2D_BLT_ROTATE90		= 0x00000080,
	G2D_BLT_ROTATE180		= 0x00000100,
	G2D_BLT_ROTATE270		= 0x00000200,
	G2D_BLT_MIRROR45		= 0x00000400,
	G2D_BLT_MIRROR135		= 0x00000800,
	G2D_BLT_SRC_PREMULTIPLY	= 0x00001000,
	G2D_BLT_DST_PREMULTIPLY	= 0x00002000,
} g2d_blt_flags;

/* BLD LAYER ALPHA MODE*/
typedef enum {
	G2D_PIXEL_ALPHA,
	G2D_GLOBAL_ALPHA,
	G2D_MIXER_ALPHA,
} g2d_alpha_mode_enh;

/* flip rectangle struct */
typedef struct {
	int32_t		x;		/* left top point coordinate x */
	int32_t		y;		/* left top point coordinate y */
	uint32_t		w;		/* rectangle width */
	uint32_t		h;		/* rectangle height */
} g2d_rect;

/* g2d color gamut */
typedef enum {
	G2D_BT601,
	G2D_BT709,
	G2D_BT2020,
} g2d_color_gmt;

/* image struct */
typedef struct {
	uint64_t		addr[3];/* base addr of image frame buffer in byte */
	uint32_t		w;	/* width of image frame buffer in pixel */
	uint32_t		h;	/* height of image frame buffer in pixel */
	g2d_data_fmt	format;	/* pixel format of image frame buffer */
	g2d_pixel_seq	pixel_seq;/* pixel sequence of image frame buffer */
} g2d_image;

enum color_range {
	COLOR_RANGE_0_255 = 0,
	COLOR_RANGE_16_235 = 1,
};

/* image struct */
typedef struct {
	int		 bbuff;
	uint32_t		 color;
	g2d_fmt_enh	 format;
	uint32_t		 laddr[3];
	uint32_t		 haddr[3];
	uint32_t		 width;
	uint32_t		 height;
	uint32_t		 align[3];

	g2d_rect	 clip_rect;

	g2d_color_gmt	 gamut;
	int		 bpremul;
	uint8_t		 alpha;
	g2d_alpha_mode_enh mode;
	int		 fd;
	uint32_t use_phy_addr;
	enum color_range color_range;
} g2d_image_enh;

/*
 * 0:Top to down, Left to right
 * 1:Top to down, Right to left
 * 2:Down to top, Left to right
 * 3:Down to top, Right to left
 */
enum g2d_scan_order {
	G2D_SM_TDLR = 0x00000000,
	G2D_SM_TDRL = 0x00000001,
	G2D_SM_DTLR = 0x00000002,
	G2D_SM_DTRL = 0x00000003,
};

typedef struct {
	g2d_fillrect_flags	 flag;
	g2d_image			 dst_image;
	g2d_rect			 dst_rect;

	uint32_t				 color;		/* fill color */
	uint32_t				 alpha;		/* plane alpha value */

} g2d_fillrect;

typedef struct {
	g2d_image_enh dst_image_h;
} g2d_fillrect_h;

typedef struct {
	g2d_blt_flags		flag;
	g2d_image		src_image;
	g2d_rect		src_rect;

	g2d_image		dst_image;
	/* left top point coordinate x of dst rect */
	int32_t			dst_x;
	/* left top point coordinate y of dst rect */
	int32_t			dst_y;

	uint32_t			color;		/* colorkey color */
	uint32_t			alpha;		/* plane alpha value */

} g2d_blt;

int g2d_blit(g2d_blt *para);

typedef struct {
	g2d_blt_flags_h flag_h;
	g2d_image_enh src_image_h;
	g2d_image_enh dst_image_h;
} g2d_blt_h;

typedef struct {
	g2d_blt_flags			 flag;
	g2d_image			 src_image;
	g2d_rect			 src_rect;

	g2d_image			 dst_image;
	g2d_rect			 dst_rect;

	uint32_t				 color;		/* colorkey color */
	uint32_t				 alpha;		/* plane alpha value */


} g2d_stretchblt;

typedef struct {
	g2d_rop3_cmd_flag back_flag;
	g2d_rop3_cmd_flag fore_flag;

	g2d_image_enh dst_image_h;
	g2d_image_enh src_image_h;
	g2d_image_enh ptn_image_h;
	g2d_image_enh mask_image_h;

} g2d_maskblt;

/* Porter Duff BLD command*/
typedef enum {
	G2D_BLD_CLEAR = 0x00000001,
	G2D_BLD_COPY = 0x00000002,
	G2D_BLD_DST = 0x00000003,
	G2D_BLD_SRCOVER = 0x00000004,
	G2D_BLD_DSTOVER = 0x00000005,
	G2D_BLD_SRCIN = 0x00000006,
	G2D_BLD_DSTIN = 0x00000007,
	G2D_BLD_SRCOUT = 0x00000008,
	G2D_BLD_DSTOUT = 0x00000009,
	G2D_BLD_SRCATOP = 0x0000000a,
	G2D_BLD_DSTATOP = 0x0000000b,
	G2D_BLD_XOR = 0x0000000c,
	G2D_CK_SRC = 0x00010000,
	G2D_CK_DST = 0x00020000,
} g2d_bld_cmd_flag;

typedef struct {
	uint32_t		*pbuffer;
	uint32_t		 size;

} g2d_palette;



typedef struct {
	long	start;
	long	end;
} g2d_cache_range;

/* CK PARA struct */
typedef struct {
	int match_rule;
/*	int match_rule; */
	uint32_t max_color;
	uint32_t min_color;
} g2d_ck;

typedef struct {
	g2d_bld_cmd_flag bld_cmd;
	g2d_image_enh dst_image_h;
	g2d_image_enh src_image_h;
	g2d_ck ck_para;
} g2d_bld;			/* blending enhance */

typedef enum {
	OP_FILLRECT = 0x1,
	OP_BITBLT = 0x2,
	OP_BLEND = 0x4,
	OP_MASK = 0x8,
	OP_SPLIT_MEM = 0x10,
} g2d_operation_flag;

/**
 * mixer_para
 */
struct mixer_para {
	g2d_operation_flag op_flag;
	g2d_blt_flags_h flag_h;
	g2d_rop3_cmd_flag back_flag;
	g2d_rop3_cmd_flag fore_flag;
	g2d_bld_cmd_flag bld_cmd;
	g2d_image_enh src_image_h;
	g2d_image_enh dst_image_h;
	g2d_image_enh ptn_image_h;
	g2d_image_enh mask_image_h;
	g2d_ck ck_para;
};

//#define SUNXI_G2D_IOC_MAGIC 'G'
//#define SUNXI_G2D_IO(nr)          _IO(SUNXI_G2D_IOC_MAGIC, nr)
//#define SUNXI_G2D_IOR(nr, size)   _IOR(SUNXI_G2D_IOC_MAGIC, nr, size)
//#define SUNXI_G2D_IOW(nr, size)   _IOW(SUNXI_G2D_IOC_MAGIC, nr, size)
//#define SUNXI_G2D_IOWR(nr, size)  _IOWR(SUNXI_G2D_IOC_MAGIC, nr, size)
//
//typedef enum {
//	G2D_CMD_BITBLT			=	0x50,
//	G2D_CMD_FILLRECT		=	0x51,
//	G2D_CMD_STRETCHBLT		=	0x52,
//	G2D_CMD_PALETTE_TBL		=	0x53,
//	G2D_CMD_QUEUE			=	0x54,
//	G2D_CMD_BITBLT_H		=	0x55,
//	G2D_CMD_FILLRECT_H		=	0x56,
//	G2D_CMD_BLD_H			=	0x57,
//	G2D_CMD_MASK_H			=	0x58,
//
//	G2D_CMD_MEM_REQUEST		=	0x59,
//	G2D_CMD_MEM_RELEASE		=	0x5A,
//	G2D_CMD_MEM_GETADR		=	0x5B,
//	G2D_CMD_MEM_SELIDX		=	0x5C,
//	G2D_CMD_MEM_FLUSH_CACHE		=	0x5D,
//	G2D_CMD_INVERTED_ORDER		=	0x5E,
//	G2D_CMD_MIXER_TASK = 0x5F,
//	G2D_CMD_CREATE_TASK = SUNXI_G2D_IOW(0x1, struct mixer_para),
//	G2D_CMD_TASK_APPLY = SUNXI_G2D_IOW(0x2, struct mixer_para),
//	G2D_CMD_TASK_DESTROY = SUNXI_G2D_IOW(0x3, unsigned int),
//	G2D_CMD_TASK_GET_PARA = SUNXI_G2D_IOR(0x4, struct mixer_para),
//
//} g2d_cmd;

/* Обеспечиваем работу тестов из linux
 *
 */

#define DIM_X 800
#define DIM_Y 480

//#include "src/display/display.h"
#define LCD_PIXEL_WIDTH		DIM_X
#define LCD_PIXEL_HEIGHT	DIM_Y
#define BYTE_PER_PIXEL		32

extern void * fb_address;
#if 1
// one buffer

	#define VIDEO_MEMORY0 ((uintptr_t) fb_address) //((uintptr_t) tfb1)
	#define VIDEO_MEMORY1 ((uintptr_t) fb_address) //((uintptr_t) tfb1)

#else

	#define VIDEO_MEMORY0 ((uintptr_t) tfb0)
	#define VIDEO_MEMORY1 ((uintptr_t) tfb1)

	extern ALIGNX_BEGIN PACKEDCOLORPIP_T tfb0 [GXSIZE(DIM_X, DIM_Y)] ALIGNX_END;
	extern ALIGNX_BEGIN PACKEDCOLORPIP_T tfb1 [GXSIZE(DIM_X, DIM_Y)] ALIGNX_END;

#endif

void g2d_main_layers_alpha(void);
void g2d_main0(void);

int g2d_fill(g2d_fillrect *para);
int g2d_blit(g2d_blt *para);
int g2d_stretchblit(g2d_stretchblt *para);

/*
 * g2d_regs_v2.h
 */
#define G2D_BASE 0x05410000

/* module base addr */
/*
 *#define G2D_TOP        (0x00000 + G2D_BASE)
 *#define G2D_MIXER      (0x00100 + G2D_BASE)
 *#define G2D_BLD        (0x00400 + G2D_BASE)
 *#define G2D_V0         (0x00800 + G2D_BASE)
 *#define G2D_UI0        (0x01000 + G2D_BASE)
 *#define G2D_UI1        (0x01800 + G2D_BASE)
 *#define G2D_UI2        (0x02000 + G2D_BASE)
 *#define G2D_WB         (0x03000 + G2D_BASE)
 *#define G2D_VSU        (0x08000 + G2D_BASE)
 *#define G2D_ROT        (0x28000 + G2D_BASE)
 *#define G2D_GSU        (0x30000 + G2D_BASE)
 */
#undef G2D_TOP
#undef G2D_MIXER
#undef G2D_BLD
#undef G2D_V0
#undef G2D_UI0
#undef G2D_UI1
#undef G2D_UI2
#undef G2D_WB
#undef G2D_VSU
#undef G2D_ROT
#undef G2D_GSU

#define G2D_TOP        (0x00000)
#define G2D_MIXER      (0x00100)
#define G2D_BLD        (0x00400)
#define G2D_V0         (0x00800)
#define G2D_UI0        (0x01000)
#define G2D_UI1        (0x01800)
#define G2D_UI2        (0x02000)
#define G2D_WB         (0x03000)
#define G2D_VSU        (0x08000)
#define G2D_ROT        (0x28000)
#define G2D_GSU        (0x30000)

/* register offset */
/* TOP register */
#define G2D_SCLK_GATE  (0x00 + G2D_TOP)
#define G2D_HCLK_GATE  (0x04 + G2D_TOP)
#define G2D_AHB_RESET  (0x08 + G2D_TOP)
#define G2D_SCLK_DIV   (0x0C + G2D_TOP)

/* MIXER GLB register */
#define G2D_MIXER_CTL  (0x00 + G2D_MIXER)
#define G2D_MIXER_INT  (0x04 + G2D_MIXER)
#define G2D_MIXER_CLK  (0x08 + G2D_MIXER)

/* LAY VIDEO register */
#define V0_ATTCTL      (0x00 + G2D_V0)
#define V0_MBSIZE      (0x04 + G2D_V0)
#define V0_COOR        (0x08 + G2D_V0)
#define V0_PITCH0      (0x0C + G2D_V0)
#define V0_PITCH1      (0x10 + G2D_V0)
#define V0_PITCH2      (0x14 + G2D_V0)
#define V0_LADD0       (0x18 + G2D_V0)
#define V0_LADD1       (0x1C + G2D_V0)
#define V0_LADD2       (0x20 + G2D_V0)
#define V0_FILLC       (0x24 + G2D_V0)
#define V0_HADD        (0x28 + G2D_V0)
#define V0_SIZE        (0x2C + G2D_V0)
#define V0_HDS_CTL0    (0x30 + G2D_V0)
#define V0_HDS_CTL1    (0x34 + G2D_V0)
#define V0_VDS_CTL0    (0x38 + G2D_V0)
#define V0_VDS_CTL1    (0x3C + G2D_V0)

/* LAY0 UI register */
#define UI0_ATTR       (0x00 + G2D_UI0)
#define UI0_MBSIZE     (0x04 + G2D_UI0)
#define UI0_COOR       (0x08 + G2D_UI0)
#define UI0_PITCH      (0x0C + G2D_UI0)
#define UI0_LADD       (0x10 + G2D_UI0)
#define UI0_FILLC      (0x14 + G2D_UI0)
#define UI0_HADD       (0x18 + G2D_UI0)
#define UI0_SIZE       (0x1C + G2D_UI0)

/* LAY1 UI register */
#define UI1_ATTR       (0x00 + G2D_UI1)
#define UI1_MBSIZE     (0x04 + G2D_UI1)
#define UI1_COOR       (0x08 + G2D_UI1)
#define UI1_PITCH      (0x0C + G2D_UI1)
#define UI1_LADD       (0x10 + G2D_UI1)
#define UI1_FILLC      (0x14 + G2D_UI1)
#define UI1_HADD       (0x18 + G2D_UI1)
#define UI1_SIZE       (0x1C + G2D_UI1)

/* LAY2 UI register */
#define UI2_ATTR       (0x00 + G2D_UI2)
#define UI2_MBSIZE     (0x04 + G2D_UI2)
#define UI2_COOR       (0x08 + G2D_UI2)
#define UI2_PITCH      (0x0C + G2D_UI2)
#define UI2_LADD       (0x10 + G2D_UI2)
#define UI2_FILLC      (0x14 + G2D_UI2)
#define UI2_HADD       (0x18 + G2D_UI2)
#define UI2_SIZE       (0x1C + G2D_UI2)

/* VSU register */
#define VS_CTRL           (0x000 + G2D_VSU)
#define VS_OUT_SIZE       (0x040 + G2D_VSU)
#define VS_GLB_ALPHA      (0x044 + G2D_VSU)
#define VS_Y_SIZE         (0x080 + G2D_VSU)
#define VS_Y_HSTEP        (0x088 + G2D_VSU)
#define VS_Y_VSTEP        (0x08C + G2D_VSU)
#define VS_Y_HPHASE       (0x090 + G2D_VSU)
#define VS_Y_VPHASE0      (0x098 + G2D_VSU)
#define VS_C_SIZE         (0x0C0 + G2D_VSU)
#define VS_C_HSTEP        (0x0C8 + G2D_VSU)
#define VS_C_VSTEP        (0x0CC + G2D_VSU)
#define VS_C_HPHASE       (0x0D0 + G2D_VSU)
#define VS_C_VPHASE0      (0x0D8 + G2D_VSU)
#define VS_Y_HCOEF0       (0x200 + G2D_VSU)
#define VS_Y_VCOEF0       (0x300 + G2D_VSU)
#define VS_C_HCOEF0       (0x400 + G2D_VSU)

/* BLD register */
#define BLD_EN_CTL         (0x000 + G2D_BLD)
#define BLD_FILLC0         (0x010 + G2D_BLD)
#define BLD_FILLC1         (0x014 + G2D_BLD)
#define BLD_CH_ISIZE0      (0x020 + G2D_BLD)
#define BLD_CH_ISIZE1      (0x024 + G2D_BLD)
#define BLD_CH_OFFSET0     (0x030 + G2D_BLD)
#define BLD_CH_OFFSET1     (0x034 + G2D_BLD)
#define BLD_PREMUL_CTL     (0x040 + G2D_BLD)
#define BLD_BK_COLOR       (0x044 + G2D_BLD)
#define BLD_SIZE           (0x048 + G2D_BLD)
#define BLD_CTL            (0x04C + G2D_BLD)
#define BLD_KEY_CTL        (0x050 + G2D_BLD)
#define BLD_KEY_CON        (0x054 + G2D_BLD)
#define BLD_KEY_MAX        (0x058 + G2D_BLD)
#define BLD_KEY_MIN        (0x05C + G2D_BLD)
#define BLD_OUT_COLOR      (0x060 + G2D_BLD)
#define ROP_CTL            (0x080 + G2D_BLD)
#define ROP_INDEX0         (0x084 + G2D_BLD)
#define ROP_INDEX1         (0x088 + G2D_BLD)
#define BLD_CSC_CTL        (0x100 + G2D_BLD)
#define BLD_CSC0_COEF00    (0x110 + G2D_BLD)
#define BLD_CSC0_COEF01    (0x114 + G2D_BLD)
#define BLD_CSC0_COEF02    (0x118 + G2D_BLD)
#define BLD_CSC0_CONST0    (0x11C + G2D_BLD)
#define BLD_CSC0_COEF10    (0x120 + G2D_BLD)
#define BLD_CSC0_COEF11    (0x124 + G2D_BLD)
#define BLD_CSC0_COEF12    (0x128 + G2D_BLD)
#define BLD_CSC0_CONST1    (0x12C + G2D_BLD)
#define BLD_CSC0_COEF20    (0x130 + G2D_BLD)
#define BLD_CSC0_COEF21    (0x134 + G2D_BLD)
#define BLD_CSC0_COEF22    (0x138 + G2D_BLD)
#define BLD_CSC0_CONST2    (0x13C + G2D_BLD)
#define BLD_CSC1_COEF00    (0x140 + G2D_BLD)
#define BLD_CSC1_COEF01    (0x144 + G2D_BLD)
#define BLD_CSC1_COEF02    (0x148 + G2D_BLD)
#define BLD_CSC1_CONST0    (0x14C + G2D_BLD)
#define BLD_CSC1_COEF10    (0x150 + G2D_BLD)
#define BLD_CSC1_COEF11    (0x154 + G2D_BLD)
#define BLD_CSC1_COEF12    (0x158 + G2D_BLD)
#define BLD_CSC1_CONST1    (0x15C + G2D_BLD)
#define BLD_CSC1_COEF20    (0x160 + G2D_BLD)
#define BLD_CSC1_COEF21    (0x164 + G2D_BLD)
#define BLD_CSC1_COEF22    (0x168 + G2D_BLD)
#define BLD_CSC1_CONST2    (0x16C + G2D_BLD)
#define BLD_CSC2_COEF00    (0x170 + G2D_BLD)
#define BLD_CSC2_COEF01    (0x174 + G2D_BLD)
#define BLD_CSC2_COEF02    (0x178 + G2D_BLD)
#define BLD_CSC2_CONST0    (0x17C + G2D_BLD)
#define BLD_CSC2_COEF10    (0x180 + G2D_BLD)
#define BLD_CSC2_COEF11    (0x184 + G2D_BLD)
#define BLD_CSC2_COEF12    (0x188 + G2D_BLD)
#define BLD_CSC2_CONST1    (0x18C + G2D_BLD)
#define BLD_CSC2_COEF20    (0x190 + G2D_BLD)
#define BLD_CSC2_COEF21    (0x194 + G2D_BLD)
#define BLD_CSC2_COEF22    (0x198 + G2D_BLD)
#define BLD_CSC2_CONST2    (0x19C + G2D_BLD)

/* WB register */
#define WB_ATT             (0x00 + G2D_WB)
#define WB_SIZE            (0x04 + G2D_WB)
#define WB_PITCH0          (0x08 + G2D_WB)
#define WB_PITCH1          (0x0C + G2D_WB)
#define WB_PITCH2          (0x10 + G2D_WB)
#define WB_LADD0           (0x14 + G2D_WB)
#define WB_HADD0           (0x18 + G2D_WB)
#define WB_LADD1           (0x1C + G2D_WB)
#define WB_HADD1           (0x20 + G2D_WB)
#define WB_LADD2           (0x24 + G2D_WB)
#define WB_HADD2           (0x28 + G2D_WB)

/* Rotate register */
#define ROT_CTL            (0x00 + G2D_ROT)
#define ROT_INT            (0x04 + G2D_ROT)
#define ROT_TIMEOUT        (0x08 + G2D_ROT)
#define ROT_IFMT           (0x20 + G2D_ROT)
#define ROT_ISIZE          (0x24 + G2D_ROT)
#define ROT_IPITCH0        (0x30 + G2D_ROT)
#define ROT_IPITCH1        (0x34 + G2D_ROT)
#define ROT_IPITCH2        (0x38 + G2D_ROT)
#define ROT_ILADD0         (0x40 + G2D_ROT)
#define ROT_IHADD0         (0x44 + G2D_ROT)
#define ROT_ILADD1         (0x48 + G2D_ROT)
#define ROT_IHADD1         (0x4C + G2D_ROT)
#define ROT_ILADD2         (0x50 + G2D_ROT)
#define ROT_IHADD2         (0x54 + G2D_ROT)
#define ROT_OSIZE          (0x84 + G2D_ROT)
#define ROT_OPITCH0        (0x90 + G2D_ROT)
#define ROT_OPITCH1        (0x94 + G2D_ROT)
#define ROT_OPITCH2        (0x98 + G2D_ROT)
#define ROT_OLADD0         (0xA0 + G2D_ROT)
#define ROT_OHADD0         (0xA4 + G2D_ROT)
#define ROT_OLADD1         (0xA8 + G2D_ROT)
#define ROT_OHADD1         (0xAC + G2D_ROT)
#define ROT_OLADD2         (0xB0 + G2D_ROT)
#define ROT_OHADD2         (0xB4 + G2D_ROT)

/* #define write_wvalue(addr, data) m_usbwordwrite32(  addr, data ) */
/* #define write_wvalue(addr, v) put_wvalue(addr, v) */
/* #define read_wvalue(addr) get_wvalue(addr) */

/* byte input */
#define get_bvalue(n)	(*((volatile uint8_t *)(n)))
/* byte output */
#define put_bvalue(n, c)	(*((volatile uint8_t *)(n)) = (c))
/* half word input */
#define get_hvalue(n)	(*((volatile uint16_t *)(n)))
/* half word output */
#define put_hvalue(n, c)	(*((volatile uint16_t *)(n)) = (c))
/* word input */
#define get_wvalue(n)	(*((volatile uint32_t *)(n)))
/* word output */
#define put_wvalue(n, c)	(*((volatile uint32_t *)(n)) = (c))

/*
 * g2d_bsp_c2.c
 */


static inline uint8_t __UADD8_Sat(uint8_t op1, uint8_t op2)
{
	  uint16_t result = (uint16_t) op1 + op2;
	  result |= -(result < op1);

	  return result;
}

static inline uint8_t __USUB8_Sat(uint8_t op1, uint8_t op2)
{
	  return op1 <= op2 ?  0 : (op1 - op2);
}



static inline uint32_t __UQADD8(uint32_t op1, uint32_t op2)
{
	  uint32_t result =
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 24), 0xFF & (op2 >> 24)) << 24) |
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 16), 0xFF & (op2 >> 16)) << 16) |
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 8), 0xFF & (op2 >> 8)) << 8) |
			  ((uint32_t) __UADD8_Sat(0xFF & (op1 >> 0), 0xFF & (op2 >> 0)) << 0);

	  return (result);
}

/**
 * Unsigned Saturating Subtract 8 performs four unsigned 8-bit integer subtractions,
 * saturates the results to the 8-bit unsigned integer
 * range 0 ≤ x ≤ 256 - 1, and writes the results to the destination register

 * @param op1
 * @param op2
 * @return
 */
static inline uint32_t __UQSUB8(uint32_t op1, uint32_t op2)
{
  uint32_t result =
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 24), 0xFF & (op2 >> 24)) << 24) |
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 16), 0xFF & (op2 >> 16)) << 16) |
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 8), 0xFF & (op2 >> 8)) << 8) |
		  ((uint32_t) __USUB8_Sat(0xFF & (op1 >> 0), 0xFF & (op2 >> 0)) << 0);

  return (result);
}




static const uintptr_t base_addr = G2D_TOP_BASE;
/* byte input */
#define read_bvalue(offset)		get_bvalue(base_addr + offset)
/* byte output */
#define write_bvalue(offset, value)	put_bvalue(base_addr + offset, value)
/* half word input */
#define read_hvalue(offset)		get_hvalue(base_addr + offset)
/* half word output */
#define write_hvalue(offset, value)	put_hvalue(base_addr + offset, value)
/* word input */
#define read_wvalue(offset)		get_wvalue(base_addr + offset)
/* word output */
#define write_wvalue(offset, value)	put_wvalue(base_addr + offset, value)

int32_t g2d_fc_set(uint32_t sel, uint32_t color_value);
int32_t g2d_format_trans(int32_t data_fmt, int32_t pixel_seq);
int32_t rgb2Ycbcr_709[12] = {
	0x0bb, 0x0275, 0x03f, 0x4200, 0xFFFFFF99, 0xFFFFFEA6, 0x01c2, 0x20200,
	0x01c2, 0xFFFFFE67, 0xFFFFFFD7, 0x20200, };
int32_t Ycbcr2rgb_709[12] = {
	0x04a8, 0x0, 0x072c, 0xFFFC1F7D, 0x04a8, 0xFFFFFF26, 0xFFFFFDDD,
	0x133F8, 0x04a8, 0x0876, 0, 0xFFFB7AA0, };

int32_t rgb2Ycbcr_601[12] = {
	0x0107, 0x0204, 0x064, 0x4200,
	0xFFFFFF68, 0xFFFFFED6, 0x01c2, 0x20200,
	0x01c2, 0xFFFFFE87, 0xFFFFFFB7, 0x20200,};
int32_t Ycbcr2rgb_601[12] = {
	0x04a8, 0x0, 0x0662, 0xFFFC865A,
	0x04a8, 0xFFFFFE70, 0xFFFFFCBF, 0x21FF4,
	0x04a8, 0x0812, 0x0, 0xFFFBAE4A,};

int32_t lan2coefftab32_full[512] = {
	0x00004000, 0x000140ff, 0x00033ffe, 0x00043ffd, 0x00063efc, 0xff083dfc,
	0x000a3bfb, 0xff0d39fb, 0xff0f37fb, 0xff1136fa, 0xfe1433fb,
	0xfe1631fb, 0xfd192ffb, 0xfd1c2cfb, 0xfd1f29fb, 0xfc2127fc,
	0xfc2424fc, 0xfc2721fc, 0xfb291ffd, 0xfb2c1cfd, 0xfb2f19fd,
	0xfb3116fe, 0xfb3314fe, 0xfa3611ff, 0xfb370fff, 0xfb390dff,
	0xfb3b0a00, 0xfc3d08ff, 0xfc3e0600, 0xfd3f0400, 0xfe3f0300,
	0xff400100,
	/* counter = 1 */
	0x00004000, 0x000140ff, 0x00033ffe, 0x00043ffd, 0x00063efc,
	0xff083dfc, 0x000a3bfb, 0xff0d39fb, 0xff0f37fb, 0xff1136fa,
	0xfe1433fb, 0xfe1631fb, 0xfd192ffb, 0xfd1c2cfb, 0xfd1f29fb,
	0xfc2127fc, 0xfc2424fc, 0xfc2721fc, 0xfb291ffd, 0xfb2c1cfd,
	0xfb2f19fd, 0xfb3116fe, 0xfb3314fe, 0xfa3611ff, 0xfb370fff,
	0xfb390dff, 0xfb3b0a00, 0xfc3d08ff, 0xfc3e0600, 0xfd3f0400,
	0xfe3f0300, 0xff400100,
	/* counter = 2 */
	0xff053804, 0xff063803, 0xff083801, 0xff093701, 0xff0a3700,
	0xff0c3500, 0xff0e34ff, 0xff1033fe, 0xff1232fd, 0xfe1431fd,
	0xfe162ffd, 0xfe182dfd, 0xfd1b2cfc, 0xfd1d2afc, 0xfd1f28fc,
	0xfd2126fc, 0xfd2323fd, 0xfc2621fd, 0xfc281ffd, 0xfc2a1dfd,
	0xfc2c1bfd, 0xfd2d18fe, 0xfd2f16fe, 0xfd3114fe, 0xfd3212ff,
	0xfe3310ff, 0xff340eff, 0x00350cff, 0x00360a00, 0x01360900,
	0x02370700, 0x03370600,
	/* counter = 3 */
	0xff083207, 0xff093206, 0xff0a3205, 0xff0c3203, 0xff0d3103,
	0xff0e3102, 0xfe113001, 0xfe132f00, 0xfe142e00, 0xfe162dff,
	0xfe182bff, 0xfe192aff, 0xfe1b29fe, 0xfe1d27fe, 0xfe1f25fe,
	0xfd2124fe, 0xfe2222fe, 0xfe2421fd, 0xfe251ffe, 0xfe271dfe,
	0xfe291bfe, 0xff2a19fe, 0xff2b18fe, 0xff2d16fe, 0x002e14fe,
	0x002f12ff, 0x013010ff, 0x02300fff, 0x03310dff, 0x04310cff,
	0x05310a00, 0x06310900,
	/* counter = 4 */
	0xff0a2e09, 0xff0b2e08, 0xff0c2e07, 0xff0e2d06, 0xff0f2d05,
	0xff102d04, 0xff122c03, 0xfe142c02, 0xfe152b02, 0xfe172a01,
	0xfe182901, 0xfe1a2800, 0xfe1b2700, 0xfe1d2500, 0xff1e24ff,
	0xfe2023ff, 0xff2121ff, 0xff2320fe, 0xff241eff, 0x00251dfe,
	0x00261bff, 0x00281afe, 0x012818ff, 0x012a16ff, 0x022a15ff,
	0x032b13ff, 0x032c12ff, 0x052c10ff, 0x052d0fff, 0x062d0d00,
	0x072d0c00, 0x082d0b00,
	/* counter = 5 */
	0xff0c2a0b, 0xff0d2a0a, 0xff0e2a09, 0xff0f2a08, 0xff102a07,
	0xff112a06, 0xff132905, 0xff142904, 0xff162803, 0xff172703,
	0xff182702, 0xff1a2601, 0xff1b2501, 0xff1c2401, 0xff1e2300,
	0xff1f2200, 0x00202000, 0x00211f00, 0x01221d00, 0x01231c00,
	0x01251bff, 0x02251aff, 0x032618ff, 0x032717ff, 0x042815ff,
	0x052814ff, 0x052913ff, 0x06291100, 0x072a10ff, 0x082a0e00,
	0x092a0d00, 0x0a2a0c00,
	/* counter = 6 */
	0xff0d280c, 0xff0e280b, 0xff0f280a, 0xff102809, 0xff112808,
	0xff122708, 0xff142706, 0xff152705, 0xff162605, 0xff172604,
	0xff192503, 0xff1a2403, 0x001b2302, 0x001c2202, 0x001d2201,
	0x001e2101, 0x011f1f01, 0x01211e00, 0x01221d00, 0x02221c00,
	0x02231b00, 0x03241900, 0x04241800, 0x04251700, 0x052616ff,
	0x06261400, 0x072713ff, 0x08271100, 0x08271100, 0x09271000,
	0x0a280e00, 0x0b280d00,
	/* counter = 7 */
	0xff0e260d, 0xff0f260c, 0xff10260b, 0xff11260a, 0xff122609,
	0xff132608, 0xff142508, 0xff152507, 0x00152506, 0x00172405,
	0x00182305, 0x00192304, 0x001b2203, 0x001c2103, 0x011d2002,
	0x011d2002, 0x011f1f01, 0x021f1e01, 0x02201d01, 0x03211c00,
	0x03221b00, 0x04221a00, 0x04231801, 0x05241700, 0x06241600,
	0x07241500, 0x08251300, 0x09251200, 0x09261100, 0x0a261000,
	0x0b260f00, 0x0c260e00,
	/* counter = 8 */
	0xff0e250e, 0xff0f250d, 0xff10250c, 0xff11250b, 0x0011250a,
	0x00132409, 0x00142408, 0x00152407, 0x00162307, 0x00172306,
	0x00182206, 0x00192205, 0x011a2104, 0x011b2004, 0x011c2003,
	0x021c1f03, 0x021e1e02, 0x031e1d02, 0x03201c01, 0x04201b01,
	0x04211a01, 0x05221900, 0x05221801, 0x06231700, 0x07231600,
	0x07241500, 0x08241400, 0x09241300, 0x0a241200, 0x0b241100,
	0x0c241000, 0x0d240f00,
	/* counter = 9 */
	0x000e240e, 0x000f240d, 0x0010240c, 0x0011240b, 0x0013230a,
	0x0013230a, 0x00142309, 0x00152308, 0x00162208, 0x00172207,
	0x01182106, 0x01192105, 0x011a2005, 0x021b1f04, 0x021b1f04,
	0x021d1e03, 0x031d1d03, 0x031e1d02, 0x041e1c02, 0x041f1b02,
	0x05201a01, 0x05211901, 0x06211801, 0x07221700, 0x07221601,
	0x08231500, 0x09231400, 0x0a231300, 0x0a231300, 0x0b231200,
	0x0c231100, 0x0d231000,
	/* counter = 10 */
	0x000f220f, 0x0010220e, 0x0011220d, 0x0012220c, 0x0013220b,
	0x0013220b, 0x0015210a, 0x0015210a, 0x01162108, 0x01172008,
	0x01182007, 0x02191f06, 0x02191f06, 0x021a1e06, 0x031a1e05,
	0x031c1d04, 0x041c1c04, 0x041d1c03, 0x051d1b03, 0x051e1a03,
	0x061f1902, 0x061f1902, 0x07201801, 0x08201701, 0x08211601,
	0x09211501, 0x0a211500, 0x0b211400, 0x0b221300, 0x0c221200,
	0x0d221100, 0x0e221000,
	/* counter = 11 */
	0x0010210f, 0x0011210e, 0x0011210e, 0x0012210d, 0x0013210c,
	0x0014200c, 0x0114200b, 0x0115200a, 0x01161f0a, 0x01171f09,
	0x02171f08, 0x02181e08, 0x03181e07, 0x031a1d06, 0x031a1d06,
	0x041b1c05, 0x041c1c04, 0x051c1b04, 0x051d1a04, 0x061d1a03,
	0x071d1903, 0x071e1803, 0x081e1802, 0x081f1702, 0x091f1602,
	0x0a201501, 0x0b1f1501, 0x0b201401, 0x0c211300, 0x0d211200,
	0x0e201200, 0x0e211100,
	/* counter = 12 */
	0x00102010, 0x0011200f, 0x0012200e, 0x0013200d, 0x0013200d,
	0x01141f0c, 0x01151f0b, 0x01151f0b, 0x01161f0a, 0x02171e09,
	0x02171e09, 0x03181d08, 0x03191d07, 0x03191d07, 0x041a1c06,
	0x041b1c05, 0x051b1b05, 0x051c1b04, 0x061c1a04, 0x071d1903,
	0x071d1903, 0x081d1803, 0x081e1703, 0x091e1702, 0x0a1f1601,
	0x0a1f1502, 0x0b1f1501, 0x0c1f1401, 0x0d201300, 0x0d201300,
	0x0e201200, 0x0f201100,
	/* counter = 13 */
	0x00102010, 0x0011200f, 0x00121f0f, 0x00131f0e, 0x00141f0d,
	0x01141f0c, 0x01141f0c, 0x01151e0c, 0x02161e0a, 0x02171e09,
	0x03171d09, 0x03181d08, 0x03181d08, 0x04191c07, 0x041a1c06,
	0x051a1b06, 0x051b1b05, 0x061b1a05, 0x061c1a04, 0x071c1904,
	0x081c1903, 0x081d1803, 0x091d1703, 0x091e1702, 0x0a1e1602,
	0x0b1e1502, 0x0c1e1501, 0x0c1f1401, 0x0d1f1400, 0x0e1f1300,
	0x0e1f1201, 0x0f1f1200,
	/* counter = 14 */
	0x00111e11, 0x00121e10, 0x00131e0f, 0x00131e0f, 0x01131e0e,
	0x01141d0e, 0x02151d0c, 0x02151d0c, 0x02161d0b, 0x03161c0b,
	0x03171c0a, 0x04171c09, 0x04181b09, 0x05181b08, 0x05191b07,
	0x06191a07, 0x061a1a06, 0x071a1906, 0x071b1905, 0x081b1805,
	0x091b1804, 0x091c1704, 0x0a1c1703, 0x0a1c1604, 0x0b1d1602,
	0x0c1d1502, 0x0c1d1502, 0x0d1d1402, 0x0e1d1401, 0x0e1e1301,
	0x0f1e1300, 0x101e1200,
	/* counter = 15 */
	0x00111e11, 0x00121e10, 0x00131d10, 0x01131d0f, 0x01141d0e,
	0x01141d0e, 0x02151c0d, 0x02151c0d, 0x03161c0b, 0x03161c0b,
	0x04171b0a, 0x04171b0a, 0x05171b09, 0x05181a09, 0x06181a08,
	0x06191a07, 0x07191907, 0x071a1906, 0x081a1806, 0x081a1806,
	0x091a1805, 0x0a1b1704, 0x0a1b1704, 0x0b1c1603, 0x0b1c1603,
	0x0c1c1503, 0x0d1c1502, 0x0d1d1402, 0x0e1d1401, 0x0f1d1301,
	0x0f1d1301, 0x101e1200,
	/* counter = 16 */
};

int32_t linearcoefftab32[32] = {
	0x00004000, 0x00023e00, 0x00043c00, 0x00063a00, 0x00083800,
	0x000a3600, 0x000c3400, 0x000e3200, 0x00103000, 0x00122e00,
	0x00142c00, 0x00162a00, 0x00182800, 0x001a2600, 0x001c2400,
	0x001e2200, 0x00202000, 0x00221e00, 0x00241c00, 0x00261a00,
	0x00281800, 0x002a1600, 0x002c1400, 0x002e1200, 0x00301000,
	0x00320e00, 0x00340c00, 0x00360a00, 0x00380800, 0x003a0600,
	0x003c0400, 0x003e0200, };
int32_t g2d_bsp_open(void)
{
	write_wvalue(G2D_SCLK_GATE, 0x3);
	write_wvalue(G2D_HCLK_GATE, 0x3);
	write_wvalue(G2D_AHB_RESET, 0x3);
	return 0;
}

int32_t g2d_bsp_close(void)
{
	write_wvalue(G2D_AHB_RESET, 0x0);
	write_wvalue(G2D_HCLK_GATE, 0x0);
	write_wvalue(G2D_SCLK_GATE, 0x0);
	return 0;
}

int32_t g2d_bsp_reset(void)
{
	write_wvalue(G2D_AHB_RESET, 0x0);
	write_wvalue(G2D_AHB_RESET, 0x3);
	return 0;
}

int32_t g2d_mixer_reset(void)
{
	uint32_t reg_val;

	reg_val = read_wvalue(G2D_AHB_RESET);
	write_wvalue(G2D_AHB_RESET, reg_val & 0xfffffffe);
	write_wvalue(G2D_AHB_RESET, reg_val & 0xffffffff);
	return 0;
}

int32_t g2d_rot_reset(void)
{
	uint32_t reg_val;

	reg_val = read_wvalue(G2D_AHB_RESET);
	write_wvalue(G2D_AHB_RESET, reg_val & 0xfffffffd);
	write_wvalue(G2D_AHB_RESET, reg_val & 0xffffffff);
	return 0;
}

int32_t g2d_scan_order_fun(uint32_t scan_order)
{
	uint32_t tmp;

	tmp = read_wvalue(G2D_MIXER_CTL);
	tmp |= ((scan_order >> 24) & 0xf0);
	write_wvalue(G2D_MIXER_CTL, tmp);
	return 0;
}


/**
 * G2D IRQ query funct
 * if the mission finish IRQ flag was set to 1, then clear the flag
 * and return 1
 * if the IRQ was set to 0, then return 0
 */
int32_t mixer_irq_query(void)
{
	uint32_t tmp;

	tmp = read_wvalue(G2D_MIXER_INT);
	if (tmp & 0x1) {
		write_wvalue(G2D_MIXER_INT, tmp);
		return 0;
	}
	return -1;
}


/**
 * G2D IRQ query funct
 * if the mission finish IRQ flag was set to 1, then clear the flag
 * and return 1
 * if the IRQ was set to 0, then return 0
 */
int32_t rot_irq_query(void)
{
	uint32_t tmp;

	tmp = read_wvalue(ROT_INT);
	if (tmp & 0x1) {
		write_wvalue(ROT_INT, tmp);
		return 0;
	}
	return -1;
}

int32_t mixer_irq_enable(void)
{
	write_wvalue(G2D_MIXER_INT, 0x10);
	return 0;
}

int32_t rot_irq_enable(void)
{
	write_wvalue(ROT_INT, 0x10000);
	return 0;
}

int32_t g2d_irq_disable(void)
{
	write_wvalue(G2D_MIXER_INT, 0x0);
	return 0;
}

int32_t rot_irq_disable(void)
{
	write_wvalue(ROT_INT, 0x0);
	return 0;
}

int32_t g2d_sclk_div(uint32_t div)
{
	uint32_t reg_val;

	reg_val = read_wvalue(G2D_SCLK_DIV);
	reg_val &= 0xfffffff0;
	reg_val |= (div & 0xf);
	write_wvalue(G2D_SCLK_DIV, reg_val);
	return 0;
}

int32_t rot_sclk_div(uint32_t div)
{
	uint32_t reg_val;

	reg_val = read_wvalue(G2D_SCLK_DIV);
	reg_val &= 0xffffff0f;
	reg_val |= (div & 0xf) << 4;
	write_wvalue(G2D_SCLK_DIV, reg_val);
	return 0;
}

int32_t porter_duff(uint32_t cmd)
{
	switch (cmd) {
	case G2D_BLD_CLEAR:
		write_wvalue(BLD_CTL, 0x00000000);
		break;
	case G2D_BLD_COPY:
		write_wvalue(BLD_CTL, 0x00010001);
		break;
	case G2D_BLD_DST:
		write_wvalue(BLD_CTL, 0x01000100);
		break;
	case G2D_BLD_SRCOVER:
		write_wvalue(BLD_CTL, 0x03010301);
		break;
	case G2D_BLD_DSTOVER:
		write_wvalue(BLD_CTL, 0x01030103);
		break;
	case G2D_BLD_SRCIN:
		write_wvalue(BLD_CTL, 0x00020002);
		break;
	case G2D_BLD_DSTIN:
		write_wvalue(BLD_CTL, 0x02000200);
		break;
	case G2D_BLD_SRCOUT:
		write_wvalue(BLD_CTL, 0x00030003);
		break;
	case G2D_BLD_DSTOUT:
		write_wvalue(BLD_CTL, 0x03000300);
		break;
	case G2D_BLD_SRCATOP:
		write_wvalue(BLD_CTL, 0x03020302);
		break;
	case G2D_BLD_DSTATOP:
		write_wvalue(BLD_CTL, 0x02030203);
		break;
	case G2D_BLD_XOR:
		write_wvalue(BLD_CTL, 0x03030303);
		break;
	default:
		write_wvalue(BLD_CTL, 0x03010301);
		}
	return 0;
}


/**
 * @csc_no: CSC ID, G2D support three CSC,
 * -1 will return to indicate inappropriate CSC number.
 * @csc_sel: CSC format, G2D support the ITU-R 601. ITU-R 709. standard trans-
 *  form between RGB and YUV colorspace.
 */
int32_t g2d_csc_reg_set(uint32_t csc_no, g2d_csc_sel csc_sel)
{
	uint32_t i;
	uint32_t csc_base_addr;
	uint32_t tmp;

	switch (csc_no) {
	case 0:
		csc_base_addr = G2D_BLD + 0x110;
		tmp = read_wvalue(BLD_CSC_CTL);
		tmp |= 0x1;
		write_wvalue(BLD_CSC_CTL, tmp);
		break;
	case 1:
		csc_base_addr = G2D_BLD + 0x140;
		tmp = read_wvalue(BLD_CSC_CTL);
		tmp |= 0x1 << 1;
		write_wvalue(BLD_CSC_CTL, tmp);
		break;
	case 2:
		csc_base_addr = G2D_BLD + 0x170;
		tmp = read_wvalue(BLD_CSC_CTL);
		tmp |= 0x1 << 2;
		write_wvalue(BLD_CSC_CTL, tmp);
		break;
	default:

/* __wrn("sel wrong csc no.\n"); */
		    return -1;
	}
	switch (csc_sel) {
	case G2D_RGB2YUV_709:
		for (i = 0; i < 12; i++)
			write_wvalue(csc_base_addr + (i << 2),
				      rgb2Ycbcr_709[i]);
		break;
	case G2D_YUV2RGB_709:
		for (i = 0; i < 12; i++)
			write_wvalue(csc_base_addr + (i << 2),
				      Ycbcr2rgb_709[i]);
		break;
	case G2D_RGB2YUV_601:
		for (i = 0; i < 12; i++)
			write_wvalue(csc_base_addr + (i << 2),
				      rgb2Ycbcr_601[i]);

/* write_wvalue(csc_base_addr + (i<<2), */
/* rgb2Ycbcr_601[i]); */
		    break;
	case G2D_YUV2RGB_601:
		for (i = 0; i < 12; i++)
			write_wvalue(csc_base_addr + (i << 2),
				      Ycbcr2rgb_601[i]);

/* write_wvalue(csc_base_addr + (i<<2), */
/* Ycbcr2rgb_601[i]); */
		    break;
	default:

/* __wrn("wrong csc standard\n"); */
		    return -2;
	}
	return 0;
}


/**
 * set colorkey para.
 */
int32_t ck_para_set(g2d_ck *para)
{
	uint32_t tmp = 0x0;

	if (para->match_rule)
		tmp = 0x7;	/* ������� - �� ��������� � �������� */
	write_wvalue(BLD_KEY_CON, tmp);
	write_wvalue(BLD_KEY_MAX, para->max_color & 0x00ffffff);
	write_wvalue(BLD_KEY_MIN, para->min_color & 0x00ffffff);
	return 0;
}


/**
 */
int32_t g2d_byte_cal(uint32_t format, uint32_t *ycnt, uint32_t *ucnt, uint32_t *vcnt)
{
	*ycnt = 0;
	*ucnt = 0;
	*vcnt = 0;
	if (format <= G2D_FORMAT_BGRX8888)
		*ycnt = 4;

	else if (format <= G2D_FORMAT_BGR888)
		*ycnt = 3;

	else if (format <= G2D_FORMAT_BGRA5551)
		*ycnt = 2;

	else if (format <= G2D_FORMAT_BGRA1010102)
		*ycnt = 4;

	else if (format <= 0x23) {
		*ycnt = 2;
	}

	else if (format <= 0x25) {
		*ycnt = 1;
		*ucnt = 2;
	}

	else if (format == 0x26) {
		*ycnt = 1;
		*ucnt = 1;
		*vcnt = 1;
	}

	else if (format <= 0x29) {
		*ycnt = 1;
		*ucnt = 2;
	}

	else if (format == 0x2a) {
		*ycnt = 1;
		*ucnt = 1;
		*vcnt = 1;
	}

	else if (format <= 0x2d) {
		*ycnt = 1;
		*ucnt = 2;
	}

	else if (format == 0x2e) {
		*ycnt = 1;
		*ucnt = 1;
		*vcnt = 1;
	}

	else if (format == 0x30)
		*ycnt = 1;

	else if (format <= 0x36) {
		*ycnt = 2;
		*ucnt = 4;
	}

	else if (format <= 0x39)
		*ycnt = 6;
	return 0;
}


/**
 */
uint32_t cal_align(uint32_t width, uint32_t align)
{
	switch (align) {
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


/**
 * @sel:layer no.
 */
int32_t g2d_vlayer_set(uint32_t sel, g2d_image_enh *image)
{
	unsigned long long addr0, addr1, addr2;
	uint32_t tmp;
	uint32_t ycnt, ucnt, vcnt;
	uint32_t pitch0, pitch1, pitch2;
	uint32_t /*ch, */cw, cy, cx;

	switch (sel) {
	case 0:

		    /* base_addr = G2D_V0; */
		    break;
	default:
		return -1;
	}
	tmp = ((image->alpha & 0xff) << 24);
	if (image->bpremul)
		tmp |= (0x1 << 17);
	tmp |= (image->format << 8);
	tmp |= (image->mode << 1);
	tmp |= 1;
	write_wvalue(V0_ATTCTL, tmp);
	tmp =
	    (((image->clip_rect.h ==
	       0 ? 0 : image->clip_rect.h -
	       1) & 0x1fff) << 16) | ((image->clip_rect.w ==
					0 ? 0 : image->clip_rect.w -
					1) & 0x1fff);
	write_wvalue(V0_MBSIZE, tmp);

	    /* offset is set to 0, ovl size is set to layer size */
	    write_wvalue(V0_SIZE, tmp);
	write_wvalue(V0_COOR, 0);
	if ((image->format >= G2D_FORMAT_YUV422UVC_V1U1V0U0)
	      && (image->format <= G2D_FORMAT_YUV422_PLANAR)) {
		cw = image->width >> 1;
		//ch = image->height;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y;
	}

	else if ((image->format >= G2D_FORMAT_YUV420UVC_V1U1V0U0)
		 && (image->format <= G2D_FORMAT_YUV420_PLANAR)) {
		cw = image->width >> 1;
		//ch = image->height >> 1;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y >> 1;
	}

	else if ((image->format >= G2D_FORMAT_YUV411UVC_V1U1V0U0)
		 && (image->format <= G2D_FORMAT_YUV411_PLANAR)) {
		cw = image->width >> 2;
		//ch = image->height;
		cx = image->clip_rect.x >> 2;
		cy = image->clip_rect.y;
	}

	else {
		cw = 0;
		//ch = 0;
		cx = 0;
		cy = 0;
	}
	g2d_byte_cal(image->format, &ycnt, &ucnt, &vcnt);
	pitch0 = cal_align(ycnt * image->width, image->align[0]);
	write_wvalue(V0_PITCH0, pitch0);
	pitch1 = cal_align(ucnt * cw, image->align[1]);
	write_wvalue(V0_PITCH1, pitch1);
	pitch2 = cal_align(vcnt * cw, image->align[2]);
	write_wvalue(V0_PITCH2, pitch2);
	G2D_INFO_MSG("VInPITCH: %d, %d, %d\n",
				pitch0, pitch1, pitch2);
	G2D_INFO_MSG("VInAddrB: 0x%x, 0x%x, 0x%x\n",
			image->laddr[0], image->laddr[1], image->laddr[2]);
	addr0 =
	    image->laddr[0] + ((uint64_t) image->haddr[0] << 32) +
	    pitch0 * image->clip_rect.y + ycnt * image->clip_rect.x;
	write_wvalue(V0_LADD0, addr0 & 0xffffffff);
	addr1 =
	    image->laddr[1] + ((uint64_t) image->haddr[1] << 32) + pitch1 * cy +
	    ucnt * cx;
	write_wvalue(V0_LADD1, addr1 & 0xffffffff);
	addr2 =
	    image->laddr[2] + ((uint64_t) image->haddr[2] << 32) + pitch2 * cy +
	    vcnt * cx;
	write_wvalue(V0_LADD2, addr2 & 0xffffffff);
	tmp = ((addr0 >> 32) & 0xff) | ((addr1 >> 32) & 0xff) << 8 |
	    ((addr2 >> 32) & 0xff) << 16;
	write_wvalue(V0_HADD, tmp);
	G2D_INFO_MSG("VInAddrA: 0x%llx, 0x%llx, 0x%llx\n",
							addr0, addr1, addr2);
	if (image->bbuff == 0)
		g2d_fc_set((sel + VI_LAYER_NUMBER), image->color);
	return 0;
}

int32_t g2d_uilayer_set(uint32_t sel, g2d_image_enh *img)
{
	uint64_t addr0;
	uint32_t base_addr_u, tmp;
	uint32_t ycnt, ucnt, vcnt;
	uint32_t pitch0;

	switch (sel) {
	case 0:
		base_addr_u = G2D_UI0;
		break;
	case 1:
		base_addr_u = G2D_UI1;
		break;
	case 2:
		base_addr_u = G2D_UI2;
		break;
	default:
		return -1;
	}
	tmp = (img->alpha & 0xff) << 24;
	if (img->bpremul)
		tmp |= 0x1 << 17;
	tmp |= img->format << 8;
	tmp |= img->mode << 1;
	tmp |= 1;
	write_wvalue(base_addr_u, tmp);
	tmp =
	    (((img->clip_rect.h ==
	       0 ? 0 : img->clip_rect.h -
	       1) & 0x1fff) << 16) | ((img->clip_rect.w ==
					0 ? 0 : img->clip_rect.w - 1) & 0x1fff);
	write_wvalue(base_addr_u + 0x4, tmp);
	write_wvalue(base_addr_u + 0x1C, tmp);
	write_wvalue(base_addr_u + 0x8, 0);
	g2d_byte_cal(img->format, &ycnt, &ucnt, &vcnt);
	pitch0 = cal_align(ycnt * img->width, img->align[0]);
	write_wvalue(base_addr_u + 0xC, pitch0);
	addr0 =
	    img->laddr[0] + ((uint64_t) img->haddr[0] << 32) +
	    pitch0 * img->clip_rect.y + ycnt * img->clip_rect.x;
	write_wvalue(base_addr_u + 0x10, addr0 & 0xffffffff);
	write_wvalue(base_addr_u + 0x18, (addr0 >> 32) & 0xff);
	if (img->bbuff == 0)
		g2d_fc_set((sel + VI_LAYER_NUMBER), img->color);
	return 0;
}

int32_t g2d_wb_set(g2d_image_enh *image)
{
	uint64_t addr0, addr1, addr2;
	uint32_t tmp;
	uint32_t ycnt, ucnt, vcnt;
	uint32_t pitch0, pitch1, pitch2;
	uint32_t /*ch, */cw, cy, cx;

	write_wvalue(WB_ATT, image->format);
	tmp =
	    (((image->clip_rect.h ==
	       0 ? 0 : image->clip_rect.h -
	       1) & 0x1fff) << 16) | ((image->clip_rect.w ==
					0 ? 0 : image->clip_rect.w -
					1) & 0x1fff);
	write_wvalue(WB_SIZE, tmp);
	/*write to the bld out reg */
	G2D_INFO_MSG("BLD_CH_OSIZE W:  0x%x\n", image->clip_rect.w);
	G2D_INFO_MSG("BLD_CH_OSIZE H:  0x%x\n", image->clip_rect.h);
	write_wvalue(BLD_SIZE, tmp);
	/* set outdata premul */
	tmp = read_wvalue(BLD_OUT_COLOR);

	if (image->bpremul)
		write_wvalue(BLD_OUT_COLOR, tmp | 0x1);

	else
		write_wvalue(BLD_OUT_COLOR, tmp & 0x2);
	if ((image->format >= G2D_FORMAT_YUV422UVC_V1U1V0U0)
	      && (image->format <= G2D_FORMAT_YUV422_PLANAR)) {
		cw = image->width >> 1;
		//ch = image->height;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y;
	}

	else if ((image->format >= G2D_FORMAT_YUV420UVC_V1U1V0U0)
		 && (image->format <= G2D_FORMAT_YUV420_PLANAR)) {
		cw = image->width >> 1;
		//ch = image->height >> 1;
		cx = image->clip_rect.x >> 1;
		cy = image->clip_rect.y >> 1;
	}

	else if ((image->format >= G2D_FORMAT_YUV411UVC_V1U1V0U0)
		 && (image->format <= G2D_FORMAT_YUV411_PLANAR)) {
		cw = image->width >> 2;
		//ch = image->height;
		cx = image->clip_rect.x >> 2;
		cy = image->clip_rect.y;
	}

	else {
		cw = 0;
		//ch = 0;
		cx = 0;
		cy = 0;
	}
	g2d_byte_cal(image->format, &ycnt, &ucnt, &vcnt);
	pitch0 = cal_align(ycnt * image->width, image->align[0]);
	write_wvalue(WB_PITCH0, pitch0);
	pitch1 = cal_align(ucnt * cw, image->align[1]);
	write_wvalue(WB_PITCH1, pitch1);
	pitch2 = cal_align(vcnt * cw, image->align[2]);
	write_wvalue(WB_PITCH2, pitch2);
	G2D_INFO_MSG("OutputPitch: %d, %d, %d\n", pitch0, pitch1, pitch2);

	addr0 =
	    image->laddr[0] + ((uint64_t) image->haddr[0] << 32) +
	    pitch0 * image->clip_rect.y + ycnt * image->clip_rect.x;
	write_wvalue(WB_LADD0, addr0 & 0xffffffff);
	write_wvalue(WB_HADD0, (addr0 >> 32) & 0xff);
	addr1 =
	    image->laddr[1] + ((uint64_t) image->haddr[1] << 32) + pitch1 * cy +
	    ucnt * cx;
	write_wvalue(WB_LADD1, addr1 & 0xffffffff);
	write_wvalue(WB_HADD1, (addr1 >> 32) & 0xff);
	addr2 =
	    image->laddr[2] + ((uint64_t) image->haddr[2] << 32) + pitch2 * cy +
	    vcnt * cx;
	write_wvalue(WB_LADD2, addr2 & 0xffffffff);
	write_wvalue(WB_HADD2, (addr2 >> 32) & 0xff);
	G2D_INFO_MSG("WbAddr: 0x%llx, 0x%llx, 0x%llx\n", addr0, addr1, addr2);
	return 0;
}


/**
 * fillcolor set
 * @sel:layer_no, 0--Layer Video,1--Layer UI0,2--Layer UI1,3--Layer UI2
 * @color_value:fill color value
 */
int32_t g2d_fc_set(uint32_t sel, uint32_t color_value)
{
	uint32_t tmp;

	G2D_INFO_MSG("FILLCOLOR: sel: %d, color: 0x%x\n", sel, color_value);

	if (sel == 0) {
		/* Layer Video */
		tmp = read_wvalue(V0_ATTCTL); //����� ��������
		tmp |= (0x1 << 4);
		write_wvalue(V0_ATTCTL, tmp);
		write_wvalue(V0_FILLC, color_value);
	}
	if (sel == 1) {
		/* Layer UI0 */
		tmp = read_wvalue(UI0_ATTR);
		tmp |= (0x1 << 4);
		write_wvalue(UI0_ATTR, tmp);
		write_wvalue(UI0_FILLC, color_value);
	}
	if (sel == 2) {
		/* Layer UI1 */
		tmp = read_wvalue(UI1_ATTR);
		tmp |= (0x1 << 4);
		write_wvalue(UI1_ATTR, tmp);
		write_wvalue(UI1_FILLC, color_value);
	}
	if (sel == 3) {
		/* Layer UI2 */
		tmp = read_wvalue(UI2_ATTR);
		tmp |= (0x1 << 4);
		write_wvalue(UI2_ATTR, tmp);
		write_wvalue(UI2_FILLC, color_value);
	}
	return 0;
}


/**
 * ROP2 cmd register set
 * Index0 is selected
 * dst mapping ch0'
 * src mapping ch1'
 */
int32_t g2d_rop2_set(uint32_t rop_cmd)
{
	if (rop_cmd == G2D_BLT_BLACKNESS) {
		/* blackness */
		/* tmpue = 0x1<<18; */
		write_wvalue(ROP_INDEX0, 0x40000);
	} else if (rop_cmd == G2D_BLT_NOTMERGEPEN) {
		/* ~(dst | src) */
		/* tmpue = (0x1<<6) | (0x1<<10) | (0x2<<11) | (0x1<<18); */
		write_wvalue(ROP_INDEX0, 0x41440);
	} else if (rop_cmd == G2D_BLT_MASKNOTPEN) {
		/* ~src&dst */
		/* tmpue = (0x1<<4) | (0x0<<10) | (0x2<<11) | (0x1<<18); */
		write_wvalue(ROP_INDEX0, 0x41010);
	} else if (rop_cmd == G2D_BLT_NOTCOPYPEN) {
		/* ~src */
		/* tmpue = (0x1<<4) | (0x2<<6) | (0x2<<11) |
		 * (0x1<<18) | (0x1<<17);
		 */
		write_wvalue(ROP_INDEX0, 0x61090);
	} else if (rop_cmd == G2D_BLT_MASKPENNOT) {
		/* src&~dst */
		/* tmpue = (0x1<<3) | (0x0<<10) | (0x2<<11) | (0x1<<18); */
		write_wvalue(ROP_INDEX0, 0x41008);
	} else if (rop_cmd == G2D_BLT_NOT) {
		/* ~dst */
		/* tmpue = (0x1<<3) | (0x2<<6) | (0x2<<11) |
		 * (0x1<<18) | (0x1<<16);
		 */
		write_wvalue(ROP_INDEX0, 0x51088);
	} else if (rop_cmd == G2D_BLT_XORPEN) {
		/* src xor dst */
		/* tmpue = (0x2<<6) | (0x2<<11) | (0x1<<18); */
		write_wvalue(ROP_INDEX0, 0x41080);
	} else if (rop_cmd == G2D_BLT_NOTMASKPEN) {
		/* ~(src & dst) */
		/* tmpue = (0x0<<6) | (0x1<<10) | (0x2<<11) | (0x1<<18); */
		write_wvalue(ROP_INDEX0, 0x41400);
	} else if (rop_cmd == G2D_BLT_MASKPEN) {
		/* src&dst */
		/* tmpue = (0x0<<6) | (0x2<<11) | (0x1<<18); */
		write_wvalue(ROP_INDEX0, 0x41000);
	} else if (rop_cmd == G2D_BLT_NOTXORPEN) {
		/* ~(src xor dst) */
		/* tmpue = (0x2<<6) | (0x1<<10) | (0x2<<11) | (0x1<<18); */
		write_wvalue(ROP_INDEX0, 0x41480);
	} else if (rop_cmd == G2D_BLT_NOP) {
		/* dst */
		/* tmpue = (0x2<<6) | (0x2<<11) | (0x1<<18) | (0x1<<16); */
		write_wvalue(ROP_INDEX0, 0x51080);
	} else if (rop_cmd == G2D_BLT_MERGENOTPEN) {
		/* ~dst or src */
		/* tmpue = (0x1<<3)| (0x1<<6) | (0x2<<11) | (0x1<<18) */
		/* write_wvalue(ROP_INDEX0, 0x40A20); */
		write_wvalue(ROP_INDEX0, 0x41048);
	} else if (rop_cmd == G2D_BLT_COPYPEN) {
		/* src */
		/* tmpue = (0x2<<6) | (0x2<<11) | (0x1<<18) | (0x1<<17); */
		write_wvalue(ROP_INDEX0, 0x61080);
	} else if (rop_cmd == G2D_BLT_MERGEPENNOT) {
		/* src or ~dst */
		/* tmpue =  (0x1<<3)| (0x1<<6) | (0x2<<11) | (0x1<<18) */
		write_wvalue(ROP_INDEX0, 0x41048);
	} else if (rop_cmd == G2D_BLT_MERGEPEN) {
		/* src or dst */
		/* tmpue = (0x1<<6) | (0x1<<18) | (0x2<<11); */
		write_wvalue(ROP_INDEX0, 0x41040);
	} else if (rop_cmd == G2D_BLT_WHITENESS) {
		/* whiteness */
		/* tmpue = (0x1<<18) | (0x1<<15); */
		write_wvalue(ROP_INDEX0, 0x48000);
	} else
		return -1;
	return 0;
}


/**
 * ROP3 cmd register set
 * dst mapping ch0'
 * src mapping ch1'
 * ptn mapping ch2'
 * -1 return meaning that the operate is not supported by now
 */
int32_t g2d_rop3_set(uint32_t sel, uint32_t rop3_cmd)
{
	uint32_t addr;

	if (sel == 0)
		addr = ROP_INDEX0;
	else if (sel == 1)
		addr = ROP_INDEX1;

	else
		return -1;
	if (rop3_cmd == G2D_ROP3_BLACKNESS) {
		/* blackness */
		/* 0x1<<18; */
		write_wvalue(addr, 0x40000);
	} else if (rop3_cmd == G2D_ROP3_NOTSRCERASE) {
		/* (~src) AND (~dst) */
		/* (0x1<<3) | (0x1<<4) | (0x1<<18) | (0x2<<11); */
		write_wvalue(addr, 0x41018);
	} else if (rop3_cmd == G2D_ROP3_NOTSRCCOPY) {

		/* ~src */
		/* (0x1<<4) | (0x2<<6) | (0x2<<11) | (0x1<<18) | (0x1<<16); */
		write_wvalue(addr, 0x51090);
	} else if (rop3_cmd == G2D_ROP3_SRCERASE) {
		/* src AND ~dst */
		/* (0x1<<3) | (0x0<<6) | (0x2<<11) | (0x1<<18); */
		write_wvalue(addr, 0x41008);
	} else if (rop3_cmd == G2D_ROP3_DSTINVERT) {
		/* ~dst */
		/* (0x1<<3) | (0x2<<6) | (0x2<<11) | (0x1<<18) | (0x1<<17); */
		write_wvalue(addr, 0x61088);
	} else if (rop3_cmd == G2D_ROP3_PATINVERT) {
		/* ptn XOR dst */
		/* (0x2<<6) | (0x2<<11) | (0x1<<17) */
		write_wvalue(addr, 0x21080);
	} else if (rop3_cmd == G2D_ROP3_SRCINVERT) {
		/* src XOR dst */
		/* (0x2<<6) | (0x2<<11) | (0x1<<18); */
		write_wvalue(addr, 0x41080);
	} else if (rop3_cmd == G2D_ROP3_SRCAND) {
		/* src AND dst */
		/* (0x0<<6) | (0x2<<11) | (0x1<<18); */
		write_wvalue(addr, 0x41000);
	} else if (rop3_cmd == G2D_ROP3_MERGEPAINT) {
		/* ~src OR dst */
		/* (0x1<<4) | (0x1<<6) | (0x2<<11) | (0x1<<18); */
		write_wvalue(addr, 0x41050);
	} else if (rop3_cmd == G2D_ROP3_MERGECOPY) {
		/* src AND pattern */
		/* (0x2<<6) | (0x1<<16) */
		write_wvalue(addr, 0x10080);
	} else if (rop3_cmd == G2D_ROP3_SRCCOPY) {
		/* src */
		/* (0x2<<6) | (0x2<<11) | (0x1<<18) | (0x1<<16); */
		write_wvalue(addr, 0x51080);
	} else if (rop3_cmd == G2D_ROP3_SRCPAINT) {
		/* src OR dst */
		/* (0x1<<6) | (0x2<<11) | (0x1<<18); */
		write_wvalue(addr, 0x41040);
	} else if (rop3_cmd == G2D_ROP3_PATCOPY) {
		/* ptn */
		/* (0x1<<16) | (0x1<<17) | (0x2)<<11 */
		write_wvalue(addr, 0x31000);
	} else if (rop3_cmd == G2D_ROP3_PATPAINT) {
		/* DPSnoo */
		/* (0x1<<3) | (0x1<<6) | (0x1<<11) */
		write_wvalue(addr, 0x848);
	} else if (rop3_cmd == G2D_ROP3_WHITENESS) {
		/* whiteness */
		write_wvalue(addr, 0x48000);
	} else
		return -1;
	return 0;
}

/**
 * background color set
 */
int32_t g2d_bk_set(uint32_t color)
{
	write_wvalue(BLD_BK_COLOR, color & 0xffffffff);
	return 0;
}

/**
 * function       : g2d_vsu_calc_fir_coef(unsigned int step)
 * description    : set fir coefficients
 * parameters     :
 *                  step		<horizontal scale ratio of vsu>
 * return         :
 *                  offset (in word) of coefficient table
 */
static uint32_t g2d_vsu_calc_fir_coef(uint32_t step)
{
	uint32_t pt_coef;
	uint32_t scale_ratio, int_part, float_part, fir_coef_ofst;

	scale_ratio = step >> (VSU_PHASE_FRAC_BITWIDTH - 3);
	int_part = scale_ratio >> 3;
	float_part = scale_ratio & 0x7;
	fir_coef_ofst = (int_part == 0) ? VSU_ZOOM0_SIZE :
	    (int_part == 1) ? VSU_ZOOM0_SIZE + float_part :
	    (int_part ==
	     2) ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE +
	    (float_part >> 1) : (int_part ==
				  3) ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE +
	    VSU_ZOOM2_SIZE : (int_part ==
			       4) ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE +
	    VSU_ZOOM2_SIZE + VSU_ZOOM3_SIZE : VSU_ZOOM0_SIZE +
	    VSU_ZOOM1_SIZE + VSU_ZOOM2_SIZE + VSU_ZOOM3_SIZE + VSU_ZOOM4_SIZE;
	pt_coef = fir_coef_ofst * VSU_PHASE_NUM;
	return pt_coef;
}

int32_t g2d_rop_by_pass(uint32_t sel)
{
	if (sel == 0)
		write_wvalue(ROP_CTL, 0xF0);

	else if (sel == 1)
		write_wvalue(ROP_CTL, 0x55F0);

	else if (sel == 2)
		write_wvalue(ROP_CTL, 0xAAF0);

	else
		return -1;
	return 0;
}

int32_t g2d_vsu_para_set(uint32_t fmt, uint32_t in_w, uint32_t in_h, uint32_t out_w,
			   uint32_t out_h, uint8_t alpha)
{
	uint32_t i;
	uint64_t tmp, temp;
	uint32_t yhstep, yvstep;
	uint32_t incw, inch;
	uint32_t yhcoef_offset, yvcoef_offset, chcoef_offset;
	uint32_t format;

	if (fmt > G2D_FORMAT_IYUV422_Y1U0Y0V0)
		write_wvalue(VS_CTRL, 0x10101);

	else
		write_wvalue(VS_CTRL, 0x00000101);
	tmp = ((out_h - 1) << 16) | (out_w - 1);
	write_wvalue(VS_OUT_SIZE, tmp);
	write_wvalue(VS_GLB_ALPHA, alpha & 0xff);
	write_wvalue(VS_Y_SIZE, ((in_h - 1) << 16) | (in_w - 1));
	temp = in_w << VSU_PHASE_FRAC_BITWIDTH;
	if (out_w)
	    temp = temp/out_w;
	else
		temp = 0;
	yhstep = temp;
	write_wvalue(VS_Y_HSTEP, yhstep << 1);
	temp = in_h << VSU_PHASE_FRAC_BITWIDTH;
	if (out_h)
		temp = temp/ out_h;
	else
		temp = 0;
	yvstep = temp;
	write_wvalue(VS_Y_VSTEP, yvstep << 1);
	yhcoef_offset = g2d_vsu_calc_fir_coef(yhstep);
	for (i = 0; i < VSU_PHASE_NUM; i++) {
		write_wvalue(VS_Y_HCOEF0 + (i << 2),
			      lan2coefftab32_full[yhcoef_offset + i]);
	}
	yvcoef_offset = g2d_vsu_calc_fir_coef(yvstep);
	switch (fmt) {
	case G2D_FORMAT_IYUV422_V0Y1U0Y0:
	case G2D_FORMAT_IYUV422_Y1V0Y0U0:
	case G2D_FORMAT_IYUV422_U0Y1V0Y0:
	case G2D_FORMAT_IYUV422_Y1U0Y0V0:{
			incw = (in_w + 1) >> 1;
			inch = in_h;
			format = VSU_FORMAT_YUV422;
			write_wvalue(VS_C_SIZE,
				      ((inch - 1) << 16) | (incw - 1));

			    /* chstep = yhstep>>1 cvstep = yvstep */
			    write_wvalue(VS_C_HSTEP, yhstep);
			write_wvalue(VS_C_VSTEP, yvstep << 1);
			chcoef_offset = g2d_vsu_calc_fir_coef(yhstep >> 1);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_C_HCOEF0 + (i << 2),
					      lan2coefftab32_full[chcoef_offset
								  + i]);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_Y_VCOEF0 + (i << 2),
					      linearcoefftab32[i]);
			break;
		}
	case G2D_FORMAT_YUV422UVC_V1U1V0U0:
	case G2D_FORMAT_YUV422UVC_U1V1U0V0:
	case G2D_FORMAT_YUV422_PLANAR:{
			incw = (in_w + 1) >> 1;
			inch = in_h;
			format = VSU_FORMAT_YUV422;
			write_wvalue(VS_C_SIZE,
				      ((inch - 1) << 16) | (incw - 1));

			    /* chstep = yhstep>>1 cvstep = yvstep>>1 */
			    write_wvalue(VS_C_HSTEP, yhstep);
			write_wvalue(VS_C_VSTEP, yvstep << 1);
			chcoef_offset = g2d_vsu_calc_fir_coef(yhstep >> 1);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_C_HCOEF0 + (i << 2),
					      lan2coefftab32_full[chcoef_offset
								  + i]);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_Y_VCOEF0 + (i << 2),
					      lan2coefftab32_full[yvcoef_offset
								  + i]);
			break;
		}
	case G2D_FORMAT_YUV420_PLANAR:
	case G2D_FORMAT_YUV420UVC_V1U1V0U0:
	case G2D_FORMAT_YUV420UVC_U1V1U0V0:{
			incw = (in_w + 1) >> 1;
			inch = (in_h + 1) >> 1;
			format = VSU_FORMAT_YUV420;
			write_wvalue(VS_C_SIZE,
				      ((inch - 1) << 16) | (incw - 1));
			write_wvalue(VS_C_HSTEP, yhstep);
			write_wvalue(VS_C_VSTEP, yvstep);
			chcoef_offset = g2d_vsu_calc_fir_coef(yhstep >> 1);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_C_HCOEF0 + (i << 2),
					      lan2coefftab32_full[chcoef_offset
								  + i]);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_Y_VCOEF0 + (i << 2),
					      lan2coefftab32_full[yvcoef_offset
								  + i]);
			break;
		}
	case G2D_FORMAT_YUV411_PLANAR:
	case G2D_FORMAT_YUV411UVC_V1U1V0U0:
	case G2D_FORMAT_YUV411UVC_U1V1U0V0:{
			incw = (in_w + 3) >> 2;
			inch = in_h;
			format = VSU_FORMAT_YUV411;
			write_wvalue(VS_C_SIZE,
				      ((inch - 1) << 16) | (incw - 1));

			    /* chstep = yhstep>>2 cvstep = yvstep */
			    write_wvalue(VS_C_HSTEP, yhstep >> 1);
			write_wvalue(VS_C_VSTEP, yvstep << 1);
			chcoef_offset = g2d_vsu_calc_fir_coef(yhstep >> 2);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_C_HCOEF0 + (i << 2),
					      lan2coefftab32_full[chcoef_offset
								  + i]);
			for (i = 0; i < VSU_PHASE_NUM; i++)
				write_wvalue(VS_Y_VCOEF0 + (i << 2),
					      lan2coefftab32_full[yvcoef_offset
								  + i]);
			break;
		}
	default:
		format = VSU_FORMAT_RGB;
		incw = in_w;
		inch = in_h;
		write_wvalue(VS_C_SIZE, ((inch - 1) << 16) | (incw - 1));

		    /* chstep = yhstep cvstep = yvstep */
		    write_wvalue(VS_C_HSTEP, yhstep << 1);
		write_wvalue(VS_C_VSTEP, yvstep << 1);
		chcoef_offset = g2d_vsu_calc_fir_coef(yhstep);
		for (i = 0; i < VSU_PHASE_NUM; i++)
			write_wvalue(VS_C_HCOEF0 + (i << 2),
				      lan2coefftab32_full[chcoef_offset + i]);
//			write_wvalue(VS_C_HCOEF0 + (i << 2),
//				      linearcoefftab32[i]);

		for (i = 0; i < VSU_PHASE_NUM; i++)
//				write_wvalue(VS_Y_VCOEF0 + (i << 2),
//					      lan2coefftab32_full[yvcoef_offset + i]);
			write_wvalue(VS_Y_VCOEF0 + (i << 2),
				      linearcoefftab32[i]);
		break;
	}
	if (format == VSU_FORMAT_YUV420) {

		/**
		 * yhphase = 0;
		 * yvphase = 0;
		 * chphase = 0xFFFE0000;
		 * cvphase = 0xFFFE0000;
		 */
		write_wvalue(VS_Y_HPHASE, 0);
		write_wvalue(VS_Y_VPHASE0, 0);
		write_wvalue(VS_C_HPHASE, 0xFFFc0000);
		write_wvalue(VS_C_VPHASE0, 0xFFFc0000);
	}

	else {
		write_wvalue(VS_Y_HPHASE, 0);
		write_wvalue(VS_Y_VPHASE0, 0);
		write_wvalue(VS_C_HPHASE, 0);
		write_wvalue(VS_C_VPHASE0, 0);
	}
	if (fmt >= G2D_FORMAT_IYUV422_Y1U0Y0V0)
		write_wvalue(VS_CTRL, 0x10001);

	else
		write_wvalue(VS_CTRL, 0x00001);
	return 0;
}


int32_t g2d_calc_coarse(uint32_t format, uint32_t inw, uint32_t inh, uint32_t outw,
			  uint32_t outh, uint32_t *midw, uint32_t *midh)
{
	uint32_t tmp;

	switch (format) {
	case G2D_FORMAT_IYUV422_V0Y1U0Y0:
	case G2D_FORMAT_IYUV422_Y1V0Y0U0:
	case G2D_FORMAT_IYUV422_U0Y1V0Y0:
	case G2D_FORMAT_IYUV422_Y1U0Y0V0:{
		/* interleaved YUV422 format */
		*midw = inw;
		*midh = inh;
		break;
	}
	case G2D_FORMAT_YUV422UVC_V1U1V0U0:
	case G2D_FORMAT_YUV422UVC_U1V1U0V0:
	case G2D_FORMAT_YUV422_PLANAR:{
		if (inw >= (outw << 3)) {
			*midw = outw << 3;
			tmp = (*midw << 16) | inw;
			write_wvalue(V0_HDS_CTL0, tmp);
			tmp = (*midw << 15) | ((inw + 1) >> 1);
			write_wvalue(V0_HDS_CTL1, tmp);
		} else
			*midw = inw;
		if (inh >= (outh << 2)) {
			*midh = (outh << 2);
			tmp = (*midh << 16) | inh;
			write_wvalue(V0_VDS_CTL0, tmp);
			write_wvalue(V0_VDS_CTL1, tmp);
		} else
			*midh = inh;
		break;
	}
	case G2D_FORMAT_YUV420_PLANAR:
	case G2D_FORMAT_YUV420UVC_V1U1V0U0:
	case G2D_FORMAT_YUV420UVC_U1V1U0V0:{
		if (inw >= (outw << 3)) {
			*midw = outw << 3;
			tmp = (*midw << 16) | inw;
			write_wvalue(V0_HDS_CTL0, tmp);
			tmp = (*midw << 15) | ((inw + 1) >> 1);
			write_wvalue(V0_HDS_CTL0, tmp);
		} else
			*midw = inw;
		if (inh >= (outh << 2)) {
			*midh = (outh << 2);
			tmp = (*midh << 16) | inh;
			write_wvalue(V0_VDS_CTL0, tmp);
			tmp = (*midh << 15) | ((inh + 1) >> 1);
			write_wvalue(V0_VDS_CTL1, tmp);
		} else
			*midh = inh;
		break;
	}
	case G2D_FORMAT_YUV411_PLANAR:
	case G2D_FORMAT_YUV411UVC_V1U1V0U0:
	case G2D_FORMAT_YUV411UVC_U1V1U0V0:{
		if (inw >= (outw << 3)) {
			*midw = outw << 3;
			tmp = ((*midw) << 16) | inw;
			write_wvalue(V0_HDS_CTL0, tmp);
			tmp = ((*midw) << 14) | ((inw + 3) >> 2);
			write_wvalue(V0_HDS_CTL1, tmp);
		} else
			*midw = inw;
		if (inh >= (outh << 2)) {
			*midh = (outh << 2);
			tmp = ((*midh) << 16) | inh;
			write_wvalue(V0_VDS_CTL0, tmp);
			write_wvalue(V0_VDS_CTL1, tmp);
		} else
			*midh = inh;
		break;
	}
	default:
		if (inw >= (outw << 3)) {
			*midw = outw << 3;
			tmp = ((*midw) << 16) | inw;
			write_wvalue(V0_HDS_CTL0, tmp);
			write_wvalue(V0_HDS_CTL1, tmp);
		} else
			*midw = inw;
		if (inh >= (outh << 2)) {
			*midh = (outh << 2);
			tmp = ((*midh) << 16) | inh;
			write_wvalue(V0_VDS_CTL0, tmp);
			write_wvalue(V0_VDS_CTL1, tmp);
		} else
			*midh = inh;
		break;
	}
	return 0;
}


/*
 * sel: 0-->pipe0 1-->pipe1 other:error
 */
int32_t g2d_bldin_set(uint32_t sel, g2d_rect rect, int premul)
{
	uint32_t tmp;
	uint32_t offset;

	if (sel == 0) {
		offset = 0;
		tmp = read_wvalue(BLD_EN_CTL);
		tmp |= 0x1 << 8;
		write_wvalue(BLD_EN_CTL, tmp);
		if (premul) {
			tmp = read_wvalue(BLD_PREMUL_CTL);
			tmp |= 0x1;
			write_wvalue(BLD_PREMUL_CTL, tmp);
		}
	} else if (sel == 1) {
		offset = 0x4;
		tmp = read_wvalue(BLD_EN_CTL);
		tmp |= 0x1 << 9;
		write_wvalue(BLD_EN_CTL, tmp);
		if (premul) {
			tmp = read_wvalue(BLD_PREMUL_CTL);
			tmp |= 0x1 << 1;
			write_wvalue(BLD_PREMUL_CTL, tmp);
		}
	} else
		return -1;
	tmp = ((rect.h - 1) << 16) | (rect.w - 1);

	G2D_INFO_MSG("BLD_CH_ISIZE W:  0x%x\n", rect.w);
	G2D_INFO_MSG("BLD_CH_ISIZE H:  0x%x\n", rect.h);

	write_wvalue(BLD_CH_ISIZE0 + offset, tmp);
	tmp =
	    ((rect.y <= 0 ? 0 : rect.y - 1) << 16) | (rect.x <=
						      0 ? 0 : rect.x - 1);

	G2D_INFO_MSG("BLD_CH_ISIZE X:  0x%x\n", rect.x);
	G2D_INFO_MSG("BLD_CH_ISIZE Y:  0x%x\n", rect.y);

	write_wvalue(BLD_CH_OFFSET0 + offset, tmp);
	return 0;
}

/**
 * set the bld color space based on the format
 * if the format is UI, then set the bld in RGB color space
 * if the format is Video, then set the bld in YUV color space
 */
int32_t g2d_bld_cs_set(uint32_t format)
{
	uint32_t tmp;

	if (format <= G2D_FORMAT_BGRA1010102) {
		tmp = read_wvalue(BLD_OUT_COLOR);
		tmp &= 0xFFFFFFFD;
		write_wvalue(BLD_OUT_COLOR, tmp);
	} else if (format <= G2D_FORMAT_YUV411_PLANAR) {
		tmp = read_wvalue(BLD_OUT_COLOR);
		tmp |= 0x1 << 1;
		write_wvalue(BLD_OUT_COLOR, tmp);
	} else
		return -1;
	return 0;
}

int32_t mixer_fillrectangle(g2d_fillrect *para)
{
	g2d_image_enh src_tmp, dst_tmp;
	g2d_image_enh *src = &src_tmp;
	g2d_image_enh *dst = &dst_tmp;
	int32_t result;

	g2d_mixer_reset();
	if (para->flag == G2D_FIL_NONE) {
		pr_info("fc only!\n");
		dst->bbuff = 1;
		dst->color = para->color;
		dst->format =
			g2d_format_trans(para->dst_image.format,
					para->dst_image.pixel_seq);
		dst->laddr[0] = para->dst_image.addr[0];
		dst->laddr[1] = para->dst_image.addr[1];
		dst->laddr[2] = para->dst_image.addr[2];
		dst->haddr[0] = para->dst_image.addr[0] >> 32;
		dst->haddr[1] = para->dst_image.addr[1] >> 32;
		dst->haddr[2] = para->dst_image.addr[2] >> 32;
		dst->width = para->dst_image.w;
		dst->height = para->dst_image.h;
		dst->clip_rect.x = para->dst_rect.x;
		dst->clip_rect.y = para->dst_rect.y;
		dst->clip_rect.w = para->dst_rect.w;
		dst->clip_rect.h = para->dst_rect.h;
		dst->gamut = G2D_BT709;
		dst->alpha = para->alpha;
		dst->mode = 0;
		result = g2d_fillrectangle(dst, dst->color);
		return result;
	}
	dst->bbuff = 1;
	dst->color = para->color;
	dst->format =
		g2d_format_trans(para->dst_image.format,
				para->dst_image.pixel_seq);
	dst->laddr[0] = para->dst_image.addr[0];
	dst->laddr[1] = para->dst_image.addr[1];
	dst->laddr[2] = para->dst_image.addr[2];
	dst->haddr[0] = para->dst_image.addr[0] >> 32;
	dst->haddr[1] = para->dst_image.addr[1] >> 32;
	dst->haddr[2] = para->dst_image.addr[2] >> 32;
	dst->width = para->dst_image.w;
	dst->height = para->dst_image.h;
	dst->clip_rect.x = para->dst_rect.x;
	dst->clip_rect.y = para->dst_rect.y;
	dst->clip_rect.w = para->dst_rect.w;
	dst->clip_rect.h = para->dst_rect.h;
	dst->gamut = G2D_BT709;
	dst->alpha = para->alpha;
	if (para->flag & G2D_FIL_PIXEL_ALPHA)
		dst->mode = G2D_PIXEL_ALPHA;
	if (para->flag & G2D_FIL_PLANE_ALPHA)
		dst->mode = G2D_GLOBAL_ALPHA;
	if (para->flag & G2D_FIL_MULTI_ALPHA)
		dst->mode = G2D_MIXER_ALPHA;
	src->bbuff = 0;
	src->color = para->color;
	src->format = dst->format;
	src->gamut = G2D_BT709;
	src->format = 0;
	dst->laddr[0] = para->dst_image.addr[0];
	dst->haddr[0] = para->dst_image.addr[0] >> 32;
	src->width = para->dst_image.w;
	src->height = para->dst_image.h;
	src->clip_rect.x = para->dst_rect.x;
	src->clip_rect.y = para->dst_rect.y;
	src->clip_rect.w = para->dst_rect.w;
	src->clip_rect.h = para->dst_rect.h;
	result = g2d_bsp_bld(src, dst, G2D_BLD_DSTOVER, NULL);
	return result;
}

int32_t g2d_fillrectangle(g2d_image_enh *dst, uint32_t color_value)
{
	g2d_rect rect0;
	uint32_t tmp;
	int32_t result;

	g2d_bsp_reset();

	/* set the input layer */
	g2d_vlayer_set(0, dst);

	/* set the fill color value */
	g2d_fc_set(0, color_value);

	if (dst->format >= G2D_FORMAT_IYUV422_V0Y1U0Y0) {
		g2d_vsu_para_set(dst->format, dst->clip_rect.w,
				  dst->clip_rect.h, dst->clip_rect.w,
				  dst->clip_rect.h, 0xff);
		g2d_csc_reg_set(1, G2D_RGB2YUV_709);
	}

	/* for interleaved test */
	if ((dst->format >= G2D_FORMAT_IYUV422_V0Y1U0Y0)
			&& (dst->format <= G2D_FORMAT_IYUV422_Y1U0Y0V0)) {
		g2d_csc_reg_set(0, G2D_RGB2YUV_709);
		g2d_csc_reg_set(2, G2D_RGB2YUV_709);
		write_wvalue(BLD_CSC_CTL, 0x2);
		g2d_bk_set(0xff123456);
		porter_duff(G2D_BLD_SRCOVER);
		write_wvalue(BLD_FILLC0, 0x00108080);
		write_wvalue(BLD_FILLC1, 0x00108080);
		write_wvalue(UI0_FILLC, 0xffffffff);
		write_wvalue(UI1_FILLC, 0xffffffff);
	}

	rect0.x = 0;
	rect0.y = 0;
	rect0.w = dst->clip_rect.w;
	rect0.h = dst->clip_rect.h;
	g2d_bldin_set(0, rect0, dst->bpremul);
	g2d_bld_cs_set(dst->format);

	/* ROP sel ch0 pass */
	write_wvalue(ROP_CTL, 0xf0);
	g2d_wb_set(dst);

	/* start the module */
	mixer_irq_enable();
	tmp = read_wvalue(G2D_MIXER_CTL);
	tmp |= 0x80000000;
	G2D_INFO_MSG("INIT_MODULE: 0x%x\n", tmp);
	write_wvalue(G2D_MIXER_CTL, tmp);
	result = g2d_wait_cmd_finish();
	return result;
}


/**
 * src:source
 * ptn:pattern
 * dst:destination
 * mask:mask
 * if mask is set to NULL, do ROP3 among src, ptn, and dst using the
 * fore_flag
 */
int32_t g2d_bsp_maskblt(g2d_image_enh *src, g2d_image_enh *ptn,
			  g2d_image_enh *mask, g2d_image_enh *dst,
			  uint32_t back_flag, uint32_t fore_flag)
{
	uint32_t tmp;
	g2d_rect rect0;
	bool b_pre;
	int32_t result;

	/* int b_pre; */
	g2d_bsp_reset();
	if (dst == NULL)
		return -1;
	if (dst->format > G2D_FORMAT_BGRA1010102)
		return -2;
	g2d_vlayer_set(0, dst);
	if (src != NULL) {
		src->clip_rect.w = dst->clip_rect.w;
		src->clip_rect.h = dst->clip_rect.h;
		g2d_uilayer_set(0, src);
	}
	if (ptn != NULL) {
		ptn->clip_rect.w = dst->clip_rect.w;
		ptn->clip_rect.h = dst->clip_rect.h;
		g2d_uilayer_set(1, ptn);
	}
	if (mask != NULL) {
		mask->clip_rect.w = dst->clip_rect.w;
		mask->clip_rect.h = dst->clip_rect.h;
		g2d_uilayer_set(2, mask);

		/* set the ROP4 */
		write_wvalue(ROP_CTL, 0x1);
		g2d_rop3_set(0, back_flag & 0xff);
		g2d_rop3_set(1, fore_flag & 0xff);
	} else {
		write_wvalue(ROP_CTL, 0x0);
		g2d_rop3_set(0, back_flag);
	}
	b_pre = dst->bpremul;
	if (src)
		b_pre |= src->bpremul;
	if (ptn)
		b_pre |= ptn->bpremul;
	if (b_pre) {
		/* some layer is not premul */
		if (!src->bpremul) {
			tmp = read_wvalue(UI0_ATTR);
			tmp |= 0x1 << 16;
			write_wvalue(UI0_ATTR, tmp);
		}
		if (!dst->bpremul) {
			tmp = read_wvalue(V0_ATTCTL);
			tmp |= 0x1 << 16;
			write_wvalue(V0_ATTCTL, tmp);
		}
		if (!ptn->bpremul) {
			tmp = read_wvalue(UI1_ATTR);
			tmp |= 0x1 << 16;
			write_wvalue(UI1_ATTR, tmp);
		}

		    /* set bld in premul data */
		    write_wvalue(BLD_PREMUL_CTL, 0x1);
	}

	/*set bld para */
	rect0.x = 0;
	rect0.y = 0;
	rect0.w = dst->clip_rect.w;
	rect0.h = dst->clip_rect.h;
	g2d_bldin_set(0, rect0, dst->bpremul);
	g2d_wb_set(dst);

	/* start the module */
	mixer_irq_enable();
	tmp = read_wvalue(G2D_MIXER_CTL);
	tmp |= 0x80000000;
	G2D_INFO_MSG("INIT_MODULE: 0x%x\n", tmp);
	write_wvalue(G2D_MIXER_CTL, tmp);

	result = g2d_wait_cmd_finish();
	return result;
}

int32_t g2d_format_trans(int32_t data_fmt, int32_t pixel_seq)
{
	/* transform the g2d format 2 enhance format */
	switch (data_fmt) {
	case G2D_FMT_ARGB_AYUV8888:
		return G2D_FORMAT_ARGB8888;
	case G2D_FMT_BGRA_VUYA8888:
		return G2D_FORMAT_BGRA8888;
	case G2D_FMT_ABGR_AVUY8888:
		return G2D_FORMAT_ABGR8888;
	case G2D_FMT_RGBA_YUVA8888:
		return G2D_FORMAT_RGBA8888;
	case G2D_FMT_XRGB8888:
		return G2D_FORMAT_XRGB8888;
	case G2D_FMT_BGRX8888:
		return G2D_FORMAT_BGRX8888;
	case G2D_FMT_XBGR8888:
		return G2D_FORMAT_XBGR8888;
	case G2D_FMT_RGBX8888:
		return G2D_FORMAT_RGBX8888;
	case G2D_FMT_ARGB4444:
		return G2D_FORMAT_ARGB4444;
	case G2D_FMT_ABGR4444:
		return G2D_FORMAT_ABGR4444;
	case G2D_FMT_RGBA4444:
		return G2D_FORMAT_RGBA4444;
	case G2D_FMT_BGRA4444:
		return G2D_FORMAT_BGRA4444;
	case G2D_FMT_ARGB1555:
		return G2D_FORMAT_ARGB1555;
	case G2D_FMT_ABGR1555:
		return G2D_FORMAT_ABGR1555;
	case G2D_FMT_RGBA5551:
		return G2D_FORMAT_RGBA5551;
	case G2D_FMT_BGRA5551:
		return G2D_FORMAT_BGRA5551;
	case G2D_FMT_RGB565:
		return G2D_FORMAT_RGB565;
	case G2D_FMT_BGR565:
		return G2D_FORMAT_BGR565;
	case G2D_FMT_IYUV422:
		if (pixel_seq == G2D_SEQ_VYUY)
			return G2D_FORMAT_IYUV422_V0Y1U0Y0;
		if (pixel_seq == G2D_SEQ_YVYU)
			return G2D_FORMAT_IYUV422_Y1V0Y0U0;
		return -1;
	case G2D_FMT_PYUV422UVC:
		if (pixel_seq == G2D_SEQ_VUVU)
			return G2D_FORMAT_YUV422UVC_V1U1V0U0;
		return G2D_FORMAT_YUV422UVC_U1V1U0V0;
	case G2D_FMT_PYUV420UVC:
		if (pixel_seq == G2D_SEQ_VUVU)
			return G2D_FORMAT_YUV420UVC_V1U1V0U0;
		return G2D_FORMAT_YUV420UVC_U1V1U0V0;
	case G2D_FMT_PYUV411UVC:
		if (pixel_seq == G2D_SEQ_VUVU)
			return G2D_FORMAT_YUV411UVC_V1U1V0U0;
		return G2D_FORMAT_YUV411UVC_U1V1U0V0;
	case G2D_FMT_PYUV422:
		return G2D_FORMAT_YUV422_PLANAR;
	case G2D_FMT_PYUV420:
		return G2D_FORMAT_YUV420_PLANAR;
	case G2D_FMT_PYUV411:
		return G2D_FORMAT_YUV411_PLANAR;
	default:
		return -1;
	}
}

int32_t mixer_stretchblt(g2d_stretchblt *para,
			   enum g2d_scan_order scan_order)
{
	g2d_image_enh src_tmp, dst_tmp;
	g2d_image_enh *src = &src_tmp, *dst = &dst_tmp;
	g2d_ck ck_para_tmp;
	g2d_ck *ck_para = &ck_para_tmp;
	int32_t result;

	memset(src, 0, sizeof(g2d_image_enh));
	memset(dst, 0, sizeof(g2d_image_enh));
	memset(ck_para, 0, sizeof(g2d_ck));

	src->bbuff = 1;
	src->color = para->color;
	src->format =
		g2d_format_trans(para->src_image.format,
				para->src_image.pixel_seq);
	src->laddr[0] = para->src_image.addr[0];
	src->laddr[1] = para->src_image.addr[1];
	src->laddr[2] = para->src_image.addr[2];
	src->haddr[0] = para->src_image.addr[0] >> 32;
	src->haddr[1] = para->src_image.addr[1] >> 32;
	src->haddr[2] = para->src_image.addr[2] >> 32;
	src->width = para->src_image.w;
	src->height = para->src_image.h;
	src->clip_rect.x = para->src_rect.x;
	src->clip_rect.y = para->src_rect.y;
	src->clip_rect.w = para->src_rect.w;
	src->clip_rect.h = para->src_rect.h;
	src->gamut = G2D_BT709;
	src->alpha = para->alpha;
	dst->bbuff = 1;
	dst->color = para->color;
	dst->format =
		g2d_format_trans(para->dst_image.format,
				para->dst_image.pixel_seq);
	dst->laddr[0] = para->dst_image.addr[0];
	dst->laddr[1] = para->dst_image.addr[1];
	dst->laddr[2] = para->dst_image.addr[2];
	dst->haddr[0] = para->dst_image.addr[0] >> 32;
	dst->haddr[1] = para->dst_image.addr[1] >> 32;
	dst->haddr[2] = para->dst_image.addr[2] >> 32;
	dst->width = para->dst_image.w;
	dst->height = para->dst_image.h;
	dst->clip_rect.x = para->dst_rect.x;
	dst->clip_rect.y = para->dst_rect.y;
	dst->clip_rect.w = para->dst_rect.w;
	dst->clip_rect.h = para->dst_rect.h;
	dst->gamut = G2D_BT709;
	dst->alpha = para->alpha;

	if ((para->flag == G2D_BLT_NONE) ||
	      (para->flag == G2D_BLT_FLIP_HORIZONTAL) ||
	      (para->flag == G2D_BLT_FLIP_VERTICAL) ||
	      (para->flag == G2D_BLT_ROTATE90) ||
	      (para->flag == G2D_BLT_ROTATE180) ||
	      (para->flag == G2D_BLT_ROTATE270) ||
	      (para->flag == G2D_BLT_MIRROR45) ||
	      (para->flag == G2D_BLT_MIRROR135)) {
		/* ROT or scal case */
		switch (para->flag) {
		case G2D_BLT_NONE:
			if ((dst->width == src->width) &&
					(dst->height == src->height)) {
				result = g2d_bsp_bitblt(src, dst, G2D_ROT_0);
				return result;
			}
			if (scan_order == G2D_SM_DTLR)
				result = g2d_bsp_bitblt(src, dst,
						G2D_BLT_NONE | G2D_SM_DTLR_1);
			else
				result = g2d_bsp_bitblt(src, dst, G2D_BLT_NONE);
			return result;
		case G2D_BLT_FLIP_HORIZONTAL:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_H);
			return result;
		case G2D_BLT_FLIP_VERTICAL:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_V);
			return result;
		case G2D_BLT_ROTATE90:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_90);
			return result;
		case G2D_BLT_ROTATE180:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_180);
			return result;
		case G2D_BLT_ROTATE270:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_270);
			return result;
		case G2D_BLT_MIRROR45:
			result = g2d_bsp_bitblt(src, dst,
						G2D_ROT_90 | G2D_ROT_H);
			return result;
		case G2D_BLT_MIRROR135:
			result = g2d_bsp_bitblt(src, dst,
						G2D_ROT_90 | G2D_ROT_V);
			return result;
		default:
			return -1;
		}
	} else {
		if (para->flag & 0xfe0) {
			pr_err("Wrong! mixer and rot cant use at same time!\n");
			return -1;
		}
		if (para->flag & G2D_BLT_SRC_PREMULTIPLY)
			src->bpremul = 1;
		if (para->flag & G2D_BLT_DST_PREMULTIPLY)
			dst->bpremul = 1;
		if (para->flag & G2D_BLT_PIXEL_ALPHA) {
			src->mode = G2D_PIXEL_ALPHA;
			dst->mode = G2D_PIXEL_ALPHA;
		}
		if (para->flag & G2D_BLT_PLANE_ALPHA) {
			src->mode = G2D_GLOBAL_ALPHA;
			dst->mode = G2D_GLOBAL_ALPHA;
		}
		if (para->flag & G2D_BLT_MULTI_ALPHA) {
			src->mode = G2D_MIXER_ALPHA;
			dst->mode = G2D_MIXER_ALPHA;
		}
		ck_para->match_rule = 0;
		ck_para->max_color  = __UQADD8(para->color,0x00010101);
		ck_para->min_color  = __UQSUB8(para->color,0x00010101);

		result = g2d_bsp_bitblt(src, dst, G2D_BLT_NONE /*G2D_BLT_NONE*/ );

		return result;
	}
}

unsigned int PorterDuff=G2D_BLD_SRCOVER;

int32_t mixer_blt(g2d_blt *para, enum g2d_scan_order scan_order)
{
	g2d_image_enh src_tmp, dst_tmp;
	g2d_image_enh *src = &src_tmp;
	g2d_image_enh *dst = &dst_tmp;
	g2d_ck ck_para_tmp;
	g2d_ck *ck_para = &ck_para_tmp;
	int32_t result;

	memset(src, 0, sizeof(g2d_image_enh));
	memset(dst, 0, sizeof(g2d_image_enh));
	memset(ck_para, 0, sizeof(g2d_ck));

	G2D_INFO_MSG("Input_G2D_Format:  0x%x\n", para->src_image.format);
	G2D_INFO_MSG("BITBLT_flag:  0x%x\n", para->flag);
	G2D_INFO_MSG("inPICWidth:  %d\n", para->src_image.w);
	G2D_INFO_MSG("inPICHeight: %d\n", para->src_image.h);
	G2D_INFO_MSG("inRectX:  %d\n", para->src_rect.x);
	G2D_INFO_MSG("inRectY: %d\n", para->src_rect.y);
	G2D_INFO_MSG("inRectW:  %d\n", para->src_rect.w);
	G2D_INFO_MSG("inRectH: %d\n", para->src_rect.h);
	G2D_INFO_MSG("Output_G2D_Format:  0x%x\n", para->dst_image.format);
	G2D_INFO_MSG("outPICWidth:  %d\n", para->dst_image.w);
	G2D_INFO_MSG("outPICHeight: %d\n", para->dst_image.h);
	G2D_INFO_MSG("outRectX:  %d\n", para->dst_x);
	G2D_INFO_MSG("outRectY: %d\n", para->dst_y);
	src->bbuff = 1;
	src->color = para->color;
	src->format =
			g2d_format_trans(para->src_image.format,
					para->src_image.pixel_seq);
	src->laddr[0] = para->src_image.addr[0];
	src->laddr[1] = para->src_image.addr[1];
	src->laddr[2] = para->src_image.addr[2];
	src->haddr[0] = para->src_image.addr[0] >> 32;
	src->haddr[1] = para->src_image.addr[1] >> 32;
	src->haddr[2] = para->src_image.addr[2] >> 32;
	src->width = para->src_image.w;
	src->height = para->src_image.h;
	src->clip_rect.x = para->src_rect.x;
	src->clip_rect.y = para->src_rect.y;
	src->clip_rect.w = para->src_rect.w;
	src->clip_rect.h = para->src_rect.h;
	src->gamut = G2D_BT709;
	src->alpha = para->alpha;
	dst->bbuff = 1;
	dst->format =
		g2d_format_trans(para->dst_image.format,
						para->dst_image.pixel_seq);
	dst->laddr[0] = para->dst_image.addr[0];
	dst->laddr[1] = para->dst_image.addr[1];
	dst->laddr[2] = para->dst_image.addr[2];
	dst->haddr[0] = para->dst_image.addr[0] >> 32;
	dst->haddr[1] = para->dst_image.addr[1] >> 32;
	dst->haddr[2] = para->dst_image.addr[2] >> 32;
	dst->width = para->dst_image.w;
	dst->height = para->dst_image.h;
	dst->clip_rect.x = para->dst_x;
	dst->clip_rect.y = para->dst_y;
	dst->clip_rect.w = src->clip_rect.w;
	dst->clip_rect.h = src->clip_rect.h;
	dst->gamut = G2D_BT709;
	dst->alpha = para->alpha;

	G2D_INFO_MSG("inPICaddr0: 0x%x\n", src->laddr[0]);
	G2D_INFO_MSG("inPICaddr1: 0x%x\n", src->laddr[1]);
	G2D_INFO_MSG("inPICaddr2: 0x%x\n", src->laddr[2]);
	G2D_INFO_MSG("outPICaddr0: 0x%x\n", dst->laddr[0]);
	G2D_INFO_MSG("outPICaddr1: 0x%x\n", dst->laddr[1]);
	G2D_INFO_MSG("outPICaddr2: 0x%x\n", dst->laddr[2]);

	if ((para->flag == G2D_BLT_NONE) ||
	      (para->flag == G2D_BLT_FLIP_HORIZONTAL) ||
	      (para->flag == G2D_BLT_FLIP_VERTICAL) ||
	      (para->flag == G2D_BLT_ROTATE90) ||
	      (para->flag == G2D_BLT_ROTATE180) ||
	      (para->flag == G2D_BLT_ROTATE270) ||
	      (para->flag == G2D_BLT_MIRROR45) ||
	      (para->flag == G2D_BLT_MIRROR135)) {
		/* ROT case */
		switch (para->flag) {
		case G2D_BLT_NONE:
			if ((dst->width == src->width) &&
					(dst->height == src->height)) {
				result = g2d_bsp_bitblt(src, dst, G2D_ROT_0);
				return result;
			}
			if (scan_order == G2D_SM_DTLR)
				result = g2d_bsp_bitblt(src, dst,
						G2D_BLT_NONE | G2D_SM_DTLR_1);
			else
				result = g2d_bsp_bitblt(src, dst, G2D_BLT_NONE);
			return result;
		case G2D_BLT_FLIP_HORIZONTAL:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_H);
			return result;
		case G2D_BLT_FLIP_VERTICAL:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_V);
			return result;
		case G2D_BLT_ROTATE90:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_90);
			return result;
		case G2D_BLT_ROTATE180:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_180);
			return result;
		case G2D_BLT_ROTATE270:
			result = g2d_bsp_bitblt(src, dst, G2D_ROT_270);
			return result;
		case G2D_BLT_MIRROR45:
			result = g2d_bsp_bitblt(src, dst,
						G2D_ROT_90 | G2D_ROT_H);
			return result;
		case G2D_BLT_MIRROR135:
			result = g2d_bsp_bitblt(src, dst,
						G2D_ROT_90 | G2D_ROT_V);
			return result;
		default:
			return -1;
		}
	} else {
		if (para->flag & 0xfe0) {
			pr_err("Wrong! mixer and rot cant use at same time!\n");
			return -1;
		}
		if (para->flag & G2D_BLT_SRC_PREMULTIPLY)
			src->bpremul = 1;
		if (para->flag & G2D_BLT_DST_PREMULTIPLY)
			dst->bpremul = 1;
		if (para->flag & G2D_BLT_PIXEL_ALPHA) {
			src->mode = G2D_PIXEL_ALPHA;
			dst->mode = G2D_PIXEL_ALPHA;
		}
		if (para->flag & G2D_BLT_PLANE_ALPHA) {
			src->mode = G2D_GLOBAL_ALPHA;
			dst->mode = G2D_GLOBAL_ALPHA;
		}
		if (para->flag & G2D_BLT_MULTI_ALPHA) {
			src->mode = G2D_MIXER_ALPHA;
			dst->mode = G2D_MIXER_ALPHA;
		}
		/* ������? ������� �������� ��� */
		ck_para->match_rule = 0;
		ck_para->max_color  = __UQADD8(para->color,0x00010101); //para->color;
		ck_para->min_color  = __UQSUB8(para->color,0x00010101); //para->color;
//		ck_para->max_color  = para->color;
//		ck_para->min_color  = para->color;

		result = g2d_bsp_bld(src, dst, PorterDuff /*G2D_BLD_SRCOVER*/ /*G2D_BLD_DSTOVER*/ , ck_para /*NULL*/); //��������� ��������� � �������� ����!

		return result;
	}
}

int32_t g2d_bsp_bitblt(g2d_image_enh *src, g2d_image_enh *dst, uint32_t flag)
{
	g2d_rect rect0, rect1;
	bool bpre;
	uint32_t ycnt, ucnt, vcnt;
	uint32_t pitch0, pitch1, pitch2;
	uint64_t addr0, addr1, addr2;
	uint32_t midw, midh;
	uint32_t tmp;
	uint32_t /*ch, */cw, cy, cx;
	int32_t result;

	g2d_bsp_reset();
	if (dst == NULL) {
		pr_err("[G2D]dst image is NULL!\n");
		return -1;
	}
	if (src == NULL) {
		pr_err("[G2D]src image is NULL!\n");
		return -2;
	}
	G2D_INFO_MSG("BITBLT_flag:  0x%x\n", flag);
	if (G2D_BLT_NONE == (flag & 0x0fffffff)) {
		G2D_INFO_MSG("Input info:---------------------------------\n");
		G2D_INFO_MSG("Src_fd:  %d\n", src->fd);
		G2D_INFO_MSG("Format:  0x%x\n", src->format);
		G2D_INFO_MSG("BITBLT_alpha_mode:  0x%x\n", src->mode);
		G2D_INFO_MSG("BITBLT_alpha_val:  0x%x\n", src->alpha);
		G2D_INFO_MSG("inClipRectX:  %d\n", src->clip_rect.x);
		G2D_INFO_MSG("inClipRectY: %d\n", src->clip_rect.y);
		G2D_INFO_MSG("inClipRectW:  %d\n", src->clip_rect.w);
		G2D_INFO_MSG("inClipRectH: %d\n", src->clip_rect.h);

		G2D_INFO_MSG("Output info:--------------------------------\n");
		G2D_INFO_MSG("Dst_fd:  %d\n", dst->fd);
		G2D_INFO_MSG("Format:  0x%x\n", dst->format);
		G2D_INFO_MSG("outClipRectX:  %d\n", dst->clip_rect.x);
		G2D_INFO_MSG("outClipRectY: %d\n", dst->clip_rect.y);
		G2D_INFO_MSG("outClipRectW:  %d\n", dst->clip_rect.w);
		G2D_INFO_MSG("outClipRectH: %d\n", dst->clip_rect.h);
		/*single src opt */
		g2d_vlayer_set(0, src);
		if (src->mode) {
			/* need abp process */
			g2d_uilayer_set(2, dst);
		}
		if ((src->format >= G2D_FORMAT_IYUV422_V0Y1U0Y0) ||
				(src->clip_rect.w != dst->clip_rect.w) ||
				(src->clip_rect.h != dst->clip_rect.h)) {
			g2d_calc_coarse(src->format, src->clip_rect.w,
					src->clip_rect.h, dst->clip_rect.w,
					dst->clip_rect.h, &midw, &midh);
			g2d_vsu_para_set(src->format, midw, midh,
					dst->clip_rect.w, dst->clip_rect.h,
					0xff);
		}
		write_wvalue(ROP_CTL, 0xf0);
		/*set bld para */
		rect0.x = 0;
		rect0.y = 0;
		rect0.w = dst->clip_rect.w;
		rect0.h = dst->clip_rect.h;
		g2d_bldin_set(0, rect0, dst->bpremul);
		g2d_bld_cs_set(src->format);
		if (src->mode) {
			/* need abp process */
			rect1.x = 0;
			rect1.y = 0;
			rect1.w = dst->clip_rect.w;
			rect1.h = dst->clip_rect.h;
			g2d_bldin_set(1, rect1, dst->bpremul);
		}
		if ((src->format <= G2D_FORMAT_BGRA1010102) &&
		      (dst->format > G2D_FORMAT_BGRA1010102)) {
			if (dst->clip_rect.w <= 1280 && dst->clip_rect.h <= 720)
				g2d_csc_reg_set(2, G2D_RGB2YUV_601);
			else
				g2d_csc_reg_set(2, G2D_RGB2YUV_709);
		}
		if ((src->format > G2D_FORMAT_BGRA1010102) &&
		      (dst->format <= G2D_FORMAT_BGRA1010102)) {
			if (dst->clip_rect.w <= 1280 && dst->clip_rect.h <= 720)
				g2d_csc_reg_set(2, G2D_YUV2RGB_601);
			else
				g2d_csc_reg_set(2, G2D_YUV2RGB_709);
		}
		g2d_wb_set(dst);
	}

	else if (flag & 0xff) {
		/* ROP2 operate */
		if ((src->format > G2D_FORMAT_BGRA1010102) | (dst->format >
					G2D_FORMAT_BGRA1010102))
			return -3;
		g2d_uilayer_set(0, dst);
		g2d_vlayer_set(0, src);

		/* bpre = 0; */
		bpre = false;
		if (src->bpremul || dst->bpremul) {
			bpre = true;
			/* bpre = 1; */
			/* some layer is premul */
			if (!src->bpremul) {
				tmp = read_wvalue(V0_ATTCTL);
				tmp |= 0x1 << 16;
				write_wvalue(V0_ATTCTL, tmp);
			}
			if (!dst->bpremul) {
				tmp = read_wvalue(UI0_ATTR);
				tmp |= 0x1 << 16;
				write_wvalue(UI0_ATTR, tmp);
			}
		}
		if ((src->clip_rect.w != dst->clip_rect.w)
		      || (src->clip_rect.h != dst->clip_rect.h)) {
			g2d_calc_coarse(src->format, src->clip_rect.w,
					 src->clip_rect.h, dst->clip_rect.w,
					 dst->clip_rect.h, &midw, &midh);
			g2d_vsu_para_set(src->format, midw, midh,
					   dst->clip_rect.w, dst->clip_rect.h,
					   0xff);
		}
		write_wvalue(ROP_CTL, 0x0);
		g2d_rop2_set(flag & 0xff);
		tmp = read_wvalue(ROP_INDEX0);
		tmp |= 0x2;
		write_wvalue(ROP_INDEX0, tmp);

		/*set bld para */
		rect0.x = 0;
		rect0.y = 0;
		rect0.w = dst->clip_rect.w;
		rect0.h = dst->clip_rect.h;
		g2d_bldin_set(0, rect0, bpre);
		g2d_wb_set(dst);
	}

	else if (flag & 0xff00) {
		/* ROT operate */
		tmp = 1;
		if (flag & G2D_ROT_H)
			tmp |= 0x1 << 7;
		if (flag & G2D_ROT_V)
			tmp |= 0x1 << 6;
		if ((flag & 0xf00) == G2D_ROT_90)
			tmp |= 0x1 << 4;
		if ((flag & 0xf00) == G2D_ROT_180)
			tmp |= 0x2 << 4;
		if ((flag & 0xf00) == G2D_ROT_270)
			tmp |= 0x3 << 4;
		if ((flag & 0xf00) == G2D_ROT_0)
			tmp |= 0x0 << 4;

		G2D_INFO_MSG("ROT input info: ----------------------------\n");
		G2D_INFO_MSG("Src_fd:  %d\n", src->fd);
		G2D_INFO_MSG("Format:  0x%x\n", src->format);
		G2D_INFO_MSG("Flag:  0x%x\n", flag);
		G2D_INFO_MSG("inClipRectX:  %d\n", src->clip_rect.x);
		G2D_INFO_MSG("inClipRectY: %d\n", src->clip_rect.y);
		G2D_INFO_MSG("inClipRectW:  %d\n", src->clip_rect.w);
		G2D_INFO_MSG("inClipRectH: %d\n", src->clip_rect.h);

		write_wvalue(ROT_CTL, tmp);
		write_wvalue(ROT_IFMT, src->format & 0x3F);
		write_wvalue(ROT_ISIZE,
			      ((((src->clip_rect.h -
				  1) & 0x1fff) << 16)) | ((src->clip_rect.w -
							    1) & 0x1fff));

		G2D_INFO_MSG("ROT_IFMT: 0x%x\n", read_wvalue(ROT_IFMT));
		G2D_INFO_MSG("ROT_ISIZE: 0x%x\n", read_wvalue(ROT_ISIZE));
		G2D_INFO_MSG("SRC_align: %d, %d, %d\n",
				src->align[0], src->align[1], src->align[2]);
		G2D_INFO_MSG("DST_align: %d, %d, %d\n",
				dst->align[0], dst->align[1], dst->align[2]);

		if ((src->format >= G2D_FORMAT_YUV422UVC_V1U1V0U0)
			&& (src->format <= G2D_FORMAT_YUV422_PLANAR)) {
			cw = src->width >> 1;
			//ch = src->height;
			cx = src->clip_rect.x >> 1;
			cy = src->clip_rect.y;
		}

		else if ((src->format >= G2D_FORMAT_YUV420UVC_V1U1V0U0)
			&& (src->format <= G2D_FORMAT_YUV420_PLANAR)) {
			cw = src->width >> 1;
			//ch = src->height >> 1;
			cx = src->clip_rect.x >> 1;
			cy = src->clip_rect.y >> 1;
		}

		else if ((src->format >= G2D_FORMAT_YUV411UVC_V1U1V0U0)
			&& (src->format <= G2D_FORMAT_YUV411_PLANAR)) {
			cw = src->width >> 2;
			//ch = src->height;
			cx = src->clip_rect.x >> 2;
			cy = src->clip_rect.y;
		}

		else {
			cw = 0;
			//ch = 0;
			cx = 0;
			cy = 0;
		}

		g2d_byte_cal(src->format, &ycnt, &ucnt, &vcnt);
		pitch0 = cal_align(ycnt * src->width, src->align[0]);
		write_wvalue(ROT_IPITCH0, pitch0);
		pitch1 = cal_align(ucnt * cw, src->align[1]);
		write_wvalue(ROT_IPITCH1, pitch1);
		pitch2 = cal_align(vcnt * cw, src->align[2]);
		write_wvalue(ROT_IPITCH2, pitch2);

		G2D_INFO_MSG("ROT_InPITCH: %d, %d, %d\n",
				pitch0, pitch1, pitch2);
		G2D_INFO_MSG("SRC_ADDR0: 0x%x\n", src->laddr[0]);
		G2D_INFO_MSG("SRC_ADDR1: 0x%x\n", src->laddr[1]);
		G2D_INFO_MSG("SRC_ADDR2: 0x%x\n", src->laddr[2]);

		addr0 =
		    src->laddr[0] + ((uint64_t) src->haddr[0] << 32) +
		    pitch0 * src->clip_rect.y + ycnt * src->clip_rect.x;
		write_wvalue(ROT_ILADD0, addr0 & 0xffffffff);
		write_wvalue(ROT_IHADD0, (addr0 >> 32) & 0xff);
		addr1 =
		    src->laddr[1] + ((uint64_t) src->haddr[1] << 32) +
		    pitch1 * cy + ucnt * cx;
		write_wvalue(ROT_ILADD1, addr1 & 0xffffffff);
		write_wvalue(ROT_IHADD1, (addr1 >> 32) & 0xff);
		addr2 =
		    src->laddr[2] + ((uint64_t) src->haddr[2] << 32) +
		    pitch2 * cy + vcnt * cx;
		write_wvalue(ROT_ILADD2, addr2 & 0xffffffff);
		write_wvalue(ROT_IHADD2, (addr2 >> 32) & 0xff);

		if (((flag & 0xf00) == G2D_ROT_90) | ((flag & 0xf00) ==
							G2D_ROT_270)) {
			dst->clip_rect.w = src->clip_rect.h;
			dst->clip_rect.h = src->clip_rect.w;
		}

		else {
			dst->clip_rect.w = src->clip_rect.w;
			dst->clip_rect.h = src->clip_rect.h;
		}
		write_wvalue(ROT_OSIZE,
			       ((((dst->clip_rect.h -
				   1) & 0x1fff) << 16)) | ((dst->clip_rect.w -
							     1) & 0x1fff));
		/* YUV output fmt only support 420 */
		if (src->format == G2D_FORMAT_YUV422UVC_V1U1V0U0)
			dst->format = G2D_FORMAT_YUV420UVC_V1U1V0U0;
		else if (src->format == G2D_FORMAT_YUV422UVC_U1V1U0V0)
			dst->format = G2D_FORMAT_YUV420UVC_U1V1U0V0;
		else if (src->format == G2D_FORMAT_YUV422_PLANAR)
			dst->format = G2D_FORMAT_YUV420_PLANAR;
		else
			dst->format = src->format;

		if ((dst->format >= G2D_FORMAT_YUV422UVC_V1U1V0U0)
			&& (dst->format <= G2D_FORMAT_YUV422_PLANAR)) {
			cw = dst->width >> 1;
			//ch = dst->height;
			cx = dst->clip_rect.x >> 1;
			cy = dst->clip_rect.y;
		}

		else if ((dst->format >= G2D_FORMAT_YUV420UVC_V1U1V0U0)
			&& (dst->format <= G2D_FORMAT_YUV420_PLANAR)) {
			cw = dst->width >> 1;
			//ch = dst->height >> 1;
			cx = dst->clip_rect.x >> 1;
			cy = dst->clip_rect.y >> 1;
		}

		else if ((dst->format >= G2D_FORMAT_YUV411UVC_V1U1V0U0)
			&& (dst->format <= G2D_FORMAT_YUV411_PLANAR)) {
			cw = dst->width >> 2;
			//ch = dst->height;
			cx = dst->clip_rect.x >> 2;
			cy = dst->clip_rect.y;
		}

		else {
			cw = 0;
			//ch = 0;
			cx = 0;
			cy = 0;
		}

		g2d_byte_cal(dst->format, &ycnt, &ucnt, &vcnt);
		G2D_INFO_MSG("ROT output info: ----------------------------\n");
		G2D_INFO_MSG("Dst_fd:  %d\n", dst->fd);
		G2D_INFO_MSG("Format:  0x%x\n", dst->format);

		pitch0 = cal_align(ycnt * dst->width, dst->align[0]);
		write_wvalue(ROT_OPITCH0, pitch0);
		pitch1 = cal_align(ucnt * cw, dst->align[1]);
		write_wvalue(ROT_OPITCH1, pitch1);
		pitch2 = cal_align(vcnt * cw, dst->align[2]);
		write_wvalue(ROT_OPITCH2, pitch2);

		G2D_INFO_MSG("ROT_OutPITCH: %d, %d, %d\n",
				pitch0, pitch1, pitch2);
		G2D_INFO_MSG("outClipRectX:  %d\n", dst->clip_rect.x);
		G2D_INFO_MSG("outClipRectY: %d\n", dst->clip_rect.y);
		G2D_INFO_MSG("outClipRectW:  %d\n", dst->clip_rect.w);
		G2D_INFO_MSG("outClipRectH: %d\n", dst->clip_rect.h);
		addr0 =
		    dst->laddr[0] + ((uint64_t) dst->haddr[0] << 32) +
		    pitch0 * dst->clip_rect.y + ycnt * dst->clip_rect.x;
		write_wvalue(ROT_OLADD0, addr0 & 0xffffffff);
		write_wvalue(ROT_OHADD0, (addr0 >> 32) & 0xff);
		addr1 =
		    dst->laddr[1] + ((uint64_t) dst->haddr[1] << 32) +
		    pitch1 * cy + ucnt * cx;
		write_wvalue(ROT_OLADD1, addr1 & 0xffffffff);
		write_wvalue(ROT_OHADD1, (addr1 >> 32) & 0xff);
		addr2 =
		    dst->laddr[2] + ((uint64_t) dst->haddr[2] << 32) +
		    pitch2 * cy + vcnt * cx;
		write_wvalue(ROT_OLADD2, addr2 & 0xffffffff);
		write_wvalue(ROT_OHADD2, (addr2 >> 32) & 0xff);

		G2D_INFO_MSG("DST_ADDR0: 0x%x\n", dst->laddr[0]);
		G2D_INFO_MSG("DST_ADDR1: 0x%x\n", dst->laddr[1]);
		G2D_INFO_MSG("DST_ADDR2: 0x%x\n", dst->laddr[2]);

		/* start the module */
		rot_irq_enable();
		tmp = read_wvalue(ROT_CTL);
		tmp |= (0x1 << 31);
		G2D_INFO_MSG("init_module: 0x%x\n", tmp);
		write_wvalue(ROT_CTL, tmp);

		result = g2d_wait_cmd_finish();

		return result;
	}
	g2d_scan_order_fun(flag & 0xf0000000);

	/* start the module */
	mixer_irq_enable();
	tmp = read_wvalue(G2D_MIXER_CTL);
	tmp |= 0x80000000;
	G2D_INFO_MSG("INIT_MODULE: 0x%x\n", tmp);
	write_wvalue(G2D_MIXER_CTL, tmp);

	result = g2d_wait_cmd_finish();
	return result;
}

int32_t g2d_bsp_bld(g2d_image_enh *src, g2d_image_enh *dst, uint32_t flag,
		    g2d_ck *ck_para)
{
	g2d_rect rect0, rect1;
	uint32_t tmp;
	int32_t result;

	if (dst == NULL)
		return -1;
	g2d_mixer_reset();
	g2d_vlayer_set(0, dst);
	g2d_uilayer_set(2, src);
	if ((dst->format > G2D_FORMAT_BGRA1010102) &&
			(src->format <= G2D_FORMAT_BGRA1010102))
		g2d_csc_reg_set(1, G2D_RGB2YUV_709);
	write_wvalue(ROP_CTL, 0xF0);

	rect0.x = 0;
	rect0.y = 0;
	rect0.w = dst->clip_rect.w;
	rect0.h = dst->clip_rect.h;

	rect1.x = 0;
	rect1.y = 0;
	rect1.w = src->clip_rect.w;
	rect1.h = src->clip_rect.h;
	g2d_bldin_set(0, rect0, dst->bpremul);
	g2d_bldin_set(1, rect1, src->bpremul);

	G2D_INFO_MSG("BLD_FLAG:  0x%x\n", flag);

	porter_duff(flag & 0xFFF);

flag|=G2D_CK_SRC;

	if (flag & G2D_CK_SRC)
        {
		write_wvalue(BLD_KEY_CTL, 0x3);
        }
	else if (flag & G2D_CK_DST)
        {
		write_wvalue(BLD_KEY_CTL, 0x1);
        }

	if (ck_para != NULL)
        {
		ck_para_set(ck_para);
        }

	g2d_wb_set(dst);
	g2d_bld_cs_set(dst->format);

	/* start the modult */
	mixer_irq_enable();
	tmp = read_wvalue(G2D_MIXER_CTL);
	tmp |= 0x80000000;
	G2D_INFO_MSG("INIT_MODULE: 0x%x\n", tmp);
	write_wvalue(G2D_MIXER_CTL, tmp);

	result = g2d_wait_cmd_finish();
	return result;
}

int32_t g2d_get_clk_cnt(uint32_t *clk)
{
	int32_t ret;

	ret = read_wvalue(G2D_MIXER_CLK);
	if (ret != 0)
		return -1;

	    /* clear clk cnt */
	    write_wvalue(G2D_MIXER_CLK, 0);
	return 0;
}

/*
 * g2d.c
 */

int g2d_ext_hd_finish_flag=0;
//
//void G2D_Clk(void)
//{
// G2D_BGR_REG&=~(1<<16);               //assert Reset
//
// G2D_CLK_REG=(1UL<<31)|(1<<24)|(3-1); //enable clk, PLLVIDEO0(4x), div 3
//
// G2D_BGR_REG|=1;                      //pass G2D clock
//
// MBUS_MAT_CLK_GATING_REG|=(1<<10)|1;  //MBUS gating for G2D & DMA
//
// G2D_BGR_REG|=(1<<16);                //de-assert Reset
//}
//
//int g2d_init(g2d_init_para *para)
//{
// mixer_set_reg_base(G2D_TOP_BASE);
// return 0;
//}
//
//static const g2d_init_para G2D_INIT=
//{
// .g2d_base=G2D_TOP_BASE
//};
//
//void G2D_Init(void)
//{
// //G2D_Clk();                  //�����, �����, ������...
// g2d_init((void*)&G2D_INIT); //��������� �������� ������
//
// g2d_bsp_open();
//
// PRINTF("\nG2D Open!\n");
//}

/* ������ � �������� ���������� ������ G2D */
/* 0 - timeout. 1 - OK */
//int xhwacc_waitdone(void)
//{
////	unsigned n = 0x20000000;
////	xG2D_MIXER->G2D_MIXER_CTL |= (1u << 31);	/* start the module */
//	for (;;)
//	{
//		const uint_fast32_t MASK = (1u << 0);	/* FINISH_IRQ */
//		const uint_fast32_t sts = xG2D_MIXER->G2D_MIXER_INT;
//		if (((sts & MASK) != 0))
//		{
//			xG2D_MIXER->G2D_MIXER_INT = MASK;
//			break;
//		}
//		hardware_nonguiyield();
//		if (-- n == 0)
//		{
//			PRINTF("xG2D_MIXER->G2D_MIXER_CTL=%08X, xG2D_MIXER->G2D_MIXER_INT=%08X\n", xG2D_MIXER->G2D_MIXER_CTL, xG2D_MIXER->G2D_MIXER_INT);
//			return 0;
//		}
//	}
//	ASSERT((xG2D_MIXER->G2D_MIXER_CTL & (1u << 31)) == 0);
//	return 1;
//}

int g2d_wait_cmd_finish(void)
{
 uint32_t mixer_irq_flag,rot_irq_flag;

 Loop:

 mixer_irq_flag=mixer_irq_query();
 rot_irq_flag=rot_irq_query();

 if(mixer_irq_flag==0)
 {
  g2d_mixer_reset();
  return 1;
 }
 else if(rot_irq_flag==0)
 {
  g2d_rot_reset();
  return 2;
 }

 goto Loop;
}

int g2d_fill(g2d_fillrect *para)
{
	int32_t err = 0;

	/* check the parameter valid */
	if (((para->dst_rect.x < 0) &&
	     ((-para->dst_rect.x) > para->dst_rect.w)) ||
	    ((para->dst_rect.y < 0) &&
	     ((-para->dst_rect.y) > para->dst_rect.h)) ||
	    ((para->dst_rect.x > 0) &&
	     (para->dst_rect.x > para->dst_image.w - 1)) ||
	    ((para->dst_rect.y > 0) &&
	     (para->dst_rect.y > para->dst_image.h - 1))) {
		pr_warn("invalid fillrect parameter setting");
		return -22;
	}
	if (((para->dst_rect.x < 0) &&
				((-para->dst_rect.x) < para->dst_rect.w))) {
		para->dst_rect.w = para->dst_rect.w + para->dst_rect.x;
		para->dst_rect.x = 0;
	} else if ((para->dst_rect.x + para->dst_rect.w)
			> para->dst_image.w) {
		para->dst_rect.w = para->dst_image.w - para->dst_rect.x;
	}
	if (((para->dst_rect.y < 0) &&
				((-para->dst_rect.y) < para->dst_rect.h))) {
		para->dst_rect.h = para->dst_rect.h + para->dst_rect.y;
		para->dst_rect.y = 0;
	} else if ((para->dst_rect.y + para->dst_rect.h)
			> para->dst_image.h)
		para->dst_rect.h = para->dst_image.h - para->dst_rect.y;

	g2d_ext_hd_finish_flag = 0;

	err = mixer_fillrectangle(para);

	return err;
}

enum g2d_scan_order scan_order=G2D_SM_TDLR;

int g2d_blit(g2d_blt *para)
{
	int32_t err = 0;
	uint32_t tmp_w, tmp_h;

	if ((para->flag & G2D_BLT_ROTATE90) ||
			(para->flag & G2D_BLT_ROTATE270)) {
		tmp_w = para->src_rect.h;
		tmp_h = para->src_rect.w;
	} else {
		tmp_w = para->src_rect.w;
		tmp_h = para->src_rect.h;
	}


#if 1
	/* check the parameter valid */
	if (((para->src_rect.x < 0) &&
	     ((-para->src_rect.x) > para->src_rect.w)) ||
	    ((para->src_rect.y < 0) &&
	     ((-para->src_rect.y) > para->src_rect.h)) ||
	    ((para->dst_x < 0) &&
	     ((-para->dst_x) > tmp_w)) ||
	    ((para->dst_y < 0) &&
	     ((-para->dst_y) > tmp_h)) ||
	    ((para->src_rect.x > 0) &&
	     (para->src_rect.x > para->src_image.w - 1)) ||
	    ((para->src_rect.y > 0) &&
	     (para->src_rect.y > para->src_image.h - 1)) ||
	    ((para->dst_x > 0) &&
	     (para->dst_x > para->dst_image.w - 1)) ||
	    ((para->dst_y > 0) && (para->dst_y > para->dst_image.h - 1))) {
		pr_warn("invalid blit parameter setting");
		return -22;
	}
#endif

	if (((para->src_rect.x < 0) &&
				((-para->src_rect.x) < para->src_rect.w))) {
		para->src_rect.w = para->src_rect.w + para->src_rect.x;
		para->src_rect.x = 0;
	} else if ((para->src_rect.x + para->src_rect.w)
			> para->src_image.w) {
		para->src_rect.w = para->src_image.w - para->src_rect.x;
	}
	if (((para->src_rect.y < 0) &&
				((-para->src_rect.y) < para->src_rect.h))) {
		para->src_rect.h = para->src_rect.h + para->src_rect.y;
		para->src_rect.y = 0;
	} else if ((para->src_rect.y + para->src_rect.h)
			> para->src_image.h) {
		para->src_rect.h = para->src_image.h - para->src_rect.y;
	}

	if (((para->dst_x < 0) && ((-para->dst_x) < tmp_w))) {
		para->src_rect.w = tmp_w + para->dst_x;
		para->src_rect.x = (-para->dst_x);
		para->dst_x = 0;
	} else if ((para->dst_x + tmp_w) > para->dst_image.w) {
		para->src_rect.w = para->dst_image.w - para->dst_x;
	}
	if (((para->dst_y < 0) && ((-para->dst_y) < tmp_h))) {
		para->src_rect.h = tmp_h + para->dst_y;
		para->src_rect.y = (-para->dst_y);
		para->dst_y = 0;
	} else if ((para->dst_y + tmp_h) > para->dst_image.h)
		para->src_rect.h = para->dst_image.h - para->dst_y;

	g2d_ext_hd_finish_flag = 0;

	/* Add support inverted order copy, however,
	 * hardware have a bug when reciving y coordinate,
	 * it use (y + height) rather than (y) on inverted
	 * order mode, so here adjust it before pass it to hardware.
	 */
//	mutex_lock(&global_lock);
	if (scan_order > G2D_SM_TDRL)
		para->dst_y += para->src_rect.h;
//	mutex_unlock(&global_lock);

	err = mixer_blt(para, scan_order);

	return err;
}

int g2d_stretchblit(g2d_stretchblt *para)
{
	int32_t err = 0;

	/* check the parameter valid */
	if (((para->src_rect.x < 0) &&
	     ((-para->src_rect.x) > para->src_rect.w)) ||
	    ((para->src_rect.y < 0) &&
	     ((-para->src_rect.y) > para->src_rect.h)) ||
	    ((para->dst_rect.x < 0) &&
	     ((-para->dst_rect.x) > para->dst_rect.w)) ||
	    ((para->dst_rect.y < 0) &&
	     ((-para->dst_rect.y) > para->dst_rect.h)) ||
	    ((para->src_rect.x > 0) &&
	     (para->src_rect.x > para->src_image.w - 1)) ||
	    ((para->src_rect.y > 0) &&
	     (para->src_rect.y > para->src_image.h - 1)) ||
	    ((para->dst_rect.x > 0) &&
	     (para->dst_rect.x > para->dst_image.w - 1)) ||
	    ((para->dst_rect.y > 0) &&
	     (para->dst_rect.y > para->dst_image.h - 1))) {
		pr_warn("invalid stretchblit parameter setting");
		return -22;
	}
	if (((para->src_rect.x < 0) &&
				((-para->src_rect.x) < para->src_rect.w))) {
		para->src_rect.w = para->src_rect.w + para->src_rect.x;
		para->src_rect.x = 0;
	} else if ((para->src_rect.x + para->src_rect.w)
			> para->src_image.w) {
		para->src_rect.w = para->src_image.w - para->src_rect.x;
	}
	if (((para->src_rect.y < 0) &&
				((-para->src_rect.y) < para->src_rect.h))) {
		para->src_rect.h = para->src_rect.h + para->src_rect.y;
		para->src_rect.y = 0;
	} else if ((para->src_rect.y + para->src_rect.h)
			> para->src_image.h) {
		para->src_rect.h = para->src_image.h - para->src_rect.y;
	}

	if (((para->dst_rect.x < 0) &&
				((-para->dst_rect.x) < para->dst_rect.w))) {
		para->dst_rect.w = para->dst_rect.w + para->dst_rect.x;
		para->dst_rect.x = 0;
	} else if ((para->dst_rect.x + para->dst_rect.w)
			> para->dst_image.w) {
		para->dst_rect.w = para->dst_image.w - para->dst_rect.x;
	}
	if (((para->dst_rect.y < 0) &&
				((-para->dst_rect.y) < para->dst_rect.h))) {
		para->dst_rect.h = para->dst_rect.h + para->dst_rect.y;
		para->dst_rect.y = 0;
	} else if ((para->dst_rect.y + para->dst_rect.h)
			> para->dst_image.h) {
		para->dst_rect.h = para->dst_image.h - para->dst_rect.y;
	}

	g2d_ext_hd_finish_flag = 0;

	/* Add support inverted order copy, however,
	 * hardware have a bug when reciving y coordinate,
	 * it use (y + height) rather than (y) on inverted
	 * order mode, so here adjust it before pass it to hardware.
	 */

//	mutex_lock(&global_lock);
	if (scan_order > G2D_SM_TDRL)
		para->dst_rect.y += para->src_rect.h;
//	mutex_unlock(&global_lock);

	err = mixer_stretchblt(para, scan_order);

	return err;
}

/*
 * cmd-test.c
 */
static g2d_fillrect   G2D_FILLRECT;
static g2d_blt        G2D_BLT;
static g2d_stretchblt G2D_STRETCHBLT;

static void drawrect(void * dst, uint32_t c)
{
	G2D_FILLRECT.flag = G2D_FIL_NONE; //G2D_FIL_PIXEL_ALPHA; //G2D_FIL_PLANE_ALPHA;

	G2D_FILLRECT.dst_image.addr[0] = (uint32_t)dst;
	G2D_FILLRECT.dst_image.addr[1] = (uint32_t)dst;
	G2D_FILLRECT.dst_image.addr[2] = (uint32_t)dst;
	G2D_FILLRECT.dst_image.w = LCD_PIXEL_WIDTH;
	G2D_FILLRECT.dst_image.h = LCD_PIXEL_HEIGHT;
	G2D_FILLRECT.dst_image.format = G2D_FMT_XBGR8888;
	G2D_FILLRECT.dst_image.pixel_seq = G2D_SEQ_NORMAL;

	G2D_FILLRECT.dst_rect.x = 0;
	G2D_FILLRECT.dst_rect.y = 0;

	G2D_FILLRECT.dst_rect.w = DIM_X;
	G2D_FILLRECT.dst_rect.h = DIM_Y;

	G2D_FILLRECT.color = c;
	G2D_FILLRECT.alpha = 0x0;

	g2d_fill(&G2D_FILLRECT);
}

static void drawpng(void * dst, int width, int height, void * data, int x, int y)
{

	G2D_BLT.flag = G2D_BLT_NONE | 0 * G2D_BLT_PLANE_ALPHA;

	G2D_BLT.src_image.addr[0] = (uintptr_t)data;
	G2D_BLT.src_image.addr[1] = 0;    //(uintptr_t)png->data;	// was index=0
	G2D_BLT.src_image.addr[2] = 0;    //(uintptr_t)png->data;	// was index=0
	G2D_BLT.src_image.w = width;
	G2D_BLT.src_image.h = height;
	G2D_BLT.src_image.format = G2D_FMT_XBGR8888;
	G2D_BLT.src_image.pixel_seq = G2D_SEQ_NORMAL;

	G2D_BLT.src_rect.x = 0;
	G2D_BLT.src_rect.y = 0;

	G2D_BLT.src_rect.w = width;
	G2D_BLT.src_rect.h = height;

	G2D_BLT.dst_image.addr[0] = (uint32_t)dst;
	G2D_BLT.dst_image.addr[1] = 0;               //memory;	// was index=0
	G2D_BLT.dst_image.addr[2] = 0;               //memory;	// was index=0
	G2D_BLT.dst_image.w = LCD_PIXEL_WIDTH;
	G2D_BLT.dst_image.h = LCD_PIXEL_HEIGHT;
	G2D_BLT.dst_image.format = G2D_FMT_XBGR8888;
	G2D_BLT.dst_image.pixel_seq = G2D_SEQ_NORMAL;

	G2D_BLT.dst_x = x;
	G2D_BLT.dst_y = y;

	G2D_BLT.color = 0x00000000;
	G2D_BLT.alpha = 0xFF;

	g2d_blit(&G2D_BLT);
}

static int random_int(int a, int b)
{
	return (int)((b - a) * (double)rand() / (double)RAND_MAX + a);
}

static void drawfunc1(struct window_t * w, void * o)
{
	struct surface_t * s = w->s;
	drawrect(s->pixels, 0x00ff0000);
	dma_cache_sync(s->pixels, s->pixlen, DMA_FROM_DEVICE);
}

static void drawfunc2(struct window_t * w, void * o)
{
	struct surface_t * s = w->s;
	drawrect(s->pixels, 0x000000ff);
	dma_cache_sync(s->pixels, s->pixlen, DMA_FROM_DEVICE);
	struct color_t c;
	color_init(&c, 0x33, 0x99, 0xcc, 0xff);
	surface_fill(s, NULL, NULL, 400, 240, &c);
}

static void drawfunc3(struct window_t * w, void * o)
{
	struct surface_t * s = w->s;
	struct surface_t * logo = (struct surface_t *)o;
	int x = random_int(0 + 100, 800 - 100);
	int y = random_int(0 + 100, 480 - 100);

	dma_cache_sync(logo->pixels, logo->pixlen, DMA_TO_DEVICE);
	drawpng(s->pixels, surface_get_width(logo), surface_get_height(logo), surface_get_pixels(logo), x, y);
}

void test_g2d_new(void)
{
	struct window_t * w = window_alloc(NULL, NULL);
	struct xfs_context_t * ctx = xfs_alloc("/private/framework", 0);
	struct surface_t * logo = surface_alloc_from_xfs(ctx, "assets/images/logo.png");

	for(int i = 0; i < 10; i++)
	{
		window_present(w, logo, drawfunc1);
		msleep(500);
		window_present(w, logo, drawfunc2);
		msleep(500);
		window_present(w, logo, drawfunc3);
		msleep(500);
	}

	surface_free(logo);
	xfs_free(ctx);
	window_free(w);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct g2d_t113_pdata_t {
	virtual_addr_t virt;
	char * clk;
	int reset;
};

static void g2d_init(struct g2d_t * g2d)
{
	g2d_bsp_open();
}

static inline void blend(uint32_t * d, uint32_t * s)
{
	uint32_t dv, sv = *s;
	uint8_t da, dr, dg, db;
	uint8_t sa, sr, sg, sb;
	uint8_t a, r, g, b;
	int t;

	sa = (sv >> 24) & 0xff;
	if(sa == 255)
	{
		*d = sv;
	}
	else if(sa != 0)
	{
		sr = (sv >> 16) & 0xff;
		sg = (sv >> 8) & 0xff;
		sb = (sv >> 0) & 0xff;
		dv = *d;
		da = (dv >> 24) & 0xff;
		dr = (dv >> 16) & 0xff;
		dg = (dv >> 8) & 0xff;
		db = (dv >> 0) & 0xff;
		t = sa + (sa >> 8);
		a = (((sa + da) << 8) - da * t) >> 8;
		r = (((sr + dr) << 8) - dr * t) >> 8;
		g = (((sg + dg) << 8) - dg * t) >> 8;
		b = (((sb + db) << 8) - db * t) >> 8;
		*d = (a << 24) | (r << 16) | (g << 8) | (b << 0);
	}
}

static inline bool_t sw_blit(struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * o)
{
	struct region_t r, region;
	struct matrix_t t;
	uint32_t * p;
	uint32_t * dp = surface_get_pixels(s);
	uint32_t * sp = surface_get_pixels(o);
	int ds = surface_get_stride(s) >> 2;
	int ss = surface_get_stride(o) >> 2;
	int sw = surface_get_width(o);
	int sh = surface_get_height(o);
	int x1, y1, x2, y2, stride;
	int x, y, ox, oy;
	double fx, fy, ofx, ofy;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, sw, sh, &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	stride = ds - r.w;
	p = dp + y1 * ds + x1;
	fx = x1;
	fy = y1;
	memcpy(&t, m, sizeof(struct matrix_t));
	matrix_invert(&t);
	matrix_transform_point(&t, &fx, &fy);

	for(y = y1; y < y2; ++y, fx += t.c, fy += t.d)
	{
		ofx = fx;
		ofy = fy;
		for(x = x1; x < x2; ++x, ofx += t.a, ofy += t.b)
		{
			ox = (int)ofx;
			oy = (int)ofy;
			if(ox >= 0 && ox < sw && oy >= 0 && oy < sh)
			{
				blend(p, sp + oy * ss + ox);
			}
			p++;
		}
		p += stride;
	}
	return TRUE;
}

static inline bool_t sw_fill(struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	struct region_t r, region;
	struct matrix_t t;
	uint32_t * p, v;
	int ds = surface_get_stride(s) >> 2;
	int x1, y1, x2, y2, stride;
	int x, y, ox, oy;
	double fx, fy, ofx, ofy;

	region_init(&r, 0, 0, surface_get_width(s), surface_get_height(s));
	if(clip)
	{
		if(!region_intersect(&r, &r, clip))
			return TRUE;
	}
	matrix_transform_region(m, w, h, &region);
	if(!region_intersect(&r, &r, &region))
		return TRUE;

	x1 = r.x;
	y1 = r.y;
	x2 = r.x + r.w;
	y2 = r.y + r.h;
	stride = ds - r.w;
	p = (uint32_t *)surface_get_pixels(s) + y1 * ds + x1;
	v = color_get_premult(c);
	fx = x1;
	fy = y1;
	memcpy(&t, m, sizeof(struct matrix_t));
	matrix_invert(&t);
	matrix_transform_point(&t, &fx, &fy);

	for(y = y1; y < y2; ++y, fx += t.c, fy += t.d)
	{
		ofx = fx;
		ofy = fy;
		for(x = x1; x < x2; ++x, ofx += t.a, ofy += t.b)
		{
			ox = (int)ofx;
			oy = (int)ofy;
			if(ox >= 0 && ox < w && oy >= 0 && oy < h)
				*p = v;
			p++;
		}
		p += stride;
	}
	return TRUE;
}

static bool_t g2d_t113_blit(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, struct surface_t * o)
{
	//struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;
	//return sw_blit(s, clip, m, o);
	return FALSE;
}

static bool_t g2d_t113_fill(struct g2d_t * g2d, struct surface_t * s, struct region_t * clip, struct matrix_t * m, int w, int h, struct color_t * c)
{
	//struct g2d_t113_pdata_t * pdat = (struct g2d_t113_pdata_t *)g2d->priv;
	//return sw_fill(s, clip, m, w, h, c);
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

	g2d->name = alloc_device_name(dt_read_name(n), -1);
	g2d->blit = g2d_t113_blit;
	g2d->fill = g2d_t113_fill;
	g2d->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	g2d_init(g2d);

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
