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

struct render_page
{
	/* bitmap information description */
	struct bitmap_info info;

	/* render page's view port */
	struct {
		x_u32 x;
		x_u32 y;
		x_u32 w;
		x_u32 h;
	} viewport;

	/* indicates data's memory type */
	x_bool flag;

	/* pointer to data, video card memory or system memory */
	void * data;

	/* link other render page */
	struct list_head entry;
};

/*
 * defined the structure of framebuffer information.
 */
struct fb_info
{
	/* the framebuffer name. */
	const char * name;

	/* bitmap information description for framebuffer */
	struct bitmap_info info;

	/* active render page */
	struct render_page * active;

	/* render page list */
	struct render_page render;
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

	/* set view port */
	x_u32 (*set_viewport)(struct render_page * render, x_u32 x, x_u32 y, x_u32 w, x_u32 h);

	/* get view port */
	x_u32 (*get_viewport)(struct render_page * render, x_u32 * x, x_u32 * y, x_u32 * w, x_u32 * h);

	/* map color */
	x_u32 (*map_color)(struct render_page * render, x_u8 r, x_u8 g, x_u8 b, x_u8 a);

	/* unmap color */
	x_bool (*unmap_color)(struct render_page * render, x_u32 c, x_u8 * r, x_u8 * g, x_u8 * b, x_u8 * a);

	/* fill rect */
	x_bool (*fill_rect)(struct render_page * render, x_u32 c, x_u32 x, x_u32 y, x_u32 w, x_u32 h);

	/* blit bitmap */
	x_bool (*blit_bitmap)(struct bitmap * bitmap, struct render_page * render, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 ox, x_u32 oy, x_u32 w, x_u32 h);

	/* blit render */
	x_bool (*blit_render)(struct render_page * src, struct render_page * dst, enum blit_mode mode, x_u32 x, x_u32 y, x_u32 ox, x_u32 oy, x_u32 w, x_u32 h);

	/* scroll */
	x_bool (*scroll)(struct render_page * render, x_u32 c, x_s32 dx, x_s32 dy);

	/* create render */
	x_bool (*create_render)(struct render_page * render, x_u32 w, x_u32 h);

	/* delete render */
	x_bool (*delete_render)(struct render_page * render);

	/* set active render */
	x_bool (*set_active)(struct render_page * render);

	/* get active render */
	x_bool (*get_active)(struct render_page * render);

	/* ioctl framebuffer */
	x_s32 (*ioctl)(x_u32 cmd, void * arg);
};


struct fb * search_framebuffer(const char * name);
x_bool register_framebuffer(struct fb * fb);
x_bool unregister_framebuffer(struct fb * fb);

#endif /* __FB_H__ */
