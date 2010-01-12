#ifndef __FB_H__
#define __FB_H__

#include <configs.h>
#include <default.h>
#include <terminal/terminal.h>

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

/**
 * defined the struct of framebuffer information.
 */
struct fb_info
{
	/* the framebuffer name. */
	const char * name;

    /* the width of the framebuffer */
    x_u16 width;

    /* the height of the framebuffer */
    x_u16 height;

    /* detailed format of rgb */
    enum format_rgb format;

	/* 1, 2, 4, 8, 16, or 32 bits per pixel */
    x_u16 bpp;

    /* number of bytes per line (byte) */
    x_u16 stride;

    /* current seek position */
    x_u32 pos;

    /* base address of the framebuffer, or 0 if not directly accessible */
    void * base;
};

/*
 * defined the struct of framebuffer.
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
	x_s32 (*ioctl)(x_u32 cmd, x_u32 arg);
};

struct fb * search_framebuffer(const char * name);
x_bool register_framebuffer(struct fb * fb);
x_bool unregister_framebuffer(struct fb * fb);


#endif /* __FB_H__ */
