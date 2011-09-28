#ifndef __FB_FONT_H__
#define __FB_FONT_H__

#include <xboot.h>
#include <fb/fb.h>
#include <graphic/surface.h>

struct gimage * lookup_console_font_face(u32_t code, u32_t fc, u32_t bc);

#endif /* __FB_FONT_H__ */
