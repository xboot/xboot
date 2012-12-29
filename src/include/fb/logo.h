#ifndef __FB_LOGO_H__
#define __FB_LOGO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fb/fb.h>
#include <graphic/surface.h>

bool_t display_logo(struct fb * fb);
bool_t register_logo(const struct gimage * logo);

#ifdef __cplusplus
}
#endif

#endif /* __FB_LOGO_H__ */
