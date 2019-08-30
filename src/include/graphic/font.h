#ifndef __GRAPHIC_FONT_H__
#define __GRAPHIC_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <hmap.h>

struct font_context_t {
	void * library;
	struct hmap_t * map;
};

struct font_context_t * font_context_alloc(void);
void font_context_free(struct font_context_t * ctx);
void * search_face(struct font_context_t * ctx, const char * family);
int search_glyph(struct font_context_t * ctx, const char * family, u32_t code, void ** face);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_FONT_H__ */
