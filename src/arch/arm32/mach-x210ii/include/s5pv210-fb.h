#ifndef __S5PV210_FB_H__
#define __S5PV210_FB_H__

#include <xboot.h>

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
	s32_t width;

	/* vertical resolution */
	s32_t height;

	/* bits per pixel */
	s32_t bits_per_pixel;

	/* bytes per pixel */
	s32_t bytes_per_pixel;

	/* vframe frequency */
	s32_t freq;

	/* output path */
	enum s5pv210fb_output output;

	/* rgb mode */
	enum s5pv210fb_rgb_mode rgb_mode;

	/* bpp mode */
	enum s5pv210_bpp_mode bpp_mode;

	/* swap flag */
	u32_t swap;

	struct {
		/* red color */
		s32_t r_mask;
		s32_t r_field;

		/* green color */
		s32_t g_mask;
		s32_t g_field;

		/* blue color */
		s32_t b_mask;
		s32_t b_field;

		/* alpha color */
		s32_t a_mask;
		s32_t a_field;
	} rgba;

	struct {
		/* horizontal front porch */
		s32_t h_fp;

		/* horizontal back porch */
		s32_t h_bp;

		/* horizontal sync width */
		s32_t h_sw;

		/* vertical front porch */
		s32_t v_fp;

		/* vertical front porch for even field */
		s32_t v_fpe;

		/* vertical back porch */
		s32_t v_bp;

		/* vertical back porch for even field */
		s32_t v_bpe;

		/* vertical sync width */
		s32_t v_sw;
	} timing;

	struct {
		/* if 1, video data is fetched at rising edge */
		s32_t rise_vclk;

		/* if HSYNC polarity is inversed */
		s32_t inv_hsync;

		/* if VSYNC polarity is inversed */
		s32_t inv_vsync;

		/* if VDEN polarity is inversed */
		s32_t inv_vden;
	} polarity;

	/* video ram front buffer */
	void * vram_front;

	/* video ram back buffer */
	void * vram_back;

	/* lcd init */
	void (*init)(void);

	/* lcd exit */
	void (*exit)(void);

	/* lcd backlight */
	void (*backlight)(u8_t brightness);
};

#endif /* __S5PV210_FB_H__ */
