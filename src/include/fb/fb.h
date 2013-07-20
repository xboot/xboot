#ifndef __FB_H__
#define __FB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <graphic/surface.h>

/*
 * defined the structure of framebuffer information.
 */
struct fb_info_t
{
	/* the framebuffer name. */
	const char * name;

	/* current surface index */
	int index;

	/* the framebuffer's surface - triple buffering */
	struct surface_t * __surface[3];

	/* the framebuffer's surface */
	struct surface_t surface;
};

/*
 * defined the structure of framebuffer.
 */
struct fb_t
{
	/* the framebuffer information */
	struct fb_info_t * info;

	/* initialize the framebuffer */
	void (*init)(struct fb_t * fb);

	/* clean up the framebuffer */
	void (*exit)(struct fb_t * fb);

	/* swap framebuffer */
	void (*swap)(struct fb_t * fb);

	/* flush framebuffer */
	void (*flush)(struct fb_t * fb);

	/* present framebuffer */
	void (*present)(struct fb_t * fb);

	/* ioctl framebuffer */
	int (*ioctl)(struct fb_t * fb, int cmd, void * arg);

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
