#ifndef __EXYNOS4412_FB_H__
#define __EXYNOS4412_FB_H__

#include <xboot.h>
#include <mmu.h>
#include <fb/fb.h>
#include <exynos4412/reg-gpio.h>
#include <exynos4412/reg-others.h>
#include <exynos4412/reg-lcd.h>

enum exynos4412_fb_output_t
{
	EXYNOS4412_FB_OUTPUT_RGB,
	EXYNOS4412_FB_OUTPUT_I80LDI0,
	EXYNOS4412_FB_OUTPUT_I80LDI1,
	EXYNOS4412_FB_OUTPUT_WB_RGB,
	EXYNOS4412_FB_OUTPUT_WB_I80LDI0,
	EXYNOS4412_FB_OUTPUT_WB_I80LDI1,
};

enum exynos4412_fb_rgb_mode_t
{
	EXYNOS4412_FB_MODE_RGB_P 	= 0,
	EXYNOS4412_FB_MODE_BGR_P 	= 1,
	EXYNOS4412_FB_MODE_RGB_S 	= 2,
	EXYNOS4412_FB_MODE_BGR_S 	= 3,
};

enum exynos4412_bpp_mode_t
{
	EXYNOS4412_FB_BPP_MODE_1BPP			= 0x0,
	EXYNOS4412_FB_BPP_MODE_2BPP			= 0x1,
	EXYNOS4412_FB_BPP_MODE_4BPP			= 0x2,
	EXYNOS4412_FB_BPP_MODE_8BPP_PAL		= 0x3,
	EXYNOS4412_FB_BPP_MODE_8BPP			= 0x4,
	EXYNOS4412_FB_BPP_MODE_16BPP_565	= 0x5,
	EXYNOS4412_FB_BPP_MODE_16BPP_A555	= 0x6,
	EXYNOS4412_FB_BPP_MODE_16BPP_X555	= 0x7,
	EXYNOS4412_FB_BPP_MODE_18BPP_666	= 0x8,
	EXYNOS4412_FB_BPP_MODE_18BPP_A665	= 0x9,
	EXYNOS4412_FB_BPP_MODE_19BPP_A666	= 0xa,
	EXYNOS4412_FB_BPP_MODE_24BPP_888	= 0xb,
	EXYNOS4412_FB_BPP_MODE_24BPP_A887	= 0xc,
	EXYNOS4412_FB_BPP_MODE_32BPP		= 0xd,
	EXYNOS4412_FB_BPP_MODE_16BPP_A444	= 0xe,
	EXYNOS4412_FB_BPP_MODE_15BPP_555	= 0xf,
};

enum {
	EXYNOS4412_FB_SWAP_WORD		= (0x1 << 0),
	EXYNOS4412_FB_SWAP_HWORD	= (0x1 << 1),
	EXYNOS4412_FB_SWAP_BYTE		= (0x1 << 2),
	EXYNOS4412_FB_SWAP_BIT		= (0x1 << 3),
};

struct exynos4412_fb_data_t
{
	/* Register base address */
	physical_addr_t regbase;

	/* Horizontal resolution */
	s32_t width;

	/* Vertical resolution */
	s32_t height;

	/* Horizontal dpi */
	s32_t xdpi;

	/* Vertical dpi */
	s32_t ydpi;

	/* Bits per pixel */
	s32_t bits_per_pixel;

	/* Bytes per pixel */
	s32_t bytes_per_pixel;

	/* Vframe frequency */
	s32_t freq;

	/* output path */
	enum exynos4412_fb_output_t output;

	/* RGB mode */
	enum exynos4412_fb_rgb_mode_t rgb_mode;

	/* bpp mode */
	enum exynos4412_bpp_mode_t bpp_mode;

	/* Swap flag */
	u32_t swap;

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

	/* init */
	void (*init)(struct exynos4412_fb_data_t * dat);
	/* exit */
	void (*exit)(struct exynos4412_fb_data_t * dat);

	/* backlight brightness */
	int brightness;
	/* set backlight brightness */
	void (*setbl)(struct exynos4412_fb_data_t * dat, int brightness);
	/* get backlight brightness */
	int (*getbl)(struct exynos4412_fb_data_t * dat);
};

#endif /* __EXYNOS4412_FB_H__ */
