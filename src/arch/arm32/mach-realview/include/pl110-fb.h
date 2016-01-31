#ifndef __PL110_FB_H__
#define __PL110_FB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <dma/dma.h>
#include <led/led.h>
#include <fb/fb.h>

struct pl110_fb_data_t
{
	int width;
	int height;
	int xdpi;
	int ydpi;
	int bpp;
	int h_fp;
	int h_bp;
	int h_sw;
	int v_fp;
	int v_bp;
	int v_sw;
	char * backlight;
	physical_addr_t phys;
};

#ifdef __cplusplus
}
#endif

#endif /* __PL110_FB_H__ */
