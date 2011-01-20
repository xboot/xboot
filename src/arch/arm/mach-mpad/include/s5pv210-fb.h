#ifndef __S5PV210_FB_H__
#define __S5PV210_FB_H__

#include <configs.h>
#include <default.h>


enum s5pv210fb_output
{
	S5PV210FB_OUTPUT_RGB,
	S5PV210FB_OUTPUT_ITU,
	S5PV210FB_OUTPUT_I80LDI0,
	S5PV210FB_OUTPUT_I80LDI1,
	S5PV210FB_OUTPUT_WB_RGB,
	S5PV210FB_OUTPUT_WB_I80LDI0,
	S5PV210FB_OUTPUT_WB_I80LDI1,
};

enum s5pv210fb_rgb_mode
{
	S5PV210FB_MODE_RGB_P 			= 0,
	S5PV210FB_MODE_BGR_P 			= 1,
	S5PV210FB_MODE_RGB_S 			= 2,
	S5PV210FB_MODE_BGR_S 			= 3,
};

enum s5pv210_bpp_mode
{
	S5PV210FB_BPP_MODE_1BPP			= 0x0,
	S5PV210FB_BPP_MODE_2BPP			= 0x1,
	S5PV210FB_BPP_MODE_4BPP			= 0x2,
	S5PV210FB_BPP_MODE_8BPP_PAL		= 0x3,
	S5PV210FB_BPP_MODE_8BPP			= 0x4,
	S5PV210FB_BPP_MODE_16BPP_565	= 0x5,
	S5PV210FB_BPP_MODE_16BPP_A555	= 0x6,
	S5PV210FB_BPP_MODE_18BPP_666	= 0x8,
	S5PV210FB_BPP_MODE_18BPP_A665	= 0x9,
	S5PV210FB_BPP_MODE_24BPP_888	= 0xb,
	S5PV210FB_BPP_MODE_24BPP_A887	= 0xc,
	S5PV210FB_BPP_MODE_32BPP		= 0xd,
	S5PV210FB_BPP_MODE_16BPP_A444	= 0xe,
	S5PV210FB_BPP_MODE_15BPP_555	= 0xf,
};

enum {
	S5PV210FB_SWAP_WORD				= (0x1 << 0),
	S5PV210FB_SWAP_HWORD			= (0x1 << 1),
	S5PV210FB_SWAP_BYTE				= (0x1 << 2),
	S5PV210FB_SWAP_BIT				= (0x1 << 3),
};

struct s5pv210fb_lcd
{
	/* horizontal resolution */
	x_s32 width;

	/* vertical resolution */
	x_s32 height;

	/* bits per pixel */
	x_s32 bpp;

	/* vframe frequency */
	x_s32 freq;

	/* output path */
	enum s5pv210fb_output output;

	/* rgb mode */
	enum s5pv210fb_rgb_mode rgb_mode;

	/* bpp mode */
	enum s5pv210_bpp_mode bpp_mode;

	/* swap flag */
	x_u32 swap;

	struct {
		/* red color */
		x_s32 r_mask;
		x_s32 r_field;

		/* green color */
		x_s32 g_mask;
		x_s32 g_field;

		/* blue color */
		x_s32 b_mask;
		x_s32 b_field;

		/* alpha color */
		x_s32 a_mask;
		x_s32 a_field;
	} rgba;

	struct {
		/* horizontal front porch */
		x_s32 h_fp;

		/* horizontal back porch */
		x_s32 h_bp;

		/* horizontal sync width */
		x_s32 h_sw;

		/* vertical front porch */
		x_s32 v_fp;

		/* vertical front porch for even field */
		x_s32 v_fpe;

		/* vertical back porch */
		x_s32 v_bp;

		/* vertical back porch for even field */
		x_s32 v_bpe;

		/* vertical sync width */
		x_s32 v_sw;
	} timing;

	struct {
		/* if 1, video data is fetched at rising edge */
		x_s32 rise_vclk;

		/* if HSYNC polarity is inversed */
		x_s32 inv_hsync;

		/* if VSYNC polarity is inversed */
		x_s32 inv_vsync;

		/* if VDEN polarity is inversed */
		x_s32 inv_vden;
	} polarity;

	/* video ram buffer */
	void * vram;

	/* lcd init */
	void (*init)(void);

	/* lcd exit */
	void (*exit)(void);

	/* lcd backlight */
	void (*backlight)(x_u8 brightness);
};

#endif /* __S5PV210_FB_H__ */
