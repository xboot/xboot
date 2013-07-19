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
struct fb_info
{
	/* the framebuffer name. */
	const char * name;

	/* the framebuffer's surface */
	struct surface_t surface;
};

/*
 * defined the structure of framebuffer.
 */
struct fb
{
	/* the framebuffer information */
	struct fb_info * info;

	/* initialize the framebuffer */
	void (*init)(struct fb * fb);

	/* clean up the framebuffer */
	void (*exit)(struct fb * fb);

	/* swap framebuffer */
	void (*swap)(struct fb * fb);

	/* flush framebuffer */
	void (*flush)(struct fb * fb);

	/* ioctl framebuffer */
	int (*ioctl)(struct fb * fb, int cmd, void * arg);

	/* private data */
	void * priv;
};

struct fb * get_default_framebuffer(void);
bool_t set_default_framebuffer(const char * name);

struct fb * search_framebuffer(const char * name);
bool_t register_framebuffer(struct fb * fb);
bool_t unregister_framebuffer(struct fb * fb);

#ifdef __cplusplus
}
#endif

#endif /* __FB_H__ */
