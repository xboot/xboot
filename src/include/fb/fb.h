#ifndef __FB_H__
#define __FB_H__

#include <configs.h>
#include <default.h>
#include <fb/bitmap.h>

//TODO
#if 0
/*
 * the point struct
 */
struct point
{
	x_s32 x, y;
};

/*
 * the rect struct
 */
struct rect
{
	x_s32 x0, y0;
	x_s32 x1, y1;
};

/*
 * rgb format
 */
enum format_rgb {
	FORMAT_RGB_8888,
	FORMAT_RGB_888,
	FORMAT_RGB_565,
	FORMAT_RGB_555,
	FORMAT_RGB_332
};
#endif

/**
 * defined the structure of framebuffer information.
 */
struct fb_info
{
	/* the framebuffer name. */
	const char * name;

	struct bitmap bitmap;
};

/*
 * defined the structure of framebuffer.
 */
struct fb
{
	/* the framebuffer information */
	struct fb_info * info;

	/* initialize the framebuffer */
	void (*init)(void);

	/* clean up the framebuffer */
	void (*exit)(void);

	/* set back light's brightness */
	void (*bl)(x_u8 brightness);

	/* set pixel */
	void (*set_pixel)(x_u32 x, x_u32 y, x_u32 c);

	/* set pixel */
	x_u32 (*get_pixel)(x_u32 x, x_u32 y);

	/* hline */
	void (*hline)(x_u32 x0, x_u32 y0, x_u32 x, x_u32 c);

	/* vline */
	void (*vline)(x_u32 x0, x_u32 y0, x_u32 y, x_u32 c);

	/* ioctl framebuffer */
	x_s32 (*ioctl)(x_u32 cmd, void * arg);
};

struct fb * search_framebuffer(const char * name);
x_bool register_framebuffer(struct fb * fb);
x_bool unregister_framebuffer(struct fb * fb);


#endif /* __FB_H__ */
