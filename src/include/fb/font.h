#ifndef __FB_FONT_H__
#define __FB_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/color.h>
#include <fb/render.h>

struct texture_t * lookup_console_font_face(struct render_t * render, u32_t code, struct color_t * fc, struct color_t * bc);

#ifdef __cplusplus
}
#endif

#endif /* __FB_FONT_H__ */
