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
	/* device name */
	const char * name;

	/* initialize the framebuffer */
	void (*init)(struct fb_t * fb);

	/* clean up the framebuffer */
	void (*exit)(struct fb_t * fb);

	/* create a render */
	struct render_t * (*create)(struct fb_t * fb);

	/* destroy a render */
	void (*destroy)(struct fb_t * fb, struct render_t * render);

	/* present a render */
	void (*present)(struct fb_t * fb, struct render_t * render);

	/* ioctl framebuffer */
	int (*ioctl)(struct fb_t * fb, int cmd, void * arg);

	/* alone render - create by register */
	struct render_t * alone;
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
