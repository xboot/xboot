#ifndef __SANDBOX_FB_H__
#define __SANDBOX_FB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>
#include <sandbox.h>

struct sandbox_fb_data_t
{
	int width;
	int height;
	int xdpi;
	int ydpi;
	int fullscreen;
	void * priv;
};

#ifdef __cplusplus
}
#endif

#endif /* __SANDBOX_FB_H__ */
