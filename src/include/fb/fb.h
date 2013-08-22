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
	/* Framebuffer name */
	char * name;

	/* Initialize the framebuffer */
	void (*init)(struct fb_t * fb);

	/* Clean up the framebuffer */
	void (*exit)(struct fb_t * fb);

	/* Cursor position of x with offset */
	int (*xcursor)(struct fb_t * fb, int ox);

	/* Cursor position of y with offset */
	int (*ycursor)(struct fb_t * fb, int oy);

	/* Backlight brightness */
	int (*backlight)(struct fb_t * fb, int brightness);

	/* Create a render */
	struct render_t * (*create)(struct fb_t * fb);

	/* Destroy a render */
	void (*destroy)(struct fb_t * fb, struct render_t * render);

	/* Present a render */
	void (*present)(struct fb_t * fb, struct render_t * render);

	/* Suspend framebuffer */
	void (*suspend)(struct fb_t * fb);

	/* Resume framebuffer */
	void (*resume)(struct fb_t * fb);

	/* Alone render - create by register */
	struct render_t * alone;

	/* Private data */
	void * priv;
};

struct fb_t * search_framebuffer(const char * name);
struct fb_t * search_first_framebuffer(void);
bool_t register_framebuffer(struct fb_t * fb);
bool_t unregister_framebuffer(struct fb_t * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FB_H__ */
