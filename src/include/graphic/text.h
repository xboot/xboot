#ifndef __GRAPHIC_TEXT_H__
#define __GRAPHIC_TEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/color.h>
#include <graphic/region.h>
#include <graphic/matrix.h>
#include <graphic/font.h>

struct text_t {
	char * utf8;
	struct color_t c;
	struct font_context_t * fctx;
	char * family;
	int size;
	struct region_t e;
};

struct text_t * text_alloc(const char * utf8, struct color_t * c, struct font_context_t * fctx, const char * family, int size);
void text_free(struct text_t * txt);
void text_set_text(struct text_t * txt, const char * utf8);
void text_set_color(struct text_t * txt, struct color_t * c);
void text_set_font_family(struct text_t * txt, const char * family);
void text_set_font_size(struct text_t * txt, int size);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_TEXT_H__ */
