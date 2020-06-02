#ifndef __GRAPHIC_FONT_H__
#define __GRAPHIC_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <xfs/xfs.h>

struct font_context_t {
	void * library;
	void * manager;
	void * cmap;
	void * sbit;
	void * image;
	struct list_head list;
};

struct font_context_t * font_context_alloc(void);
void font_context_free(struct font_context_t * ctx);
void * font_lookup_bitmap(struct font_context_t * ctx, const char * family, int size, uint32_t code);
void * font_lookup_glyph(struct font_context_t * ctx, const char * family, int size, uint32_t code);
void font_add(struct font_context_t * ctx, struct xfs_context_t * xfs, const char * family, const char * path);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_FONT_H__ */
