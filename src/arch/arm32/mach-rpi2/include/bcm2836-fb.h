#ifndef __BCM2836_FB_H__
#define __BCM2836_FB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>
#include <bcm2836-mbox.h>

struct bcm2836_fb_data_t
{
	int width;
	int height;
	int xdpi;
	int ydpi;
	int bpp;
};

#ifdef __cplusplus
}
#endif

#endif /* __BCM2836_FB_H__ */
