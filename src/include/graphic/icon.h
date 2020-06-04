#ifndef __GRAPHIC_ICON_H__
#define __GRAPHIC_ICON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/color.h>
#include <graphic/region.h>
#include <graphic/matrix.h>
#include <graphic/font.h>

struct icon_t {
	uint32_t code;
	struct color_t * c;
	struct font_context_t * fctx;
	const char * family;
	int size;
	struct {
		int ox;
		int oy;
		int width;
		int height;
	} metrics;
};

void icon_init(struct icon_t * ico, uint32_t code, struct color_t * c, struct font_context_t * fctx, const char * family, int size);
void icon_set_code(struct icon_t * ico, uint32_t code);
void icon_set_color(struct icon_t * ico, struct color_t * c);
void icon_set_family(struct icon_t * ico, const char * family);
void icon_set_size(struct icon_t * ico, int size);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_ICON_H__ */
