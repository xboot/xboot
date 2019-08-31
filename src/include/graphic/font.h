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
	struct hmap_t * map;
};

struct font_context_t * font_context_alloc(void);
void font_context_free(struct font_context_t * ctx);
void font_install(struct font_context_t * ctx, const char * family, const char * path);
void font_install_from_xfs(struct font_context_t * ctx, struct xfs_context_t * xfs, const char * family, const char * path);
void font_uninstall(struct font_context_t * ctx, const char * family);
int search_glyph(struct font_context_t * ctx, const char * family, u32_t code, void ** face);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_FONT_H__ */
