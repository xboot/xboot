#ifndef __FB_H__
#define __FB_H__

#include <xboot.h>
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
	void (*init)(struct fb * fb);

	/* clean up the framebuffer */
	void (*exit)(struct fb * fb);

	/* map color */
	u32_t (*map_color)(struct fb * fb, u8_t r, u8_t g, u8_t b, u8_t a);

	/* unmap color */
	void (*unmap_color)(struct fb * fb, u32_t c, u8_t * r, u8_t * g, u8_t * b, u8_t * a);

	/* fill rect */
	bool_t (*fill_rect)(struct fb * fb, u32_t c, u32_t x, u32_t y, u32_t w, u32_t h);

	/* blit bitmap */
	bool_t (*blit_bitmap)(struct fb * fb, struct bitmap * bitmap, enum blit_mode mode, u32_t x, u32_t y, u32_t w, u32_t h, u32_t ox, u32_t oy);

	/* ioctl framebuffer */
	int (*ioctl)(struct fb * fb, int cmd, void * arg);

	/* private data */
	void * priv;
};


struct fb * search_framebuffer(const char * name);
bool_t register_framebuffer(struct fb * fb);
bool_t unregister_framebuffer(struct fb * fb);

#endif /* __FB_H__ */
