#ifndef __S5P6818_FB_H__
#define __S5P6818_FB_H__

#include <xboot.h>
#include <fb/fb.h>
#include <dma/dma.h>
#include <led/led.h>
#include <s5p6818-rstcon.h>
#include <s5p6818-gpio.h>
#include <s5p6818/reg-mlc.h>
#include <s5p6818/reg-dpc.h>
#include <s5p6818/reg-dpa.h>

enum s5p6818_mlc_rgbfmt_t
{
	S5P6818_MLC_RGBFMT_R5G6B5		= 0x4432,
	S5P6818_MLC_RGBFMT_B5G6R5		= 0xC432,

	S5P6818_MLC_RGBFMT_X1R5G5B5		= 0x4342,
	S5P6818_MLC_RGBFMT_X1B5G5R5		= 0xC342,
	S5P6818_MLC_RGBFMT_X4R4G4B4		= 0x4211,
	S5P6818_MLC_RGBFMT_X4B4G4R4		= 0xC211,
	S5P6818_MLC_RGBFMT_X8R3G3B2		= 0x4120,
	S5P6818_MLC_RGBFMT_X8B3G3R2		= 0xC120,

	S5P6818_MLC_RGBFMT_A1R5G5B5		= 0x3342,
	S5P6818_MLC_RGBFMT_A1B5G5R5		= 0xB342,
	S5P6818_MLC_RGBFMT_A4R4G4B4		= 0x2211,
	S5P6818_MLC_RGBFMT_A4B4G4R4		= 0xA211,
	S5P6818_MLC_RGBFMT_A8R3G3B2		= 0x1120,
	S5P6818_MLC_RGBFMT_A8B3G3R2		= 0x9120,

	S5P6818_MLC_RGBFMT_R8G8B8		= 0x4653,
	S5P6818_MLC_RGBFMT_B8G8R8		= 0xC653,

	S5P6818_MLC_RGBFMT_X8R8G8B8		= 0x4653,
	S5P6818_MLC_RGBFMT_X8B8G8R8		= 0xC653,
	S5P6818_MLC_RGBFMT_A8R8G8B8		= 0x0653,
	S5P6818_MLC_RGBFMT_A8B8G8R8		= 0x8653,
};

enum s5p6818_dpc_format_t
{
	S5P6818_DPC_FORMAT_RGB555		= 0x0,
	S5P6818_DPC_FORMAT_RGB565		= 0x1,
	S5P6818_DPC_FORMAT_RGB666		= 0x2,
	S5P6818_DPC_FORMAT_RGB888		= 0x3,
	S5P6818_DPC_FORMAT_MRGB555A		= 0x4,
	S5P6818_DPC_FORMAT_MRGB555B		= 0x5,
	S5P6818_DPC_FORMAT_MRGB565		= 0x6,
	S5P6818_DPC_FORMAT_MRGB666		= 0x7,
	S5P6818_DPC_FORMAT_MRGB888A		= 0x8,
	S5P6818_DPC_FORMAT_MRGB888B		= 0x9,
	S5P6818_DPC_FORMAT_CCIR656		= 0xa,
	S5P6818_DPC_FORMAT_CCIR601A		= 0xc,
	S5P6818_DPC_FORMAT_CCIR601B		= 0xd,
};

enum s5p6818_dpc_ycorder_t
{
	S5P6818_DPC_YCORDER_CBYCRY		= 0x0,
	S5P6818_DPC_YCORDER_CRYCBY		= 0x1,
	S5P6818_DPC_YCORDER_YCBYCR		= 0x2,
	S5P6818_DPC_YCORDER_YCRYCB		= 0x3,
};

enum s5p6818_dpc_dither_t
{
	S5P6818_DPC_DITHER_BYPASS		= 0x0,
	S5P6818_DPC_DITHER_4BIT			= 0x1,
	S5P6818_DPC_DITHER_5BIT			= 0x2,
	S5P6818_DPC_DITHER_6BIT			= 0x3,
};

struct s5p6818_fb_data_t
{
	/* Register base address */
	physical_addr_t physmlc;
	physical_addr_t physdpc;

	/* Horizontal resolution */
	int width;

	/* Vertical resolution */
	int height;

	/* Horizontal dpi */
	int xdpi;

	/* Vertical dpi */
	int ydpi;

	/* Bits per pixel */
	int bits_per_pixel;

	/* Bytes per pixel */
	int bytes_per_pixel;

	struct {
		/* Specifies output pixel format, YCbCR or RGB */
		int rgbmode;

		/* Scan mode, progressive or interlace */
		int scanmode;

		/* Enbed sync signal for ITU-R BT.656 format */
		int enbedsync;

		/* The polarity of the internal field signal */
		int polfield;

		/* Swap red and blue component */
		int swaprb;

		/* Data out format */
		enum s5p6818_dpc_format_t format;

		/* Data out order */
		enum s5p6818_dpc_ycorder_t ycorder;

		/* Dithering mode */
		enum s5p6818_dpc_dither_t rdither;
		enum s5p6818_dpc_dither_t gdither;
		enum s5p6818_dpc_dither_t bdither;
	} mode;

	struct {
		/* Pixel clock frequency */
		int pixel_clock_hz;

		/* Horizontal front porch */
		int	h_front_porch;

		/* Horizontal sync width */
		int	h_sync_width;

		/* Horizontal back porch */
		int	h_back_porch;

		/* Vertical front porch */
		int	v_front_porch;

		/* Vertical sync width */
		int	v_sync_width;

		/* Vertical back porch */
		int	v_back_porch;

		/* Start veritcal sync offset */
		int vs_start_offset;

		/* End veritcal sync offset */
		int vs_end_offset;

		/* Start even veritcal sync offset */
		int ev_start_offset;

		/* End even veritcal sync offset */
		int ev_end_offset;

		/* The delay value for RGB/PVD signal */
		int d_rgb_pvd;

		/* The delay value for HSYNC/CP1 signal */
		int d_hsync_cp1;

		/* The delay value for VSYNC/FRAM signal */
		int d_vsync_fram;

		/* The delay value for DE/CP2 signal */
		int d_de_cp2;

		/* Hsync polarity flag */
		int	h_sync_invert;

		/* Vsync polarity flag */
		int	v_sync_invert;
	} timing;

	/* Backlight led */
	char * backlight;
};

#endif /* __S5P6818_FB_H__ */
