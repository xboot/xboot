#ifndef __REALVIEW_FB_H__
#define __REALVIEW_FB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <mmu.h>
#include <fb/fb.h>
#include <realview/reg-lcd.h>

struct realview_fb_data_t
{
	/* register base address */
	physical_addr_t regbase;

	/* horizontal resolution */
	s32_t width;

	/* vertical resolution */
	s32_t height;

	/* horizontal dpi */
	s32_t xdpi;

	/* vertical dpi */
	s32_t ydpi;

	/* bits per pixel */
	s32_t bits_per_pixel;

	/* bytes per pixel */
	s32_t bytes_per_pixel;

	struct {
		/* horizontal front porch */
		s32_t h_fp;

		/* horizontal back porch */
		s32_t h_bp;

		/* horizontal sync width */
		s32_t h_sw;

		/* vertical front porch */
		s32_t v_fp;

		/* vertical back porch */
		s32_t v_bp;

		/* vertical sync width */
		s32_t v_sw;
	} timing;

	/* init */
	void (*init)(struct realview_fb_data_t * dat);

	/* exit */
	void (*exit)(struct realview_fb_data_t * dat);

	/* backlight brightness */
	int brightness;
	void (*set_backlight)(struct realview_fb_data_t * dat, int brightness);
	int (*get_backlight)(struct realview_fb_data_t * dat);
};

#ifdef __cplusplus
}
#endif

#endif /* __REALVIEW_FB_H__ */
