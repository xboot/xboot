#ifndef __FB_FONT_H__
#define __FB_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>
#include <graphic/surface.h>

struct gimage * lookup_console_font_face(u32_t code, u32_t fc, u32_t bc);

#ifdef __cplusplus
}
#endif

#endif /* __FB_FONT_H__ */
