#ifndef __GRAPHIC_FONT_H__
#define __GRAPHIC_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hmap.h>

struct font_context_t {
	void * library;
	struct hmap_t * map;
};

struct font_context_t * font_context_alloc(void);
void font_context_free(struct font_context_t * ctx);
void * font_search(struct font_context_t * ctx, const char * family);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_FONT_H__ */
