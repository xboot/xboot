#ifndef __XUI_FONT_H__
#define __XUI_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <graphic/surface.h>
#include <graphic/region.h>
#include <graphic/color.h>

void font_draw(struct surface_t * s, struct region_t * clip, int x, int y, const char * utf8, struct color_t * c);

#ifdef __cplusplus
}
#endif

#endif /* __XUI_FONT_H__ */
