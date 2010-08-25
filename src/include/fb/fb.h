#ifndef __FB_H__
#define __FB_H__

#include <configs.h>
#include <default.h>
#include <xboot/list.h>
#include <fb/bitmap.h>

/*
 * define blit operator mode
 */
enum blit_mode
{
	BLIT_MODE_REPLACE,
	BLIT_MODE_BLEND
};

/*
 * defined the structure of framebuffer information.
 */
struct fb_info
{
	/* the framebuffer name. */
	const char * name;

	/* bitmap information description for framebuffer */
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

	/* map color */
	x_u32 (*map_color)(struct fb * fb, x_u8 r, x_u8 g, x_u8 b, x_u8 a);

	/* unmap color */
	x_bool (*unmap_color)(struct fb * fb, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);

	/* fill rect */
	x_bool (*fill_rect)(struct fb * fb, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);

	/* blit bitmap */
	x_bool (*blit_bitmap)(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 ox, x_u32 oy, x_u32 w, x_u32 h);

	/* ioctl framebuffer */
	x_s32 (*ioctl)(x_u32 cmd, void * arg);
};


struct fb * search_framebuffer(const char * name);
x_bool register_framebuffer(struct fb * fb);
x_bool unregister_framebuffer(struct fb * fb);

#endif /* __FB_H__ */
