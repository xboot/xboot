#ifndef __S5PV210_FB_H__
#define __S5PV210_FB_H__

#include <configs.h>
#include <default.h>

enum s5pv210fb_output
{
	OUTPUT_RGB,
	OUTPUT_ITU,
	OUTPUT_I80LDI0,
	OUTPUT_I80LDI1,
	OUTPUT_WB_RGB,
	OUTPUT_WB_I80LDI0,
	OUTPUT_WB_I80LDI1,
};

enum s5pv210fb_rgb_mode
{
	MODE_RGB_P = 0,
	MODE_BGR_P = 1,
	MODE_RGB_S = 2,
	MODE_BGR_S = 3,
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
	enum s5pv210fb_rgb_mode mode;

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
