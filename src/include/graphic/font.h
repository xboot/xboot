#ifndef __GRAPHIC_FONT_H__
#define __GRAPHIC_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <hmap.h>
#include <xfs/xfs.h>

struct font_context_t {
	void * library;
	void * manager;
	void * cmap;
	void * sbit;
	struct hmap_t * map;
};

struct font_bitmap_t {
	int width;
	int height;
	int left;
	int top;
	int pitch;
	int xadvance;
	int yadvance;
	unsigned char * buffer;
};

struct font_context_t * font_context_alloc(void);
void font_context_free(struct font_context_t * ctx);
void font_install(struct font_context_t * ctx, const char * family, const char * path);
void font_install_from_xfs(struct font_context_t * ctx, struct xfs_context_t * xfs, const char * family, const char * path);
void font_bitmap_lookup(struct font_context_t * ctx, const char * family, int size, uint32_t code, struct font_bitmap_t * bitmap);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_FONT_H__ */
