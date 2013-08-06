#ifndef __FB_H__
#define __FB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/color.h>
#include <fb/rect.h>
#include <fb/matrix.h>
#include <fb/render.h>
#include <fb/sw/sw.h>

struct fb_t
{
	/* framebuffer name */
	const char * name;

	/* initialize the framebuffer */
	void (*init)(struct fb_t * fb);

	/* clean up the framebuffer */
	void (*exit)(struct fb_t * fb);

	/* cursor position of x with offset */
	int (*xcursor)(struct fb_t * fb, int ox);

	/* cursor position of y with offset */
	int (*ycursor)(struct fb_t * fb, int oy);

	/* backlight brightness */
	int (*backlight)(struct fb_t * fb, int brightness);

	/* suspend framebuffer */
	void (*suspend)(struct fb_t * fb);

	/* resume framebuffer */
	void (*resume)(struct fb_t * fb);

	/* create a render */
	struct render_t * (*create)(struct fb_t * fb);

	/* destroy a render */
	void (*destroy)(struct fb_t * fb, struct render_t * render);

	/* present a render */
	void (*present)(struct fb_t * fb, struct render_t * render);

	/* alone render - create by register */
	struct render_t * alone;

	/* private data */
	void * priv;
};

struct fb_t * get_default_framebuffer(void);
bool_t set_default_framebuffer(const char * name);

struct fb_t * search_framebuffer(const char * name);
bool_t register_framebuffer(struct fb_t * fb);
bool_t unregister_framebuffer(struct fb_t * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FB_H__ */
